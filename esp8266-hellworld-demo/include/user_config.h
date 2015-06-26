/***************************************************************************
* 
* File              : user_config.h
*
* Author			: Kurt E. Clothier
* Date				: June 1, 2015
* Modified			: June 22, 2015
*
* Description       : Header File for Projects
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
// Define to print debug statements over UART using DEBUG_PRINT()
#define DEBUG_PRINT_	1

// Networking
#define SSID "<YOUR SSID>"
#define SSID_PW "<YOUR PASSWORD>"
#define SSID_PASSWORD SSID_PW

#define STATION_MODE	0x01
#define SSID_LENGTH		32
#define PW_LENGTH		32

#define IP_CHECK_DELAY	2000	// Time (s) to wait before (re)checking IP

// Configuration options
#define	BITRATE			9600



