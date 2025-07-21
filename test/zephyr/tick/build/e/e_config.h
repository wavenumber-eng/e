#include "e_config__port_option.h"

#ifndef E__CONFIG_H_
#define E__CONFIG_H_

#ifndef E__PORT
    #define E__PORT				                (E__PORT_OPTION__ZEPHYR)
#endif

#include <e_config__port.h>

#ifndef CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL_MS
    #define CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL_MS (1)
#endif

#ifndef CONFIG__E_BQ_MAX_PRINTF_LEN
    #define CONFIG__E_BQ_MAX_PRINTF_LEN				   (256)
#endif

#ifndef CONFIG__E_VT100__ENABLE
    #define CONFIG__E_VT100__ENABLE                    (1)
#endif

#ifndef CONFIG__E_VT100_DEFAULT_COLOR
    #define CONFIG__E_VT100_DEFAULT_COLOR              (E_VT100__WHITE)
#endif

#ifndef CONFIG__E_LOG__ENABLE
    #define CONFIG__E_LOG__ENABLE					   (0)
#endif

#ifndef CONFIG__E_LOG__RTT_ENABLE
    #define CONFIG__E_LOG__RTT_ENABLE				   (0)
#endif

#ifndef CONFIG__E_LOG__RTT_TX_BUFFER_SIZE
    #define CONFIG__E_LOG__RTT_TX_BUFFER_SIZE          (0)
#endif

#ifndef CONFIG__E_SHELL__ENABLE
    #define CONFIG__E_SHELL__ENABLE			    	   (0)
#endif

#ifndef CONFIG__E_SHELL_PRINTF_MAX_LEN
    #define CONFIG__E_SHELL_PRINTF_MAX_LEN      	   (0U)
#endif

#ifndef CONFIG__E_SHELL_USE_HISTORY
    #define CONFIG__E_SHELL_USE_HISTORY 			   (0U)
#endif

#ifndef CONFIG__E_SHELL_SEARCH_IN_HIST
    #define CONFIG__E_SHELL_SEARCH_IN_HIST 			   (0U)
#endif

#ifndef CONFIG__E_SHELL_AUTO_COMPLETE
    #define CONFIG__E_SHELL_AUTO_COMPLETE 			   (0U)
#endif

#ifndef CONFIG__E_SHELL_BUFFER_SIZE
    #define CONFIG__E_SHELL_BUFFER_SIZE 			   (0)
#endif

#ifndef CONFIG__E_SHELL_MAX_ARGS
    #define CONFIG__E_SHELL_MAX_ARGS 				   (0U)
#endif

#ifndef CONFIG__E_SHELL_HIST_MAX
    #define CONFIG__E_SHELL_HIST_MAX 				   (0U)
#endif

#ifndef CONFIG__E_SHELL_MAX_CMD
    #define CONFIG__E_SHELL_MAX_CMD					   (0)
#endif

#endif
