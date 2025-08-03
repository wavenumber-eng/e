
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <time.h>
    #include <sys/time.h>
#endif

#ifndef E_PORT_PC
#define E_PORT_PC

#define CONFIG__E_NULL			  	    		   (void *)0

#define E__PORT__HAS_TICK						     (1)

#endif
