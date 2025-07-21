#include "stdint.h"

#include "e_config.h"

#ifndef E__DEBUG_H_
#define E__DEBUG_H_

#if CONFIG__E_LOG__RTT_ENABLE == 1

#include "external/rtt/SEGGER_RTT.h"

#define SEGGER_RTT_LOG(...) SEGGER_RTT_printf(0,__VA_ARGS__);

#else

#define SEGGER_RTT_LOG

#endif


#if CONFIG__E_LOG__ENABLE  != (0)

#define E_LOG(...) SEGGER_RTT_LOG(__VA_ARGS__);

#define E_LOG_INF(...) SEGGER_RTT_LOG(__VA_ARGS__);
#define E_LOG_DBG(...) SEGGER_RTT_LOG(__VA_ARGS__);
#define E_LOG_ERR(...) SEGGER_RTT_LOG(__VA_ARGS__);

#else

#define E_LOG(...)
#define E_LOG_INF(...);
#define E_LOG_DBG(...);
#define E_LOG_ERR(...);
#endif


/*
 * Aliases
 *
 */

#define E_DBG(...)   E_LOG(__VA_ARGS__)
#define E_ERR(...)   E_LOG_ERR(__VA_ARGS__)

#define E_MSG(...)   E_LOG(__VA_ARGS__)
#define E_DEBUG(...) E_LOG(__VA_ARGS__)

#define E_DEBUG(...) E_LOG(__VA_ARGS__)


#endif
