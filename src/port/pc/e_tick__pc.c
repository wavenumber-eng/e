#include "e.h"

#if E__PORT == E__PORT_OPTION__PC


#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <time.h>
    #include <sys/time.h>
#endif

void e_tick__init()
{

}

uint32_t e_tick__get_ms()
{
    #if defined(_WIN32) || defined(_WIN64)
        // Windows: Use GetTickCount64() or timeGetTime()
        return (uint32_t)GetTickCount();  // Returns milliseconds since boot
    
    #elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS: Use clock_gettime() with CLOCK_MONOTONIC
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
            return (uint32_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
        }
        return 0; // Error fallback
    
    #else
        // Fallback for unknown platforms
        return e__ms_ticker;
    #endif
}

void e_tick__delay_ms(uint32_t delay_ms)
{
    #if defined(_WIN32) || defined(_WIN64)
        // Windows: Use Sleep() - millisecond precision
        Sleep(delay_ms);
    
    #elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS: Use nanosleep() - more precise and interruptible
        struct timespec req, rem;
        req.tv_sec = delay_ms / 1000;
        req.tv_nsec = (delay_ms % 1000) * 1000000;
        
        // Handle interruptions (like signals)
        while (nanosleep(&req, &rem) == -1) {
            if (errno == EINTR) {
                req = rem;  // Continue with remaining time
            } else {
                break;  // Other error, give up
            }
        }
    
    #else
        // Fallback for unknown platforms - your original implementation
        e__delay_ticker = 0;
        while (e__delay_ticker < delay_ms) {
            CONFIG__E_WFI;
        }
    #endif
}

#endif
