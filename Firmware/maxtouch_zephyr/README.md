## Overview

   - For the moment scans I2C addresses in Zephyr

The I2C pins are defined in /boards/espressif/esp32s2_devkitc-pinctrl.dtsi

 :ref:`i2c0_default`.

For the moment I just edited that file manually since I don't know exactly how to extend it in the overlay:

```
 	i2c0_default: i2c0_default {
		group1 {
			pinmux = <I2C0_SDA_GPIO0>,
				 <I2C0_SCL_GPIO1>;
			bias-pull-up;
			drive-open-drain;
			output-high;
		};
	};
```

The source code shows how to:

1. Instance I2C
2. Scan the addresses from 1 to 127
3. Output the first device found and it's address


Requirements
************

Your board must:

* Have I2C enabled, connected to the device, and it must be powered
* Maybe will need pullups in the SDA, SCL lines depending on the device you use

Adding board support
********************

The idea is to port this to a nRF52 but I'm still a newbie on Zephyr!
There is a lot to learn still.