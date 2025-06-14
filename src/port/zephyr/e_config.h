#ifndef E__CONFIG_H_
#define E__CONFIG_H_

#define E__PORT_OPTION					E__PORT_OPTION__NXP__MCXN

#include <e_config__port.h>

#define CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL (CONFIG__E_TICK_PERIOD__MS)

#define CONFIG__E_VT100__ENABLE                    (1)

#define CONFIG__E_VT100_DEFAULT_COLOR               E_VT100__WHITE

#define CONFIG__E_LOG__ENABLE					   (1)

#define CONFIG__E_LOG__RTT_ENABLE				   (1)

#define CONFIG__E_LOG__RTT_TX_BUFFER_SIZE          (4096)

#define CONFIG__E_SHELL__ENABLE			    	   (0)

#ifndef CONFIG__E_BQ_MAX_PRINTF_LEN
    #define CONFIG__E_BQ_MAX_PRINTF_LEN			   (64)
#endif

#endif
