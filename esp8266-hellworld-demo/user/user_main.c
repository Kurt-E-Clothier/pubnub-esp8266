/***************************************************************************
* 
* File              : user_main.c
*
* Author			: Kurt E. Clothier
* Date				: June 17, 2015
* Modified			: June 25, 2015
*
* Description       : Pubnub Demo
*
* Compiler			: Xtensa Tools - GCC
* Hardware			: ESP8266-x
* SDK Version		: 0.9.3 - 1.1.0
*
* More Information	: http://www.projectsbykec.com/
*					: http://www.pubnub.com/
*					: http://www.esp8266.com/
*
****************************************************************************/

/***************************************************************************
	- LED flashes while connecting to network, shuts off when connected.

	- Once connected, the device will subscribe to the specified channel.
		Due to the limitations of single-threading, the device cannot
		publish at will while waiting on a response from the subscription.
		Pushing the button will queue a publish event (this can simulate 
		the change in a sensor value). The publish will take place either
		immediately after another a message is published to the channel
		by any other device, or after a timeout (~ 300 seconds).

	- The device can be easily configured to only publish data, which can take
		place whenever a publish event is desired. Similarly, the subscribing
		can be handled automatically without worrying about publish events.
		These options or other custom confurations will depend upon
		your particular use case.

	- Publising "0" to the channel from any device (in the world!) will 
		turn off the LED. Publishing "1" will turn the LED on.

	- Serial Debugging available: rate (9600) set in include/user_config.h

	- You must put add your WiFi SSID and PW to include/user_config.h 
***************************************************************************/

/**************************************************************************
	Included Header Files
***************************************************************************/
#include "esp8266.h"
#include "pubnub.h"

/**************************************************************************
	Global Variables
***************************************************************************/
os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static volatile os_timer_t msTimer;
static volatile os_timer_t networkTimer;
static volatile int msCnt = 0;	// Counter for 1ms timer
static volatile int pubCnt = 0;	// Counter for Pubnub actions
static bool connected = false;
static volatile bool buttonWasPressed = false;

// Pubnub Attributes
static const char pubkey[] = "demo";
static const char subkey[] = "demo";
static const char channel[] = "esp8266-demo";

// This isn't really necessary, just prevents over-zealous publishing...
#define PUBLISH_DELAY	2000	// Minmum delay between publishes (milliseconds)

/**************************************************************************
	Local Function Prototypes
***************************************************************************/
static void IFA user_procTask(os_event_t *events);
static void IFA initUART(void);
static void IFA initGPIO(void);

static void IFA subscribeCB(char *m);	// Callback for Pubnub Subscriptions
static void IFA connectedCB(void);		// Callback for network connection
static void IFA network_checkIP(void);	// Check for IP address

static void IFA msISR(void *arg);	// 1 ms interrupt service routine
static void IFA toggleLED(void);

/**************************************************************************
	ESP8266 MAIN FUNCITONS
***************************************************************************/

// Required for SDK v1.1.0 - Can be used to disable RF
void user_rf_pre_init(void)
{
}

// Loop function - Not being used
static void IFA user_procTask(os_event_t *events)
{
	os_delay_us(10);
    //system_os_post(user_procTaskPrio, 0, 0 );
}

// User Initialization
void IFA user_init()
{
    struct station_config stationConf;
	const char ssid[SSID_LENGTH] = SSID;
    const char pw[PW_LENGTH] = SSID_PW;

	initUART();
	initGPIO();

	pubnub_init(pubkey, subkey);
	//pubnub_subscribe(channel, subscribeCB);

	// Create a timer - initially to toggle LED
	os_timer_disarm(&msTimer);
    os_timer_setfn(&msTimer, (os_timer_func_t *)msISR, NULL);
    os_timer_arm(&msTimer, 250, REPEAT);

	// Connect to network
	// Putting this here seems to work best (over external functions)
    wifi_set_opmode(STATION_MODE);
	stationConf.bssid_set = 0;	
    os_memcpy(&stationConf.ssid, ssid, SSID_LENGTH);
    os_memcpy(&stationConf.password, pw, PW_LENGTH);

    wifi_station_set_config(&stationConf);

	// Setup timer to check for IP address
	os_timer_disarm(&networkTimer);
	os_timer_setfn(&networkTimer, (os_timer_func_t *)network_checkIP, NULL);
	os_timer_arm(&networkTimer, IP_CHECK_DELAY, NO_REPEAT);

    //Start os task
    system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
    system_os_post(user_procTaskPrio, 0, 0 );

}

/**************************************************************************
	INTERRUPT SERVICE ROUTINES
***************************************************************************/

// 1 ms timer
static void IFA msISR(void *arg)
{
	// Only check button once connected to network
	if (connected) {
		
		// Do these things every 50 ms
		if(++msCnt == 50){

			// Check button
			if (PIN_IS_LO(0)) {	// Button is active low!

				// Only do this every so often...
				if (0 == pubCnt) {
					buttonWasPressed = true;
					pubCnt = PUBLISH_DELAY;
				}
			}

			// Publish if something has happened
			if (buttonWasPressed) {
				pubnub_publish(channel, "\"Hello World!\"");
				buttonWasPressed = false;
			}
			else
				pubnub_subscribe(channel, subscribeCB);

			msCnt = 0;
		}	

		// Decrement Publish Delay
		if (pubCnt)
			--pubCnt;
	}

	// Toggle LED until connected
	else 
		toggleLED();
}

/**************************************************************************
	UTILITIES
***************************************************************************/

// Check for IP address before continuing with connection...
static void IFA network_checkIP(void)
{
	struct ip_info ipconfig;
	os_timer_disarm(&networkTimer);
	wifi_get_ip_info(STATION_IF, &ipconfig);
	if (wifi_station_get_connect_status() == STATION_GOT_IP && ipconfig.ip.addr != 0) {
		connectedCB();
	}
   	else {
		DEBUG_PRINT(("  No ip found...\r\n"));
		os_timer_setfn(&networkTimer, (os_timer_func_t *)network_checkIP, NULL);
		os_timer_arm(&networkTimer, IP_CHECK_DELAY, NO_REPEAT);
	}
}

// Callback - Runs when connected to network
static void IFA connectedCB(void)
{
	pubnub_connect();
	connected = true;
	SET_PIN_LO(2);

	// setup 1ms timer
	os_timer_disarm(&msTimer);
    os_timer_arm(&msTimer, 1, REPEAT);
}

// Callback - Runs when someone publishes to subscribed channel(s)
static void IFA subscribeCB(char *m)
{
	DEBUG_PRINT(("\nSubscribe Callback:\n%s\n", m));
	if (m[1] == '0')
		SET_PIN_LO(2);
	else if(m[1] == '1')
		SET_PIN_HI(2);
}

// Toggle LED
static void IFA toggleLED(void)
{
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2)
		SET_PIN_LO(2);
    else
		SET_PIN_HI(2);
}

/**************************************************************************
	INITIALIZATION ROUTINES AND POWER MODES
***************************************************************************/

// Configure UART for serial debugging
static void IFA initUART(void)
{
	// BITRATE set in user_config.h
	uart_div_modify(0, UART_CLK_FREQ / BITRATE);
}

// Configure GPIO
static void IFA initGPIO(void)
{
	gpio_init();

	// Set GPIO 2 as output
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	SET_PIN_LO(2);

	// Set GPIO 0 as input (external pullup)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	SET_AS_INPUT(0);

	os_delay_us(1000);
}

