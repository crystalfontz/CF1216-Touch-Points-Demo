# HeFei Sitronix CF1216 Touch Controller Demonstration Code

This is Arduino sample code for any display that uses the HeFei Sitronix CF1216 touch controller. The datasheet for this controller is available [here](https://www.crystalfontz.com/controllers/HeFeiSitronix/CF1216/). The code records the number of touch points detected when the capacitive touch screen is touched. 

Please refer the display datasheet for the touch tail before connection to a power source. 

## Connection Guide
```
// ARD       | Port  | Touch tail pin   | Function                                |
//-----------+-------+------------------+-----------------------------------------+
// GND       |       | 01               | GROUND                                  |
// 3.3V      |       | 02               | POWER 3.3V                              |
//-----------+-------+------------------+-----------------------------------------+
// A0        | PORTC | 03               | Reset                           (RST)   |
// A1        | PORTC | 04               | Interrupt                       (INT)   |
// A4        | PORTC | 05               | I2C data                        (SDA)   |
// A5        | PORTB | 06               | I2C clock                       (SCL)   |
//-----------+-------+------------------+-----------------------------------------+
```

## Display Information
Here are links to our active displays that use the HeFei Sitronix CF1216 touch controller:\
[CFAF320240H0-035SC](https://www.crystalfontz.com/product/cfaf320240h0035sc-320x240-capacitive-touchscreen-tft-display) 

For more inforamtion about other touchscreen display offerings, please see our full list [here](https://www.crystalfontz.com/c/touch-screen-displays/527).