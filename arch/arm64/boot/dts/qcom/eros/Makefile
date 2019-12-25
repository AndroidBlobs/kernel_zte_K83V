ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
	dtbo-y = sdm439-mtp_$(ZTE_BOARD_NAME)-overlay.dtbo
	sdm439-mtp_$(ZTE_BOARD_NAME)-overlay.dtbo-base := sdm439-mtp_$(ZTE_BOARD_NAME).dtb
else
	dtb-y = sdm439-mtp_$(ZTE_BOARD_NAME).dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
