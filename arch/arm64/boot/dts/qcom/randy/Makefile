ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
	dtbo-y = sdm450-pmi632-mtp-s3_$(ZTE_BOARD_NAME)-overlay.dtbo
	sdm450-pmi632-mtp-s3_$(ZTE_BOARD_NAME)-overlay.dtbo-base := sdm450-pmi632-mtp-s3_$(ZTE_BOARD_NAME).dtb
else
	dtb-y = sdm450-pmi632-mtp-s3_$(ZTE_BOARD_NAME).dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
