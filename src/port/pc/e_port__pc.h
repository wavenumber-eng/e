
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <time.h>
    #include <sys/time.h>
#endif

#ifndef E_PORT_PC
#define E_PORT_PC

#define CONFIG__E_LIST_NULL			  			   (void *)0

#define CONFIG__E_TICK_PERIOD__MS			       (1)

#define CONFIG__E_TICK_IRQ_PRIORITY			       (0)	  	      

#define CONFIG__E_WFI						       



#endif
