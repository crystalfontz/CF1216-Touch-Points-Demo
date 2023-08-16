//==============================================================================
//
//  CRYSTALFONTZ
//
//  The touch controller is a HeFei Sitronix CF1216
//    https://www.crystalfontz.com/controllers/HeFeiSitronix/CF1216/
//
//  Seeeduino, an open-source 3.3v capable Arduino clone.
//    https://www.crystalfontz.com/product/cfapn15062-seeeduino-arduino-clone-microprocessor
//    https://github.com/SeeedDocument/SeeeduinoV4/raw/master/resources/Seeeduino_v4.2_sch.pdf
//
//==============================================================================
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
//
// Connection guide:
// Capacitive touch tail and control lines on Seeeduino:
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

// Include interfaces
#include <Arduino.h>
#include <Wire.h>

// Define x and y resolution of display and Arduino pins
#define RESPIN 14
#define INTPIN 15
#define X_RES 320
#define Y_RES 240

// The touch controller address can be retrieved using the i2c scanner example in Arduino
// or from the touch controller datasheet
int address = 0x55;

// Define a number of touch points you would like to try reading (you may or may not have this many
// touch points)
int touchpoints = 10;

void setup(void)
{

  // Debug port / Arduino Serial Monitor (optional)
  Serial.begin(115200);
  Serial.println("setup started");

  // Setup pin directions
  pinMode(RESPIN, OUTPUT);
  pinMode(INTPIN, INPUT);

  // Perform a hardware reset
  digitalWrite(RESPIN, LOW);
  delay(500);
  digitalWrite(RESPIN, HIGH);
  delay(1000);

  // Begin wire transmission on i2c address
  Wire.begin();
  Wire.beginTransmission(address);

  // Select the register to write to (in this case we are selecting the firmware version
  // register)
  Wire.write(0x00);

  // In order to read from a register, we must select the register with a write command,
  // issue the endTransmission command to transfer the previous write command to the i2c
  // address, request from the i2c address the number of bytes required and then issue a
  // read command

  if (Wire.endTransmission(address))
  {
    Serial.println("failed to read");
    while (1)
      ;
  }

  else
  {
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    uint8_t fwv = Wire.read();
    Serial.print("FW = ");
    Serial.println(fwv, HEX);
  }

  // In order to set the XY resolution of the display, we must write to register 0x04 - 0x06 the upper and lower
  // bytes of the XY co-ordinates
  Wire.beginTransmission(address);                           // Begin wire transmission on i2c address
  Wire.write(0x04);                                          // Select the register to start writing to/reading from
  Wire.write((X_RES & 0xF00) >> 4) | ((Y_RES & 0xF00) >> 8); // Set the upper bytes of the x and y co-ordinates (0x04)
  Wire.write(X_RES & 0xFF);                                  // Set the lower byte of the x co-ordinate (0x05)
  Wire.write(Y_RES & 0xFF);                                  // Set the lower byte of the y co-ordinate (0x06)
  Wire.endTransmission();                                    // Transfer the write commands to the i2c address
  
  Serial.println("setup complete");
}

void loop()
{
  uint16_t x;
  uint16_t y;
  uint8_t XYH_POS;
  uint8_t XL_POS;
  uint8_t YL_POS;

  // When the display is touched, the INTPIN will read a value. In this case, the interrupt pulls the line low when
  // the screen is touched
  while (analogRead(INTPIN) < 10)
  {
    Wire.beginTransmission(address); // Begin wire transmission on i2c address

    // In order to find the number of touch points and the touch location on the display,
    // we will be using the XY co-ordinate register at address 0x12
    Wire.write(0x12);
    Wire.endTransmission(); // Transfer the write commands to the i2c address

    // The XY co-ordinate registers range from 0x12 - 0x15. As we are starting at 0x12, we
    // need to request 4 bytes of data to cycle through all the information in the registers.
    // We multiply the number of bytes by the number of touchpoints we would like to try
    // reading from
    Wire.requestFrom(address, 4 * touchpoints);

    // Loop through the readings for each touchpoint
    for (int i = 0; i < touchpoints; i++)
    {
      XYH_POS = Wire.read();      // The 1st byte is from 0x12 which has x and y co-ordinate high byte
      XL_POS = Wire.read();       // The 2nd byte is from 0x13 which has the x co-ordinate low byte
      YL_POS = Wire.read();       // The 3rd byte is from 0x14 which has the y co-ordinate low byte
      uint8_t temp = Wire.read(); // The 4th byte is from 0x15 and is reserved, therefore write it to a
                                  // temporary variable
      // Locations 0 to touchpoints-1
      Serial.print("Location: ");
      Serial.print(i);
      // Combine the bytes from registers 0x12 to 0x14 to get the actual x and y co-ordinate bytes
      x = (((XYH_POS & 0xF0) << 4) | XL_POS & 0xFF);
      y = (((XYH_POS & 0x0F) << 8) | YL_POS & 0xFF);
      Serial.print(", X = ");
      Serial.print(x); // Print the x co-ordinate
      Serial.print(", Y = ");
      Serial.println(y); // Print the y co-ordinate
      x = 0;
      y = 0;
    }
    Serial.println(""); // Print a blank line for legibility
    break; // Exit the while loop after printing all readings
  }
  delay(1500); // This delay tries to ensure the same reading isn't made several times but creates a delay between when
               // the screen is touched and the time it takes to print a reading on the serial monitor
}
