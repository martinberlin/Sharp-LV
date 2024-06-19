## MaxTouch MXT144

Thanks to Larry Bank [@bitbank2](https://github.com/bitbank2) to be the firt one making the touch work with his [bb_captouch](https://github.com/bitbank2/bb_captouch/) library.
Just load in Arduino IDE the [MaxTouchSharp](https://github.com/martinberlin/Sharp-LV/tree/main/Firmware/MaxTouchSharp) folder. Select any board and connect I2C plus RST pin to the microcontroller. Does not need in my opinion pullup resistances for a quick test although there will be added in the PCB.

### Additional challenge with this MaxTouch device

We should check the [mXT144U Datasheet](https://www.microchip.com/content/dam/mchp/documents/OTH/ProductDocuments/DataSheets/mXT144U%20Touchscreen%20Controller%20Product%20Datasheet.pdf) and see if there is an "interrupt mode" where IRQ pin stays HIGH and goes low when there are messages pending to be read.

### 2 flavours of PCB being designed

1. **Nordic nRF52840** BLE only/ Low consumption (And low experience from my side) you can preview it in this repository, all files open source and MIT licensed. Reviewed by the super helpful and friendly [Nordic engineers in the devZone](https://devzone.nordicsemi.com/f/nordic-q-a/112090/schematics-sanity-check-for-a-sharp-lcd-controller-using-nrf52840)



2. **ESP32-S3FN8** silicon only board w/ BLE & Wi-Fi, made one before so the experience is a bit higher, this one is not **low consumption**. But at least is 240 Mhz and we could try boosting Sharp with LVGL to the max. possible speed.
[Preview both here](https://github.com/martinberlin/Sharp-LV/wiki/PCBs-being-made)

![S3 PCB version](https://private-user-images.githubusercontent.com/2692928/341010109-5f33ea71-9d8a-4fb1-8af0-fe7ed78752a8.png)