#include "e_config__port_option.h"

#ifndef _E_CONFIG__PORT_H_
#define _E_CONFIG__PORT_H_

#ifndef E__PORT
    #warning "Porting layer not defined for e, using skeleton"

    #define E__PORT E__PORT_OPTION__SKELETON

#else

    #if E__PORT == E__PORT_OPTION__NXP__MCXN

        #include <port/nxp__mcxn/e_port__nxp__mcxn.h>

    #elif E__PORT == E__PORT_OPTION__ZEPHYR

        #include <port/zephyr/e_port__zephyr.h>

    #elif E__PORT == E__PORT_OPTION__RPI__RP2XXX

        #include <port/rpi__rp2xxx/e_port__rpi_rp2xxx.h>

    #elif E__PORT == E__PORT_OPTION__RPI__RP2040

        #include <port/rpi__rp2xxx/e_port__rpi_rp2xxx.h>
        
    #elif E__PORT == E__PORT_OPTION__RPI__RP2350

        #include <port/rpi__rp2xxx/e_port__rpi_rp2xxx.h>

    #elif E__PORT == E__PORT_OPTION__RPI__RP2354

        #include <port/rpi__rp2xxx/e_port__rpi_rp2xxx.h>

    #elif E__PORT == E__PORT_OPTION__NXP__MCXA

        #include <port/nxp__mcxa/e_port__nxp__mcxa.h>

    #elif E__PORT == E__PORT_OPTION__NXP__LPC865

        #include <port/nxp__lpc865/e_port__nxp__lpc865.h>

   
    #elif E__PORT == E__PORT_OPTION__PC

         #include <port/pc/e_port__pc.h>

    #elif E__PORT == E__PORT_OPTION__TEST

         #include <port/test/e_port__test.h>

    #endif
       

#endif

#ifndef CONFIG_E_NULL
        #define CONFIG_E_NULL      NULL
#endif


#endif
