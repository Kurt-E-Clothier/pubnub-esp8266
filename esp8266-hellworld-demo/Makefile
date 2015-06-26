# Hey Emacs, this is a -*- makefile -*-
#################################################################
#
# ESP8266 Makefile Template
#
# Based on the example provided
# Thanks to:
# - zarya
# - Jeroen Domburg (Sprite_tm)
# - Christian Klippel (mamalala)
# - Tommie Gannert (tommie)
#
# Changelog:
# - 2014-10-06: Changed the variables to include the header file directory
# - 2014-10-06: Added global var for the Xtensa tool root
# - 2014-11-23: Updated for SDK 0.9.3
# - 2014-12-25: Replaced esptool by esptool.py
# - 2015-06-16: Increased readability, added section headers, expanded explanations
# - 2015-06-23: Updated for SDK 1.1.0 (Had to manually add some missing headers and libs)
#
# Modified by   : Kurt E. Clothier
# Date          : June 23, 2015
#
# Compile       : GCC -> xtensa toolchain with esptool.py
# Supports      : C
#
# More Info     : http://www.gnu.org/software/make/
#				: http://www.esp8266.com
#
#################################################################

#	You will likely have to modify the directory paths referenced 
#		to use this particular Makefile. They should be adequately
#		labeled in hopes of simplifying this task.
#
#	Primary Targets:
#		all - this is default.. it will: clean, build, clean
#		build - this will: clean, build
#		clean - this will: clean
#		flash - this will: flash the chip
#
#	Issuing a 'make' command is useful for testing compliation errors.
#	Programming the chip should be done as: make build && make flash
#	You might have to separate those commands and issue: sudo make flash'

#################################################################
#
#------------------- Project Specific Options -------------------
#
#################################################################
# name for the target project
TARGET		= pubnub_demo

# which modules (subdirectories) of the project to include in compiling
MODULES		= user pubnub 

# libraries used in this project, mainly provided by the SDK
LIBS		= c gcc hal pp phy net80211 lwip wpa main

# Redirect Error Messages
ERROR_MESSAGES	= 
#ERROR_MESSAGES	= $(TARGET).gccmessages

# SDK to use
#ESP8266_SDK		= esp_iot_sdk_v1.1.0
# This older one seems more stable...
ESP8266_SDK		= esp_iot_sdk_v0.9.3

#################################################################
#
#----------------------- Directory Options  ---------------------
#
#################################################################
# Base directory for the compiler
XTENSA_TOOLS_ROOT ?= /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin

# base directory of the ESP8266 SDK package, absolute
SDK_BASE		?= /opt/Espressif/$(ESP8266_SDK)
SDK_INCLUDE		?= $(SDK_BASE)/include/ 

#Esptool.py path and port
ESPTOOL		?= /opt/Espressif/esptool-py/esptool.py
ESPPORT		?= /dev/ttyUSB0

# Output directors to store intermediate compiled files
# relative to the project directory
BUILD_BASE	= build
FW_BASE		= firmware

# various paths from the SDK used in this project
SDK_LIBDIR	= lib
SDK_LDDIR	= ld

#################################################################
#
#----------------------------- Flags ----------------------------
#
#################################################################

#---------------- Compiler Options (c)----------------

# Generate debugging information
CFLAGS += -g

#----- Optimizer settings -----
# CFLAGS += -O0     # disable optimization (default)
# CFLAGS += -O      # equivalent to -O1
#CFLAGS += -O2       # most common optimization setting
# CFLAGS += -O3     # turn on -finline-functions, -funswitch-loops, etc.
# CFLAGS += -fast   # optimize for maximum performance
CFLAGS += -Os     # optimize for size at the expense of speed.
# CFLAGS += -Oz     # Apple OS X only size optimization setting.

#----- Warning Settings -----
# Make all warnings into errors
#CFLAGS += Werror
# Misnamed, enables a list of warning options, but not all of them.
#CFLAGS += -Wall
# Warn about anything depending on "sizeof" a funtion type or of void
CFLAGS += -Wpointer-arith 
# Warn if an undefined identifier is evaluated in an #if directive
CFLAGS += -Wundef
# Warn for missing prototypes.
#CFLAGS += -Wmissing-prototypes
# Warn if a function is declared or defined w/o specifying the argument types.
#CFLAGS += -Wstrict-prototypes
# Warn whenever a switch statement does not have a default case.
CFLAGS += -Wswitch-default
# Warn when a switch statement has an index of enumerate type and lacks a case
# for one or more of the named codes in the enumeration, or case labels outside
# the enumeration range.
CFLAGS += -Wswitch-enum
# Warn for a global function defined without a previous prototype.  Issued even
# if the definition itself provides a prototype.
#CFLAGS += -Wmissing-declarations
# Warn when a variable is shadowed, or a built-in function is shadowed.
CFLAGS += -Wshadow
# warn of user-supplied include dirs that are missing
CFLAGS += -Wmissing-include-dirs
# warn if precompiled header is found but can't be used
CFLAGS += -Winvalid-pch  
# Disallow self initialization <int i = i;>
CFLAGS += -Winit-self
# Disable automatic inlining
CFLAGS += -fno-inline-functions 

#----- Xtensa Settings -----
# Perform function long calls
CFLAGS += -mlongcalls 
# Literals are interspersed in the text section to keep them close to their references
CFLAGS += -mtext-section-literals  
# ???
CFLAGS += -D__ets__ -DICACHE_FLASH 

# Where to find included files
CFLAGS += -I. -I include/ -I $(SDK_INCLUDE) 

#----- Linker Settings -----
# Pass Options to Linker
CFLAGS += -Wl,-EL 
# Do not use the standard system starup files or libraries when linking
CFLAGS += -nostdlib 
# Include these libraries
CFLAGS += -L $(SDK_BASE)/$(SDK_LIBDIR)

#---------------- Linker Options ----------------

# linker flags used to generate the main object file
LDFLAGS		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

#################################################################
#
#----------------------- Memory Locations -----------------------
#
#################################################################
# we create two different files for uploading into the flash
# these are the names and options to generate them
FW_FILE_1	= 0x00000
FW_FILE_1_ARGS	= -bo $@ -bs .text -bs .data -bs .rodata -bc -ec
FW_FILE_2	= 0x40000
FW_FILE_2_ARGS	= -es .irom0.text $@ -ec

#################################################################
#
#----------------- Defining Executable Commands -----------------
#
#################################################################
# select which tools to use as compiler, librarian and linker
# This is a perl script to color code the output of the compiler
CC		:= $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc
AR		:= $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-ar
LD		:= $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc

# Various Terminal Commands
REMOVE		= rm -f
REMOVEDIR	= rm -rf
COPY		= cp
WINSHELL	= cmd
CLEAR		= clear
OUTPUT		= 2> $(ERROR_MESSAGES)

# linker script used for the above linkier step
LD_SCRIPT	= eagle.app.v6.ld

#################################################################
#
#------------------- Defining Output Messages -------------------
#
#################################################################
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_CLEANING = Cleaning project files:
MSG_CREATING_LIBRARY = Creating library:

#################################################################
#
#------------------ Adding Directory Prefixes -------------------
#
#################################################################
FW_TOOL		?= /usr/bin/esptool
SRC_DIR		:= $(MODULES)
BUILD_DIR	:= $(addprefix $(BUILD_BASE)/,$(MODULES))

SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))

SRC		:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC))
LIBS		:= $(addprefix -l,$(LIBS))
APP_AR		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app.a)
TARGET_OUT	:= $(addprefix $(BUILD_BASE)/,$(TARGET).out)

LD_SCRIPT	:= $(addprefix -T$(SDK_BASE)/$(SDK_LDDIR)/,$(LD_SCRIPT))

INCDIR	:= $(addprefix -I,$(SRC_DIR))

FW_FILE_1	:= $(addprefix $(FW_BASE)/,$(FW_FILE_1).bin)
FW_FILE_2	:= $(addprefix $(FW_BASE)/,$(FW_FILE_2).bin)

#################################################################
#
#----------------- Additional Variable Definitions ---------------
#
#################################################################
V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

vpath %.c $(SRC_DIR)

define compile-objects
$1/%.o: %.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(CFLAGS)  -c $$< -o $$@ 
endef

#################################################################
#
#------------------------- Make Targets -------------------------
#
#################################################################

# Compile the program and clean directories/files (default target)
all: begin clean-list checkdirs build-targets clean-again finish end

# Compile the program, ready for flash!
build: begin clean-list checkdirs build-targets finish end

# Program the target device
flash: firmware/0x00000.bin firmware/0x40000.bin
	-$(ESPTOOL) --port $(ESPPORT) write_flash 0x00000 firmware/0x00000.bin 0x40000 firmware/0x40000.bin

# Delete files and directories created during build
clean: begin clean-list end
	
#################################################################
#
#--------------------- Prerequisite Targets ---------------------
#
#################################################################
.PHONY: begin finish end all build-targets checkdirs build flash clean clean-list clean-again

# Messages
begin:
	@echo
	@echo $(MSG_BEGIN)
	
finish:
	@echo
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo

# Cleaning List
clean-list:
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(APP_AR)
	$(REMOVE) $(TARGET_OUT)
	$(REMOVEDIR) $(BUILD_DIR)
	$(REMOVEDIR) $(BUILD_BASE)
	$(REMOVE) $(FW_FILE_1)
	$(REMOVE) $(FW_FILE_2)
	$(REMOVEDIR) $(FW_BASE)
	@echo

# Cleaning List
clean-again:
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(APP_AR)
	$(REMOVE) $(TARGET_OUT)
	$(REMOVEDIR) $(BUILD_DIR)
	$(REMOVEDIR) $(BUILD_BASE)
	$(REMOVE) $(FW_FILE_1)
	$(REMOVE) $(FW_FILE_2)
	$(REMOVEDIR) $(FW_BASE)

# Build Targets
build-targets: $(TARGET_OUT) $(FW_FILE_1) $(FW_FILE_2)

# Create directory and nested directories if necessary
firmware:
	$(Q) mkdir -p $@

$(FW_FILE_1): $(TARGET_OUT)
	$(vecho) "FW $@"
	$(Q) $(FW_TOOL) -eo $(TARGET_OUT) $(FW_FILE_1_ARGS)

$(FW_FILE_2): $(TARGET_OUT)
	$(vecho) "FW $@"
	$(Q) $(FW_TOOL) -eo $(TARGET_OUT) $(FW_FILE_2_ARGS)

$(TARGET_OUT): $(APP_AR)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $(LIBS) $(APP_AR) -Wl,--end-group -o $@

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^

checkdirs: $(BUILD_DIR) $(FW_BASE)

$(BUILD_DIR):
	$(Q) mkdir -p $@

$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))

