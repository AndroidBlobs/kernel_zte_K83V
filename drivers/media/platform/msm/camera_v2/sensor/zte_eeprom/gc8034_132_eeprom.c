

#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/crc32.h>
#include "msm_sd.h"
#include "msm_cci.h"
#include "zte_eeprom.h"

#define GC8034_132_EEPROM_DEBUG
#undef CDBG
#ifdef GC8034_132_EEPROM_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif

#define GC8034_132_SENSOR_INFO_MODULE_ID_SUNNY		0x01
#define GC8034_132_SENSOR_INFO_MODULE_ID_TRULY		0x02
#define GC8034_132_SENSOR_INFO_MODULE_ID_A_KERR		0x03
#define GC8034_132_SENSOR_INFO_MODULE_ID_LITEARRAY	0x04
#define GC8034_132_SENSOR_INFO_MODULE_ID_DARLING		0x05
#define GC8034_132_SENSOR_INFO_MODULE_ID_QTECH		0x06
#define GC8034_132_SENSOR_INFO_MODULE_ID_OFLIM		0x07
#define GC8034_132_SENSOR_INFO_MODULE_ID_FOXCONN		0x11
#define GC8034_132_SENSOR_INFO_MODULE_ID_IMPORTEK	0x12
#define GC8034_132_SENSOR_INFO_MODULE_ID_ALTEK		0x13
#define GC8034_132_SENSOR_INFO_MODULE_ID_ABICO		0x14
#define GC8034_132_SENSOR_INFO_MODULE_ID_LITE_ON		0x15
#define GC8034_132_SENSOR_INFO_MODULE_ID_CHICONY		0x16
#define GC8034_132_SENSOR_INFO_MODULE_ID_PRIMAX		0x17
#define GC8034_132_SENSOR_INFO_MODULE_ID_SHARP		0x21
#define GC8034_132_SENSOR_INFO_MODULE_ID_MCNEX		0x31
#define GC8034_132_SENSOR_INFO_MODULE_ID_HOLITECH	0x42
#define GC8034_132_SENSOR_INFO_MODULE_ID_GOERTEK		0x54
#define GC8034_132_SENSOR_INFO_MODULE_ID_SHINETECH	0x55
#define GC8034_132_SENSOR_INFO_MODULE_ID_SUNWIN		0x56
#define GC8034_132_SENSOR_INFO_MODULE_ID_JSL			0x57
#define GC8034_132_SENSOR_INFO_MODULE_ID_UNION		0x58
#define GC8034_132_SENSOR_INFO_MODULE_ID_SEASIONS	0x59

MODULE_Map_Table GC8034_132_MODULE_MAP[] = {
	{ GC8034_132_SENSOR_INFO_MODULE_ID_SUNNY,
		"sunny_gc8034", "sunny_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_TRULY,
		"truly_gc8034", "truly_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_A_KERR,
		"a_kerr_gc8034", "a_kerr_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_LITEARRAY,
		"litearray_gc8034", "litearray_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_DARLING,
		"darling_gc8034", "darling_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_QTECH,
		"qtech_gc8034", "qtech_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_OFLIM,
		"oflim_gc8034", "oflim_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_FOXCONN,
		"foxconn_gc8034", "foxconn_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_IMPORTEK,
		"importek_gc8034", "importek_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_ALTEK,
		"altek_gc8034", "altek_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_ABICO,
		"abico_gc8034", "abico_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_LITE_ON,
		"lite_on_gc8034", "lite_on_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_CHICONY,
		"chicony_gc8034", "chicony_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_PRIMAX,
		"primax_gc8034", "primax_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_SHARP,
		"sharp_gc8034", "sharp_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_MCNEX,
		"mcnex_gc8034", "mcnex_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_HOLITECH,
		"holitech_gc8034", "holitech_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_GOERTEK,
		"goertek_gc8034", "goertek_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_SHINETECH,
		"shinetech_gc8034", "shinetech_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_SUNWIN,
		"sunwin_gc8034", "sunwin_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_JSL,
		"jsl_gc8034", "jsl_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_UNION,
		"union_gc8034", "union_gc8034", NULL},
	{ GC8034_132_SENSOR_INFO_MODULE_ID_SEASIONS,
		"seasons_gc8034", "seasons_gc8034", NULL},
};

#define GC8034_132_ID_ADDR 1264


void gc8034_132_parse_module_name(struct msm_eeprom_ctrl_t *e_ctrl)
{
	uint16_t sensor_module_id = e_ctrl->cal_data.mapdata[GC8034_132_ID_ADDR];

	parse_module_name(&(e_ctrl->module_info[0]), GC8034_132_MODULE_MAP,
		sizeof(GC8034_132_MODULE_MAP) / sizeof(MODULE_Map_Table),
		sensor_module_id);
}

static int gc8034_132_checksum_eeprom(struct msm_eeprom_ctrl_t *e_ctrl)
{
	int  rc = 0;
	return rc;
}

static int gc8034_132_get_read_mem_size
	(struct msm_eeprom_memory_map_array *eeprom_map_array) {
	int size = 0, i, j;
	struct msm_eeprom_mem_map_t *eeprom_map;

	if (eeprom_map_array->msm_size_of_max_mappings >
		MSM_EEPROM_MAX_MEM_MAP_CNT) {
		pr_err("%s:%d Memory map cnt greter then expected: %d",
			__func__, __LINE__,
			eeprom_map_array->msm_size_of_max_mappings);
		return -EINVAL;
	}
	for (j = 0; j < eeprom_map_array->msm_size_of_max_mappings; j++) {
		eeprom_map = &(eeprom_map_array->memory_map[j]);
		if (eeprom_map->memory_map_size >
			MSM_EEPROM_MEMORY_MAP_MAX_SIZE) {
			pr_err("%s:%d Memory map size greter then expected: %d",
				__func__, __LINE__,
				eeprom_map->memory_map_size);
			return -EINVAL;
		}
		for (i = 0; i < eeprom_map->memory_map_size; i++) {
			if ((eeprom_map->mem_settings[i].i2c_operation ==
				MSM_CAM_READ) ||
				(eeprom_map->mem_settings[i].i2c_operation ==
				MSM_CAM_POLL)) {
				size += eeprom_map->mem_settings[i].reg_data;
			}
		}
	}
	CDBG("%s Total Data Size: %d\n", __func__, size);
	return size;
}

int gc8034_132_user_read_eeprom_memory(struct msm_eeprom_ctrl_t *e_ctrl,
	struct msm_eeprom_memory_map_array *eeprom_map_array)
{
		int rc =  0, i, j, gc;
		uint8_t *memptr;
		uint16_t gc_read = 0;
		struct msm_eeprom_mem_map_t *eeprom_map;

		e_ctrl->cal_data.mapdata = NULL;

		e_ctrl->cal_data.num_data = gc8034_132_get_read_mem_size(eeprom_map_array);
		if (e_ctrl->cal_data.num_data <= 0) {
			pr_err("%s:%d Error in reading mem size\n",
				__func__, __LINE__);
			e_ctrl->cal_data.num_data = 0;
			return -EINVAL;
		}
		e_ctrl->cal_data.mapdata =
			kzalloc(e_ctrl->cal_data.num_data, GFP_KERNEL);
		if (!e_ctrl->cal_data.mapdata)
			return -ENOMEM;

		memptr = e_ctrl->cal_data.mapdata;
		for (j = 0; j < eeprom_map_array->msm_size_of_max_mappings; j++) {
			eeprom_map = &(eeprom_map_array->memory_map[j]);
			if (e_ctrl->i2c_client.cci_client) {
				e_ctrl->i2c_client.cci_client->sid =
					eeprom_map->slave_addr >> 1;
			} else if (e_ctrl->i2c_client.client) {
				e_ctrl->i2c_client.client->addr =
					eeprom_map->slave_addr >> 1;
			}
			CDBG("Slave Addr: 0x%X\n", eeprom_map->slave_addr);
			CDBG("Memory map Size: %d",
				eeprom_map->memory_map_size);
			for (i = 0; i < eeprom_map->memory_map_size; i++) {
				switch (eeprom_map->mem_settings[i].i2c_operation) {
				case MSM_CAM_WRITE: {
					e_ctrl->i2c_client.addr_type =
						eeprom_map->mem_settings[i].addr_type;
					rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_write(
						&(e_ctrl->i2c_client),
						eeprom_map->mem_settings[i].reg_addr,
						eeprom_map->mem_settings[i].reg_data,
						eeprom_map->mem_settings[i].data_type);
					msleep(eeprom_map->mem_settings[i].delay);
					if (rc < 0) {
						pr_err("%s: page write failed\n",
							__func__);
						goto clean_up;
					}
				}
				break;
				case MSM_CAM_POLL: {
					e_ctrl->i2c_client.addr_type =
						eeprom_map->mem_settings[i].addr_type;
					rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_poll(
						&(e_ctrl->i2c_client),
						eeprom_map->mem_settings[i].reg_addr,
						eeprom_map->mem_settings[i].reg_data,
						eeprom_map->mem_settings[i].data_type,
						eeprom_map->mem_settings[i].delay);
					if (rc < 0) {
						pr_err("%s: poll failed\n",
							__func__);
						goto clean_up;
					}
				}
				break;
			/*	case MSM_CAM_READ: {
					e_ctrl->i2c_client.addr_type =
						eeprom_map->mem_settings[i].addr_type;
					rc = e_ctrl->i2c_client.i2c_func_tbl->
						i2c_read_seq(&(e_ctrl->i2c_client),
						eeprom_map->mem_settings[i].reg_addr,
						memptr,
						eeprom_map->mem_settings[i].reg_data);
					msleep(eeprom_map->mem_settings[i].delay);
					if (rc < 0) {
						pr_err("%s: read failed\n",
							__func__);
						goto clean_up;
					}
					memptr += eeprom_map->mem_settings[i].reg_data;
				}
				break;
				*/
				case MSM_CAM_READ: {
					e_ctrl->i2c_client.addr_type =
					eeprom_map->mem_settings[i].addr_type;
					for (gc = 0; gc < eeprom_map->mem_settings[i].reg_data; gc++) {
						msleep(eeprom_map->mem_settings[i].delay);
						rc = e_ctrl->i2c_client.i2c_func_tbl->i2c_read(
							&(e_ctrl->i2c_client),
							eeprom_map->mem_settings[i].reg_addr,
							&gc_read,
							eeprom_map->mem_settings[i].data_type);
						if (rc < 0) {
							pr_err("%s: read failed\n",
								__func__);
							goto clean_up;
						}
						*memptr = (uint8_t)gc_read;
						memptr++;
					}
				}
				break;

				default:
					pr_err("%s: %d Invalid i2c operation LC:%d, op: %d\n",
						__func__, __LINE__, i, eeprom_map->mem_settings[i].i2c_operation);
					return -EINVAL;
				}
			}
		}
		memptr = e_ctrl->cal_data.mapdata;
		for (i = 0; i < e_ctrl->cal_data.num_data; i++)
			pr_err("memory_data[%d] = 0x%X\n", i, memptr[i]);
		return rc;
clean_up:
		kfree(e_ctrl->cal_data.mapdata);
		e_ctrl->cal_data.num_data = 0;
		e_ctrl->cal_data.mapdata = NULL;
		return rc;
	}

static struct zte_eeprom_fn_t gc8034_132_eeprom_func_tbl = {
	.eeprom_parse_map = NULL,
	.kernel_read_eeprom_memory = NULL,
	.user_read_eeprom_memory = gc8034_132_user_read_eeprom_memory,
	.eeprom_match_crc = NULL,
	.eeprom_checksum = gc8034_132_checksum_eeprom,
	.validflag_check_eeprom = NULL,
	.parse_module_name = gc8034_132_parse_module_name,
};

static const struct of_device_id gc8034_132_eeprom_dt_match[] = {
	{ .compatible = "zte,gc8034_132-eeprom", .data = &gc8034_132_eeprom_func_tbl},
};
MODULE_DEVICE_TABLE(of, common_eeprom_dt_match);

static int gc8034_132_eeprom_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;

	CDBG("%s:%d E", __func__, __LINE__);
	match = of_match_device(gc8034_132_eeprom_dt_match, &pdev->dev);
	if (match)
		rc = zte_eeprom_platform_probe_user(pdev, match);
	else {
		pr_err("%s:%d match is null\n", __func__, __LINE__);
		rc = -EINVAL;
	}
	CDBG("%s:%d X", __func__, __LINE__);
	return rc;
}

static int gc8034_132_eeprom_platform_remove(struct platform_device *pdev)
{
	const struct of_device_id *match;
	int32_t rc = 0;

	CDBG("%s:%d E", __func__, __LINE__);
	match = of_match_device(gc8034_132_eeprom_dt_match, &pdev->dev);
	if (match)
		rc = zte_eeprom_platform_remove(pdev);
	else {
		pr_err("%s:%d match is null\n", __func__, __LINE__);
		rc = -EINVAL;
	}
	CDBG("%s:%d X", __func__, __LINE__);
	return rc;
}

static struct platform_driver gc8034_132_eeprom_platform_driver = {
	.driver = {
		.name = "zte,gc8034_132-eeprom",
		.owner = THIS_MODULE,
		.of_match_table = gc8034_132_eeprom_dt_match,
	},
	.probe = gc8034_132_eeprom_platform_probe,
	.remove = gc8034_132_eeprom_platform_remove,
};

static int __init gc8034_132_eeprom_init_module(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);
	rc = platform_driver_register(&gc8034_132_eeprom_platform_driver);
	CDBG("%s:%d platform rc %d\n", __func__, __LINE__, rc);
	return rc;
}

static void __exit gc8034_132_eeprom_exit_module(void)
{
	platform_driver_unregister(&gc8034_132_eeprom_platform_driver);

}

module_init(gc8034_132_eeprom_init_module);
module_exit(gc8034_132_eeprom_exit_module);
MODULE_DESCRIPTION("ZTE EEPROM driver");
MODULE_LICENSE("GPL v2");
