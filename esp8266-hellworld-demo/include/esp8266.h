/***************************************************************************
* 
* File              : esp8266.h
*
* Author			: Kurt E. Clothier
* Date				: June 1, 2015
* Modified			: June 23, 2015
*
* Description       : Header File for all Projects
*
* Compiler			: Xtensa Tools - GCC
* Hardware			: ESP8266-x
*
* More Information	: http://www.projectsbykec.com/
*					: http://www.pubnub.com/
*					: http://www.esp8266.com/
*
****************************************************************************/

/**************************************************************************
	Definitions 
***************************************************************************/
#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

#define IFA		ICACHE_FLASH_ATTR

#define REPEAT		1
#define NO_REPEAT	0

/**************************************************************************
	Included Header Files
***************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "c_types.h"
#include "espconn.h"
#include "mem.h"

/**************************************************************************
	GPIO Handling
***************************************************************************/
// Until I better understand the register mapping for GPIO pins, 
// we'll have to rely on functions/macros from the SDK for this.
#define PIN_IS_HI(P)	(GPIO_INPUT_GET(GPIO_ID_PIN(P)))
#define PIN_IS_LO(P)	!PIN_IS_HI(P)

#define SET_PIN_HI(P)	gpio_output_set(BIT ## P, 0, BIT ## P, 0)
#define SET_PIN_LO(P)	gpio_output_set(0, BIT ## P, BIT ## P, 0)

#define SET_AS_INPUT(P)	gpio_output_set(0, 0, 0, BIT ## P)

/**************************************************************************
	Common Function Mapping
***************************************************************************/
#ifndef printf
#define printf	os_printf
#endif

#ifndef sprintf
#define sprintf	os_sprintf
#endif

#ifndef memcpy
#define memcpy	os_memcpy
#endif

#ifndef strlen
#define strlen	os_strlen
#endif

// Because... bad English...
#ifndef espconn_send
#define espconn_send	espconn_sent
#endif

// SDK Backwards Compatibility
#ifndef os_timer_done
#define os_timer_done ets_timer_done
#endif

#ifndef os_timer_handler
#define os_timer_handler ets_timer_handler_isr
#endif

#ifndef os_timer_init
#define os_timer_init ets_timer_init
#endif

#ifndef os_update_cpu_frequency
#define os_update_cpu_frequency ets_update_cpu_frequency
#endif

/**************************************************************************
	Other Helpful Macros
***************************************************************************/

// Only prints lines for debugging purposes
#ifdef DEBUG_PRINT_
#	define DEBUG_PRINT(S)	printf S
#else
#	define DEBUG_PRINT		
#endif

