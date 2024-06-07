/*
 * Copyright (c) 2024 Fasani Corp
 *
 * SPDX-License-Identifier: No Fuckin Licence
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <errno.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <input_maxtouch.c>

#define TOUCH_I2C_ADDR 0x4a


int main(void)
{
	printk("Hello testing reply from %x\n", TOUCH_I2C_ADDR);
	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	uint8_t buf[2];
	int i, ret;

	if (!device_is_ready(i2c_dev)) {
		printk("I2C: Device is not ready.\n");
		return;
	}

	while (1) {
		ret = i2c_read(i2c_dev, buf, 2, TOUCH_I2C_ADDR);
		if (0 == ret) {
			printk("i2c read success: 0x%02X%02X\n", buf[0], buf[1]);
		} else {
			printk("i2c read failure %d\n", ret);
		}
		k_msleep( 2000 );
	}

	return 0;
}
