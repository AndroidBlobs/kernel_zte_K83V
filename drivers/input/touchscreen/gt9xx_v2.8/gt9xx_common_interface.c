/************************************************************************
*
* File Name: gt9xx_common_interface.c
*
*  *   Version: v1.0
*
************************************************************************/
#include "gt9xx.h"

#define MAX_NAME_LEN_20  20

char gt9xx_vendor_name[MAX_NAME_LEN_20] = { 0 };

struct tpvendor_t gt9xx_vendor_l[] = {
	{0x00, GTP_SENSOR_ID0_VENDOR_NAME},
	{0x01, GTP_SENSOR_ID1_VENDOR_NAME},
	{0x02, GTP_SENSOR_ID2_VENDOR_NAME},
	{0x03, GTP_SENSOR_ID3_VENDOR_NAME},
	{0x04, GTP_SENSOR_ID4_VENDOR_NAME},
	{0x05, GTP_SENSOR_ID5_VENDOR_NAME},
	{0xFF, "Unknown"},
};

static int gt9xx_get_chip_vendor(int vendor_id)
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(gt9xx_vendor_l); i++) {
		if ((gt9xx_vendor_l[i].vendor_id == vendor_id) || (gt9xx_vendor_l[i].vendor_id == 0xFF)) {
			pr_info("GTP:vendor_id is 0x%x.\n", vendor_id);
			strlcpy(gt9xx_vendor_name, gt9xx_vendor_l[i].vendor_name, sizeof(gt9xx_vendor_name));
			pr_info("GTP:gt9xx_vendor_name: %s.\n", gt9xx_vendor_name);
			break;
		}
	}
	return 0;
}

static int tpd_init_tpinfo(struct tpd_classdev_t *cdev)
{
	int ret = 0;
	u8 config_ver = 0;
	struct goodix_ts_data *ts = i2c_get_clientdata(i2c_connect_client);

	ret = gtp_get_fw_info(i2c_connect_client, &ts->fw_info);
	if (ret < 0) {
		pr_err("GTP:Failed read FW version\n");
	}
	ret = gtp_i2c_read_dbl_check(i2c_connect_client, GTP_REG_CONFIG_DATA, &config_ver, 1);
	if (ret != SUCCESS) {
		pr_err("GTP:Failed read config version\n");
	}
	strlcpy(cdev->ic_tpinfo.tp_name, "Goodix", sizeof(cdev->ic_tpinfo.tp_name));
	gt9xx_get_chip_vendor(ts->fw_info.sensor_id);
	strlcpy(cdev->ic_tpinfo.vendor_name, gt9xx_vendor_name, sizeof(cdev->ic_tpinfo.vendor_name));
	cdev->ic_tpinfo.chip_model_id = TS_CHIP_GOODIX;
	cdev->ic_tpinfo.module_id = ts->fw_info.sensor_id;
	cdev->ic_tpinfo.firmware_ver = ts->fw_info.version;
	cdev->ic_tpinfo.config_ver = config_ver;
	cdev->ic_tpinfo.i2c_type = 0;
	cdev->ic_tpinfo.i2c_addr = i2c_connect_client->addr;
	return 0;
}
static int gtp_i2c_reg_read(struct tpd_classdev_t *cdev, u32 addr, u8 *data, int len)
{
	pr_info("GTP:gtp_i2c_reg_read addr is 0x%x.", addr);
	return gtp_i2c_read_dbl_check(i2c_connect_client, addr, data, len);
}

static int gtp_i2c_reg_write(struct tpd_classdev_t *cdev, u32 addr, u8 *data, int len)
{
	int i = 0;
	char *tmpbuf = NULL;

	tmpbuf = kzalloc(len + 2, GFP_KERNEL);
	if (!tmpbuf) {
		pr_info("allocate memory failed!\n");
		return -ENOMEM;
	}
	tmpbuf[0] = (u8)(addr >> 8);
	tmpbuf[1] = (u8)(addr & 0xFF);
	for (i = 2; i < len + 2; i++) {
		tmpbuf[i] = data[i - 2];
	}
	gtp_i2c_write(i2c_connect_client, tmpbuf, len + 2);
	kfree(tmpbuf);
	return 0;
}

void gtp_tpd_register_fw_class(void)
{
	pr_info("GTP: tpd_register_fw_class: %s", GTP_DRIVER_VERSION);
	tpd_fw_cdev.get_tpinfo = tpd_init_tpinfo;
	tpd_fw_cdev.tp_i2c_16bor32b_reg_read = gtp_i2c_reg_read;
	tpd_fw_cdev.tp_i2c_16bor32b_reg_write = gtp_i2c_reg_write;
	tpd_fw_cdev.reg_char_num = 4;
}
