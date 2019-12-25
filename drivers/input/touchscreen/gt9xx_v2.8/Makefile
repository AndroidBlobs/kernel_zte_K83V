#
# Makefile for the Goodix gt9xx touchscreen driver.
#
#subdir-ccflags-y += -DDEBUG
ccflags-y += -I$(srctree)/drivers/input/touchscreen/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/gt9xx_v2.8/firmware_config/$(CONFIG_TS_FIRMWARE)/
obj-$(CONFIG_TOUCHSCREEN_GT9XX_v28)	+= gt9xx.o
obj-$(CONFIG_TOUCHSCREEN_GT9XX_UPDATE)	+= gt9xx_update.o
obj-$(CONFIG_TOUCHSCREEN_GT9XX_TOOL)	+= goodix_tool.o
obj-y +=gt9xx_common_interface.o
