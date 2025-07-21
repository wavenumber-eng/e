#include "e.h"

#if E__PORT == E__PORT_OPTION__PC


#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <time.h>
    #include <sys/time.h>
#endif

void e_tick_thread_cleanup(void)
{
    #if defined(_WIN32) || defined(_WIN64)
        if (thread_handle != NULL) {
            thread_running = 0;
            // Wait max 5 seconds for thread to exit
            if (WaitForSingleObject(thread_handle, 5000) == WAIT_TIMEOUT) {
                TerminateThread(thread_handle, 0);  // Force kill if needed
            }
            CloseHandle(thread_handle);
            thread_handle = NULL;
        }
        
    #elif defined(__linux__) || defined(__APPLE__)
        if (thread_running) {
            thread_running = 0;
            
            // Give thread time to exit gracefully
            struct timespec timeout = {5, 0};  // 5 seconds
            #ifdef __linux__
            if (pthread_timedjoin_np(thread_handle, NULL, &timeout) != 0) {
                pthread_cancel(thread_handle);  // Force cancel if timeout
            }
            #else
            // macOS doesn't have pthread_timedjoin_np, so just join
            pthread_join(thread_handle, NULL);
            #endif
        }
    #endif
}


// Platform-specific thread variables
#if defined(_WIN32) || defined(_WIN64)
    static HANDLE thread_handle = NULL;
    static volatile int thread_running = 0;

    unsigned __stdcall thread_e_entry_point(void* arg)
    {
        (void)arg;
        
        e__init();
        thread_running = 1;

        while (thread_running)
        {
            e_tick__irq();
            Sleep(1);  // 1ms delay
        }
        
        return 0;
    }

#elif defined(__linux__) || defined(__APPLE__)
    static pthread_t thread_handle;
    static volatile int thread_running = 0;

    void* thread_e_entry_point(void* arg)
    {
        (void)arg;
        
        e__init();
        thread_running = 1;

        struct timespec req;
        req.tv_sec = 0;
        req.tv_nsec = 1000000;  // 1ms in nanoseconds

        while (thread_running)
        {
            e_tick__irq();
            
            struct timespec rem;
            while (nanosleep(&req, &rem) == -1) {
                if (errno == EINTR) {
                    req = rem;
                } else {
                    break;
                }
            }
        }
        
        return NULL;
    }
#endif

int32_t e_port__tick_hw_init()
{

    atexit(e_tick_thread_cleanup);

    #if defined(_WIN32) || defined(_WIN64)
        if (thread_handle != NULL) {
            return -1;  // Already initialized
        }
        
        thread_handle = (HANDLE)_beginthreadex(
            NULL,           // Security
            0,              // Stack size (default)
            thread_e_entry_point,
            NULL,           // Argument
            0,              // Creation flags
            NULL            // Thread ID
        );
        
        if (thread_handle == NULL) {
            return -1;  // Failed to create thread
        }
        
        // Optional: Set thread priority
        SetThreadPriority(thread_handle, THREAD_PRIORITY_NORMAL);
        
        return 0;
    
    #elif defined(__linux__) || defined(__APPLE__)
        if (thread_running) {
            return -1;  // Already initialized
        }
        
        int result = pthread_create(&thread_handle, NULL, thread_e_entry_point, NULL);
        if (result != 0) {
            return -1;  // Failed to create thread
        }
        
        return 0;
    
    #else
        return -1;  // Unsupported platform
    #endif
}

#endif
