#Creating IoT Devices with PubNub and the ESP8266

[image-schematic]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_schematic.png
[image-pins]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_pins.png
[image-breadboard1]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_breadboard1.png
[image-breadboard2]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_breadboard2.png
[image-console1]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_console1.png
[image-console2]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_console2.png
[image-debug1]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_debug1.png
[image-debug2]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_debug2.png
[image-debug3]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_debug3.png
[image-debug4]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_debug4.png
[image-debug5]: /home/kurt/pubnub/blog_posts/esp8266-pubnub-demo/images/esp8266_debug5.png

In case you hadn't heard, the internet of things is king of a big deal. Rather than bore you with the predictions of how many *billions* of devices will be connected in the next few years, I'm going to show you how to easily create an IoT device using the aweseome ESP8266 WiFi module and connect that device to *anyone*, *anywhere*, in real time with the power of PubNub.

Many projects encorporating the ESP8266 use it as an external module, communicaiting over a serial port with a complex set of AT commands. But this module is actually a programmable 32bit MCU with an embedded TCP stack, and if it speaks TCP, it speaks PubNub. With a bit of effort, we can write our own firmware to create a stand-alone, WiFi-enabled device!

<iframe src="https://vine.co/v/eJeJxTh9q0p/embed/simple" width="400" height="400" align="center" frameborder="0"></iframe><script src="https://platform.vine.co/static/scripts/embed.js"></script>

##Project Overview
For this project, I am using the tiny ESP-01, but any of the modules with access to GPIO0 and GPIO2 will work. The module is loaded with custom firmware, written in C, and compiled using the official SDK from Espressif on a Debian Linux machine. To start, here are a few things you need to get:

* ***ESP8266 Module*** - These are easily acquired <a href = "https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=esp8266&tbm=shop" target="_blank">online</a> for a few dollars each.
* ***USB to Serial Module*** - For programming and debugging - it must use 3.3V signals! This is a great one from <a href = "https://www.sparkfun.com/products/9873" target="_blank">Sparkfun</a>.
* ***Development Environment*** - This is the most complicated part, so I will direct you to the <a href = "https://.com/esp8266/esp8266-wiki/wiki/Toolchain" target="_blank">official documentation</a>.
* ***PubNub*** - Sign up for a free account <a href = "http://www.pubnub.com/get-started/" target="_blank">here</a>, and then get your own keys <a href = "http://admin.pubnub.com" target="_blank">here</a>. 

**Source Code Repository: http://github.com/Kurt-E-Clothier/pubnub-esp8266**

With that out of the way, we can get down to business. Like I said before, we will be writing custom firmware to load onto the ESP8266 which can send and receive data to and from any other PubNub enabled device. With over <a href = "http://www.pubnub.com/developers/" target="_blank">70 official SDKs</a>, that shouldn't be a problem! Unfortunately, the ESP8266 is not currently an *officially* supported device, but never fear - I've created a basic library for use with this module that should get you going in no time! The library builds off of the <a href = "http://www.pubnub.com/http-rest-push-api/" target="_blank">PubNub REST API</a>, but more on that later...

There is one small caveat; however, the actual networking code used in this project is not the most robust. It will connect you to a WiFi network, but it might not keep you connected through unforseen events. This will obviously affect the connection to PubNub, and you may notice intermentent performance issues after some period of time. The quick and dirty fix is a timer-based soft-reset, but adding more robust and performance proven connect/disconnect routines would be a more proper way to go. As with any aspect of an embedded system, the solution relies on your specific use case.

## The Hardware

If you want to build a *thing*, you obviously need some real life *things*. For this demo, I am using the following components:
* ***Breadboard*** - Also called a prototyping board; we will use this to connect the other components.
* ***ESP8266*** - You do have at least one of these by now, don't you?
* ***Push Button*** - x2 (normally open, momentary).
* ***LED*** - x2 (One is an optional power indicator).
* ***10k Resistor*** - x2 (Color Code: Brown, Black, Orange).
* ***1k Resistor*** - x2 (Color Code: Brown, Black, Red).
* ***0.1uF Ceramic Capacitor*** - This is optional, but does help with performance.
* ***Jumper Wire*** - So as to connect stuff together!

####Circuit Design
The power source for this project will be the actual USB connection. The USB to Serial module should have its own 3.3V regulator that we can then use to power the ESP8266. The ESP8266 is **not** 5V tolerant. I cannot stress this enough! You **must** power it and communicate using 3.3V signals, or you will be a very sad panda. With that PSA out of the way, let's take a look at the circuit.

![image][image-schematic]

You should notice that the transmit pin (TX) of the ESP goes to the receive pin (RX) of the USB to serial module, and vice versa. Here is a quick rundown of what every part is doing:
* **U1** - ESP8266 module.
* **U2** - USB to Serial module.
* **C1** - 0.1uF ceramic capacitor - (optional) "Decoupling Capcitor" to increase power line stability.
* **R1** - 10k Ohm "Pullup" Resistor - Keeps the chip from resetting by providing a "weak" path to 3.3V.
* **SW1** - Normally Open, Momentary Switch (push button) - Resets the chip by connecting "Reset" to Ground.
* **R2** - 1k Ohm Resistor - Limits the current through LED1.
* **LED1** - An LED we will control with the ESP8266.
* **R3** - 10k Ohm "Pullup" Resistor - Keeps GPIO0 "pulled high."
* **SW2** - Normally Open, Momentary Switch (push button) - Connects GPIO0 to Ground.
* **R4** - 1k Ohm Resistor - Limits the current through LED2.
* **LED2** - Power status indicator (optional).

####Wiring It Up

It's time to connect all of the components together on the breadboard. To start, let's take a look at the pinout for the ESP module I'm using:

![image][image-pins]

The pinout of your particular USB to Serial module may vary, so you'll have to consult the user guide for that. Building the circuit on a breadboard should result in something like this:
![image][image-breadboard1]

The jumper wire coming off of the USB to Serial module is for the 3.3V power supply because the 'VCC' pin is wired to 5V, so I am not using that (for the previously discussed reasons). Also, the 'ESP8266 Socket' is just an easy way to plug the module into the breadboard because of it's pin spacing. It is not required, but making your own might be a good idea! With the ESP8266 pluggen in, the cicuit will look like this:

![image][image-breadboard2]

The ESP8266 has a few different operational modes, depneding upon the state of some of the pins. If GPIO0 is held low (connected to ground) when the chip resets, it will enter the 'UART download boot mode' which allow us to program the chip's flash memory. If you recall the circuit, we are using GPIO0 to read an active low push button, so this works out nicely. To program the chip, we first push and hold reset, push the GPIO0 push button, release reset, and finally release the GPIO0 push button. If you are not used to directly programming these types of chips, this might seem strange to you, but trust me, you will get used to it quickly.

##The Firmware

With the circuit built, it's time to check out the code will be installing. If you haven't yet setup your <a href = "https://github.com/esp8266/esp8266-wiki/wiki/Toolchain" target="_blank">development environment</a>, do so now! As mentioned, this is the most difficult part of the whole project, but there are numreous online tutorials for doing this in various operating systems. If you run into any problems, consult the <a href = "http://www.esp8266.com/" target="_blank">ESP8266 Community Forum</a> for advice.
___
**FUN FACT:** The difference between software and firmware is the actual purpose of the code. Firmware is a subset of software which is installed directly to a chip's memory rather than on top of some other operating system. For example, *VLC Media Player* is **software**, while the code that controls your microwave or the BIOS on your computer is **firmware**. Embedded devices typically run firmware, but this is not an absolute.
___

The code is hosted on GitHub, so to get your copy, clone or download the repository at: http://github.com/Kurt-E-Clothier/pubnub-esp8266
The actual code is in the directory *esp8266-helloworld-demo*. If you have been developing your own firmware for this module, the directory structure should look familiar. The main source code is found in *user*, while included header files are place in *include*. The pubnub source code is found in the *pubnub* directory - imagine that! Compiled builds are found in *build*, and the files to be written to memory are in *firmware*.


####Operation

When first powered on, LED1 will flash twice per second. This will continue until the device has connected to the internet. At this time, a connection to PubNub will be established. This demo encorporates both publishing and subscribing to a channel. Unforunately, publish events cannot take place until a subscribe event takes place or times out. This is due to limitations in single-threaed devices. The provided libary can be adjusted for a particular need (such as only publishing, or automatically subscribing) or be reworked to encorporate threading if a different SDK is used. I will leave such tasks up to you, as the exact operation of the device will be specific to your needs!

The device will subscribe to the channel "esp8266-demo". Any other device writing to this channel can send commands to our ESP8266! For this demo, we will use the <a href = "http://www.pubnub.com/console/" target="_blank">developer's console</a>, but any mobile app, web page, or other application can be used.

A message of "1" will turn on LED1, while "0" will turn it off. Pushing SW2 will request a publish which will take place at either the next subscription message received or a timeout, due to the limitations previously discussed. Removing the "Subscribe" section of code or otherwise adjusting the overall program flow will allow for real time publishing.

####Makefile

I have provided a Makefile which will build the outputs and program a connected device; however, to use it you will have to manually adjust the location of your toolchain paths. If you have installed the toolchain based on the instructions I previously linked to, there should be minimal changes required.

The following primary targets are available for use:
* **all** - this is default.. it will: clean, build, clean
* **build** - this will: clean, build
* **clean** - this will: clean
* **flash** - this will: flash the chip

For basic compilation checks, simply issuing a 'make' command is quick and easy. To actually program the chip, you will have to issue 'make build' followed by 'make flash' (or possibly 'sudo make flash'). A working build is included in the repository, so once you have the Makefile adjusted, you can use it to flash your chip; however, until you manually configure your WiFi settings, the device will be unable to connect to the internet.

####Main

When directly programming common microcontrollers like the Atmel AVR line, you can create a main function containing an internal "forever loop." This is not the case with the ESP8266. This module uses hooks, similar to Arduino, so the compiler will expect a few defined functions. After that, you can schedule a timer to handle repeated tasks.

The most common hooks in use are:

Gist ID: 7756459c939b76e021c5
File: esp8266_hooks.c

    // Required for SDK v1.1.0 - Can be used to disable RF
    void user_rf_pre_init(void)
    {
    }

    // Loop function - Can/will continuously reschedule itself to run
    static void IFA user_procTask(os_event_t *events)
    {
        os_delay_us(10);
        //system_os_post(user_procTaskPrio, 0, 0 );
    }

    // User Initialization
    void IFA user_init()
    {
        // Initialize the device here
    }

Othwerwise, you are free to write your own functions and call them like a normal C program. The "IFA" attribute is actually a macro I defined in *include/esp8266.h* as:

Gist ID: 7756459c939b76e021c5
File: IFA_header.h

    #define IFA ICACHE_FLASH_ATTR

If a function is prefixed with IFA, it is written to flash which is mapped to ROM. Without this attribute, the function will end up in the instruction RAM segment, which is faster, but will fill up quickly. If at all possible, always use the IFA attribute. This header file also includes most all of the SDK libraries commonly used as well as remapping a few common C functions (like *printf*) to the version required by the Espressif SDK (*os_printf*). Lastly, it defines a simple debug printing macro which can be used to selectively print statements when a certain value is defined, which it is in the *include/user_config.h* file. I use this macro extensively in the library, but be careful with it as it can easily be broken.

Gist ID: 7756459c939b76e021c5
File: debug_print.h

    // Define to print debug statements over UART using DEBUG_PRINT()
    #define DEBUG_PRINT_	1

    // Only prints lines for debugging purposes
    #ifdef DEBUG_PRINT_
    #	define DEBUG_PRINT(S)	printf S
    #else
    #	define DEBUG_PRINT(S)
    #endif

Inside of the *user_init()* function, we will initialize the UART connection, setup the GPIO pins, and connect to WiFi. The values of *SSID* and *SSID_PW* are define in *include/user_config.h*. You must set these values to the corresponding SSID and password for your home WiFi connection! The overall code flow goes somthing like this:
1.Initialize UART
2.Initialize GPIO
3.Initialize PubNub
4.Connect to network (pause here until connection established)
5.Connect to Pubnub
6.Monitor button (SW2) and control LED1 based on messages received.

####PubNub Library

The provided library defines the following functions in *pubnub/pubnub.h*.

Gist ID: 7756459c939b76e021c5
File: pubnub_api.h

    /** 
     * Creates a connection to Pubnub
     * This should be called when a network connection is established!
     */
    void IFA pubnub_connect(void);

    /** 
     * Initialize the PubNub Connection
     */
    void IFA pubnub_init(const char *publish_key, const char *subscribe_key);

    /** 
     * Publish message on channel using Pubnub.
     * JSON type message should be used, Unicode characters are injected automatically...
     */
    bool IFA pubnub_publish(const char *channel, const char *message);

    /** 
     * Subscribe to a Pubnub channel
     */
    bool IFA pubnub_subscribe(const char *channel, Pubnub_subscribeCB subCB);

    /** 
     * Unsubscribe to a Pubnub channel
     */
    bool IFA pubnub_unsubscribe(const char *channel);

The usage of each of these functions is clearly shown in *user/user_main.c*. I have also supplied the full C source code instead of a typical lib file, so feel free to poke around inside and alter the code to be more specific to your tasks. The primary force behind the library is the <a href = "http://www.pubnub.com/http-rest-push-api/" target="_blank">PubNub REST API</a>. An HTTP Get request string is build and used for all functionality. Using these basic building blocks, other PubNub functionality can be added including <a href ="http://www.pubnub.com/products/" target ="_blank">Presence, History, and Security</a>.

##Using the PubNub Developer's Debug Console

PubNub has supplied an easy to use console for quickly testing accplications. Using the console can get you up and running much faster than developing a full web interface, or can easily test different aspects of your overall program. You can find the console <a href ="http://www.pubnub.com/console/" target ="_blank">here</a>.

In the console, type in your unique publish and subscribe keys, the channel to listen on, and push "Subscribe.

![image][image-console1]

You can then send messages to the channel as well as monitor the channel for all messages published!

![image][image-console2]
##Serial Debugging

If you plan on modifying the code to do your bidding (and I highly encourage you to do just that) you will want to make use of the DEBUG_PRINT() statements scattered about the library. These will print information through the serial port whenever various events take place. The UART is initialized to a baudrate of 9600, but this is easily modified in *include/user_config.h*. In Linux, I am using the program *gtkterm* for serial bugging. A typical run through of this demo will look something like this:

![image][image-debug1]

After this initial message is sent, PubNub will send an empty response. Now, we can subscribe to a channel. The first subscribe call will use a Timetoken of '0'. Pubnub will respond with a valid timetoken which should be used in the next subscribe call.

![image][image-debug2]

From here on, every resonse after a subscribe will contain a new Timetoken. This new Timetoken should always be used in the next subscribe call. After a subscribe call, the program will wait for a response or a timeout. When a message is received, it will be displayed, and the user defined subscribe callback function in *user_main.c* will be called.

![image][image-debug3]

If the button on GPIO0 is pushed, a publish request will be made. You should see a "Waiting on subscription(s)" message. The publish will then happen after the next message is received or a timeout occurs (in roughly 300 seconds). After the publish event, a new subscribe call using the last Timetoken is made again.

![image][image-debug4]

Note, that any messages sent from this device to a subscribed channel will be received on this device as well.

![image][image-debug5]

##Wrapping Up

That about does it for this demo. I hope you have seen how easy it can be to create IoT devices using an ESP8266 module, and the incredible amount of versatility PubNub offers to that device. You can communicate with your *thing* in real time from **any other PubNub enabled device in the world**! The presented library can also be customized for your specific needs. Functionality can easily be added or removed, and the basics shown here can really be applied to any other chip you want to use that has TCP capabilities. So go forth, and create something amazing! Just be sure to let us know what you come up with.




