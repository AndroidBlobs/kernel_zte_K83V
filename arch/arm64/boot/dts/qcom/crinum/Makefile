ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
	dtbo-y = zte-msm8917-pmi8940-$(ZTE_BOARD_NAME)-overlay.dtbo
	zte-msm8917-pmi8940-$(ZTE_BOARD_NAME)-overlay.dtbo-base := zte-msm8917-pmi8940-$(ZTE_BOARD_NAME).dtb
else
	dtb-y = zte-msm8917-pmi8940-$(ZTE_BOARD_NAME).dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
