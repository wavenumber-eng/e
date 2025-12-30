#ifndef _E_CONFIG__PORT_OPTION_H_
#define _E_CONFIG__PORT_OPTION_H_

#define E__PORT_OPTION__SKELETON    (0x0000)
#define E__PORT_OPTION__PC          (0x0001)
#define E__PORT_OPTION__ZEPHYR      (0x0002)
#define E__PORT_OPTION__TEST        (0x0003)
 

#define E__PORT_OPTION__NXP__MCXA   (0x1000)
#define E__PORT_OPTION__NXP__MCXN   (0x1100)
#define E__PORT_OPTION__NXP__LPC865 (0x1200)

#define E__PORT_OPTION__RPI__RP2XXX (0x2000)
#define E__PORT_OPTION__RPI__RP2040 (0x2040)
#define E__PORT_OPTION__RPI__RP2350 (0x2350)
#define E__PORT_OPTION__RPI__RP2354 (0x2354)


// Ports should indicate these systems as (1) or (0)
 // E__PORT__HAS_TICK

#endif
