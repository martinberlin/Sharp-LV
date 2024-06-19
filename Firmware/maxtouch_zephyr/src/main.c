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

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int main(void)
{
	printk("Hello testing reply from %x\n", TOUCH_I2C_ADDR);

	uint8_t buf[2];
	int i, ret;

	if (!device_is_ready(i2c_dev)) {
		printk("I2C: Device is not ready.\n");
		return -1;
	}
	// I2C scan
	bool found = false;
	for (uint8_t i = 1; i < 127; i++)
     {
		ret = i2c_read(i2c_dev, buf, 2, i);
		if (0 == ret) {
			found = true;
			printk("i2c ADDR 0x%x read success: 0x%02X%02X\n", i, buf[0], buf[1]);
		} 
		k_msleep( 200 );
	}
	if (!found) {
		printk("No I2C devices found\n");
	}

	mxt_init(i2c_dev);

	return 0;
}
