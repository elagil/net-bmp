##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Enable semihosting, if required.
# SEMIHOSTING_OPT = --specs=rdimon.specs -lc -lrdimon

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O0 -ggdb -fomit-frame-pointer -falign-functions=16 $(SEMIHOSTING_OPT)
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT =
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data.
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT =
endif

# Enable this if you want link time optimizations (LTO).
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x800
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x800
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = hard
endif

# FPU-related options.
ifeq ($(USE_FPU_OPT),)
  USE_FPU_OPT = -mfloat-abi=$(USE_FPU) -mfpu=fpv4-sp-d16
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, target, sources and paths
#
PROJECT = net-bmp
VERSION = $(shell git describe --tags --abbrev=0)
COMMIT_HASH = $(shell git rev-parse --short HEAD)
$(info Building "$(PROJECT)" version "$(VERSION)-$(COMMIT_HASH)")

# Target settings.
MCU  = cortex-m4

# Imported source files and paths.
CHIBIOS  := ./chibios
CONFDIR  := ./cfg
BUILDDIR := ./build
DEPDIR   := ./.dep
BOARDDIR := ./board

# Libraries.
LWIPDIR  := lwip/src
BMDIR    := blackmagic/src

# Important paths.
SOURCEDIR := ./source/
AUTOBUILD_ROOT := $(SOURCEDIR)

# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(BOARDDIR)/board.mk
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk
# Stream support.
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS)/os/various/shell/shell.mk
# Auto-build files in ./source recursively.
# This does NOT add include directories recursively - only SOURCEDIR.
include make/autobuild.mk
# lwip support.
include make/lwip.mk
# blackmagic library.
include make/blackmagic.mk

# Define linker script file here.
LDSCRIPT= ./STM32F407xE.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(ALLCSRC) \
       $(TESTSRC) \
       $(CHIBIOS)/os/various/evtimer.c \
       $(CHIBIOS)/os/various/syscalls.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global setting.
CPPSRC = $(ALLCPPSRC)

# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
INCDIR = $(CONFDIR) $(ALLINC) $(TESTINC) $(SOURCEDIR)/compat

# Define C warning options here.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes -Werror

# Define C++ warning options here.
CPPWARN = -Wall -Wextra -Wundef

#
# Project, target, sources and paths
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = \
  -DCHPRINTF_USE_FLOAT \
  -DSHELL_CMD_TEST_ENABLED=0 \
  -DVERSION=\"$(VERSION)\" \
  -DCOMMIT_HASH=\"$(COMMIT_HASH)\" \
  -DPC_HOSTED=0 \
  -DNO_LIBOPENCM3 \
  -DFIRMWARE_VERSION=\"$(VERSION)\" \
  $(BMDEF)

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user section
##############################################################################

##############################################################################
# Common rules
#

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################

##############################################################################
# Custom rules
#

#
# Custom rules
##############################################################################
