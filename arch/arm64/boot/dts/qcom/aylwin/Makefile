ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
    ifeq ($(ENABLE_AB), true)
        dtbo-y = zte-msm8917-pmi8950-$(ZTE_BOARD_NAME)-ab-overlay.dtbo
        zte-msm8917-pmi8950-$(ZTE_BOARD_NAME)-ab-overlay.dtbo-base := zte-msm8917-pmi8950-$(ZTE_BOARD_NAME).dtb
        $(warning AB system has been enabled)
    else
        dtbo-y = zte-msm8917-pmi8950-$(ZTE_BOARD_NAME)-overlay.dtbo
        zte-msm8917-pmi8950-$(ZTE_BOARD_NAME)-overlay.dtbo-base := zte-msm8917-pmi8950-$(ZTE_BOARD_NAME).dtb
        $(warning AB system has not been enabled)
    endif
else
    dtb-y = zte-msm8917-pmi8950-$(ZTE_BOARD_NAME).dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
