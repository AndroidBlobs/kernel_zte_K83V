/***********************************
***** hall sensor****************
************************************
************************************/
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/pm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#define KEY_HALL_SENSOR_DOWN	249
#define KEY_HALL_SENSOR_UP	 250

static int hall_state = 1;
struct pinctrl *hall_gpio_pinctrl = NULL;
struct pinctrl_state *hall_gpio_state = NULL;

static int set_hall_gpio_state(struct device *dev);

module_param(hall_state, int, 0644);

struct hall_chip {
	struct input_dev *input;
	struct work_struct work;
	int irq;
	bool hall_enabled;
} *hall_chip_data;

static void hall_work_func(struct work_struct *work)
{
	int value;

	if (hall_chip_data == NULL || hall_chip_data->input == NULL) {
		pr_info("hall_work_func ERROR");
		return;
	}

	value = gpio_get_value(hall_chip_data->irq);

	pr_info("%s:hall test value=%d, enabled = %d\n", __func__, value, hall_chip_data->hall_enabled);

	if (hall_chip_data->hall_enabled == 1) {
		if (value == 1) {
			/*log for off*/
			pr_info("%s:hall ===switch is off!!the value = %d\n", __func__, value);
			input_report_key(hall_chip_data->input, KEY_HALL_SENSOR_UP, 1);
			input_sync(hall_chip_data->input);
			input_report_key(hall_chip_data->input, KEY_HALL_SENSOR_UP, 0);
			hall_state = 1;
		} else {
			/*log for on*/
			pr_info("%s:hall ===switch is on!!the value = %d\n", __func__, value);
			input_report_key(hall_chip_data->input, KEY_HALL_SENSOR_DOWN, 1);
			input_sync(hall_chip_data->input);
			input_report_key(hall_chip_data->input, KEY_HALL_SENSOR_DOWN, 0);
			hall_state = 0;
		}
		input_sync(hall_chip_data->input);
	}
}

static irqreturn_t hall_interrupt(int irq, void *dev_id)
{
	if (hall_chip_data == NULL) {
		return IRQ_NONE;
	}

	pr_info("%s: hall_interrupt!!\n", __func__);

	schedule_work(&hall_chip_data->work);

	return IRQ_HANDLED;
}

int hall_parse_dt(struct platform_device *pdev)
{
	hall_chip_data->irq = of_get_named_gpio(pdev->dev.of_node, "gpio_irq", 0);

	if (!gpio_is_valid(hall_chip_data->irq)) {
		pr_info("gpio irq pin %d is invalid.\n", hall_chip_data->irq);
		return -EINVAL;
	}

	return 0;
}

/* set hall gpio input and no pull*/
static int set_hall_gpio_state(struct device *dev)
{
	int error = 0;

	hall_gpio_pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR_OR_NULL(hall_gpio_pinctrl)) {
		pr_info("Can not get hall_gpio_pinctrl\n");
		error = PTR_ERR(hall_gpio_pinctrl);
		return error;
	}

	hall_gpio_state = pinctrl_lookup_state(hall_gpio_pinctrl, "zte_hall_gpio_active");
	if (IS_ERR_OR_NULL(hall_gpio_state)) {
		pr_info("Can not get hall_gpio_state\n");
		error = PTR_ERR(hall_gpio_state);
		return error;
	}

	error = pinctrl_select_state(hall_gpio_pinctrl, hall_gpio_state);
	if (error) {
		pr_info("can not set hall_gpio pins to zte_hall_gpio_active states\n");
	} else {
		pr_info("set_hall_gpio_state success.\n");
	}
	return error;
}

static int hall_probe(struct platform_device *pdev)
{
	int value_status;
	struct device *dev = &pdev->dev;
	int error = 0;
	int irq = 0;

	if (pdev->dev.of_node == NULL) {
		dev_info(&pdev->dev, "can not find device tree node\n");
		return -ENODEV;
	}

	hall_chip_data = kzalloc(sizeof(struct hall_chip), GFP_KERNEL);
	hall_chip_data->input = input_allocate_device();
	if (!hall_chip_data || !hall_chip_data->input) {
		error = -ENOMEM;
		goto fail0;
	}

	hall_chip_data->input->name = "hall_ic";

	set_bit(EV_KEY, hall_chip_data->input->evbit);
	set_bit(KEY_HALL_SENSOR_DOWN, hall_chip_data->input->keybit);
	set_bit(KEY_HALL_SENSOR_UP, hall_chip_data->input->keybit);

	error = input_register_device(hall_chip_data->input);
	if (error) {
		pr_err("hall: Unable to register input device, error: %d\n", error);
		goto fail0;
	}

	error = hall_parse_dt(pdev);
	if (error) {
		goto fail1;
	}

	if (hall_chip_data->irq) {
		irq = gpio_to_irq(hall_chip_data->irq);

		error = gpio_request(hall_chip_data->irq, "hall_irq");
		if (error) {
			pr_info("%s:hall error3\n", __func__);
			goto fail2;
		}

		error = gpio_direction_input(hall_chip_data->irq);
		if (error) {
			pr_info("%s:hall error3\n", __func__);
			goto fail2;
		}
	}

	error = set_hall_gpio_state(dev);
	if (error < 0) {
		pr_info("set_hall_gpio_state failed.\n");
	}

	if (irq) {
		INIT_WORK(&(hall_chip_data->work), hall_work_func);
		error = request_threaded_irq(irq, NULL, hall_interrupt,
							   IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_ONESHOT,
						      "hall_irq", NULL);
		if (error) {
			pr_err("gpio-hall: Unable to claim irq %d; error %d\n", irq, error);
			goto fail2;
		}

		enable_irq_wake(irq);
	}

	hall_chip_data->hall_enabled = 1;

	value_status = gpio_get_value(hall_chip_data->irq);
	pr_err("gpio-hall: irq %d;\n", value_status);
	if (value_status == 1) {
		hall_state = 1;
	} else {
		hall_state = 0;
	}

	pr_info("hall Init success! hall_state=%d\n", hall_state);
	return 0;

fail2:
	gpio_free(hall_chip_data->irq);
fail1:
	input_unregister_device(hall_chip_data->input);
	kfree(hall_chip_data);
fail0:
	platform_set_drvdata(pdev, NULL);
	return error;
}

static int hall_remove(struct platform_device *pdev)
{
	gpio_free(hall_chip_data->irq);
	input_unregister_device(hall_chip_data->input);
	kfree(hall_chip_data);

	return 0;
}


static struct of_device_id hall_of_match[] = {
	{.compatible = "hall_ic", },
	{},
};

static struct platform_driver hall_driver = {
	.probe = hall_probe,
	.remove = hall_remove,
	.driver = {
		.name = "hall_ic",
		.owner = THIS_MODULE,
		.of_match_table = hall_of_match,
	},
};

static int hall_init(void)
{
	return platform_driver_register(&hall_driver);
}

static void hall_exit(void)
{
	platform_driver_unregister(&hall_driver);
}

module_init(hall_init);
module_exit(hall_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("hall sensor driver");
MODULE_ALIAS("platform:hall-sensor");
