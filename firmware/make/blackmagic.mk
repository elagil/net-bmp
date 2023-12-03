BMTARGETDIR = $(BMDIR)/target

BMTARGETSRC = \
	$(BMTARGETDIR)/adiv5.c        \
	$(BMTARGETDIR)/adiv5_jtag.c   \
	$(BMTARGETDIR)/adiv5_swd.c    \
	$(BMTARGETDIR)/cortex.c       \
	$(BMTARGETDIR)/cortexar.c     \
	$(BMTARGETDIR)/cortexm.c      \
	$(BMTARGETDIR)/efm32.c        \
	$(BMTARGETDIR)/hc32l110.c     \
	$(BMTARGETDIR)/imxrt.c        \
	$(BMTARGETDIR)/jtag_devs.c    \
	$(BMTARGETDIR)/jtag_scan.c    \
	$(BMTARGETDIR)/lmi.c          \
	$(BMTARGETDIR)/lpc_common.c   \
	$(BMTARGETDIR)/lpc11xx.c      \
	$(BMTARGETDIR)/lpc17xx.c      \
	$(BMTARGETDIR)/lpc15xx.c      \
	$(BMTARGETDIR)/lpc40xx.c      \
	$(BMTARGETDIR)/lpc43xx.c      \
	$(BMTARGETDIR)/lpc546xx.c     \
	$(BMTARGETDIR)/lpc55xx.c      \
	$(BMTARGETDIR)/kinetis.c      \
	$(BMTARGETDIR)/msp432e4.c     \
	$(BMTARGETDIR)/msp432p4.c     \
	$(BMTARGETDIR)/nrf51.c        \
	$(BMTARGETDIR)/nrf91.c        \
	$(BMTARGETDIR)/nxpke04.c      \
	$(BMTARGETDIR)/rp.c           \
	$(BMTARGETDIR)/sam3x.c        \
	$(BMTARGETDIR)/sam4l.c        \
	$(BMTARGETDIR)/samd.c         \
	$(BMTARGETDIR)/samx5x.c       \
	$(BMTARGETDIR)/sfdp.c         \
	$(BMTARGETDIR)/spi.c          \
	$(BMTARGETDIR)/ch32f1.c       \
	$(BMTARGETDIR)/stm32f1.c      \
	$(BMTARGETDIR)/stm32f4.c      \
	$(BMTARGETDIR)/stm32h5.c      \
	$(BMTARGETDIR)/stm32h7.c      \
	$(BMTARGETDIR)/stm32mp15.c    \
	$(BMTARGETDIR)/stm32l0.c      \
	$(BMTARGETDIR)/stm32l4.c      \
	$(BMTARGETDIR)/stm32g0.c      \
	$(BMTARGETDIR)/at32f43x.c     \
	$(BMTARGETDIR)/renesas.c      \
	$(BMTARGETDIR)/target.c       \
	$(BMTARGETDIR)/target_flash.c \
	$(BMTARGETDIR)/target_probe.c \
	$(BMTARGETDIR)/gdb_reg.c

# BMSRC = \
# 	$(BMDIR)/command.c      \
# 	$(BMDIR)/crc32.c        \
# 	$(BMDIR)/exception.c    \
# 	$(BMDIR)/gdb_hostio.c   \
# 	$(BMDIR)/gdb_packet.c   \
# 	$(BMDIR)/gdb_main.c     \
# 	$(BMDIR)/hex_utils.c    \
# 	$(BMDIR)/maths_utils.c  \
# 	$(BMDIR)/morse.c        \
# 	$(BMDIR)/remote.c       \
# 	$(BMDIR)/timing.c       \
# 	$(BMDIR)/rtt.c

BMINC = \
	$(BMDIR)/include \
	$(BMDIR)/target

BMDEF = \
	-DENABLE_DEBUG=0

# Shared variables
ALLCSRC += $(BMSRC) $(BMTARGETSRC)
ALLINC  += $(BMINC)
