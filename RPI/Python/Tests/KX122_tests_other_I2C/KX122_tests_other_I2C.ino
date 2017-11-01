/*------------------------------------------------------------------------------
 ROHM Multi-Sensor Shield Board - Sensor Output Application
 
    Copyright (C) 2015 ROHM USDC Applications Engineering Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 This program reads the value of the connected ROHM Sensors on the Multi-sensor Shield and 
 returns the sensor output through the serial port
 
 Confirmed on Arduino UNO Board
 Rework Required:
   1. The Arduino UNO's I2C input pins are routed to A4 and A5 on the Arduino UNO board.
       Thus, to make sure this does not conflict on our board, we need to remove jumper pins connected to A4/A5
       AKA. REMOVE R31 and R32
   2. Similarly above, since we removed the connection for A4, we need to connect ADC1 (or UV Sensor output) to the Arduino.
       Thus, this code expects for A4 to be re-routed to A0.
       We do this by removing R27 and by routing the top resistor pad of R31 to the bottom resistor pad of R27 (
 
First Revision Posted to Git on 15 June 2015
 by ROHM USDC Applications Engineering Team

------------------------------------------------------------------------------*/


// ----- Included Files -----
//#include <Wire.h>         //Default I2C Library

//#define SCL_PIN 5  //A5       //Note that if you are using the Accel/Mag Sensor, you will need to download and
//#define SCL_PORT PORTC    //install the "SoftI2CMaster" as "Wire" does not support repeated start...
//#define SDA_PIN 4  //A4         //References:
//#define SDA_PORT PORTC    //  http://playground.arduino.cc/Main/SoftwareI2CLibrary

//#include <SoftI2CMaster.h>  //  https://github.com/felias-fogg/SoftI2CMaster
//#define I2C_TIMEOUT 1000  // Sets Clock Stretching up to 1sec
//#define I2C_FASTMODE 1    // Sets 400kHz operating speed

#include "I2C.h"

// ----- Globals -----

int KX122_DeviceAddress = 0x1E; // 0x3C; // this is the 8bit address, 7bit address = 0x1E
int KX122_Accel_X_LB = 0;
int KX122_Accel_X_HB = 0;
int KX122_Accel_Y_LB = 0;
int KX122_Accel_Y_HB = 0;
int KX122_Accel_Z_LB = 0;
int KX122_Accel_Z_HB = 0;
int KX122_Accel_X_RawOUT = 0;
int KX122_Accel_Y_RawOUT = 0;
int KX122_Accel_Z_RawOUT = 0;
float KX122_Accel_X_OUT = 0;
float KX122_Accel_Y_OUT = 0;
float KX122_Accel_Z_OUT = 0;


void setup()
{
  //Wire.begin();        // start I2C functionality
  Serial.begin(9600);  // start serial port at 9600 bps
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  //----- Start Initialization for KX122 Accel Sensor -----  

  //1. CNTL1 (0x18) loaded with 0x41
  //2. ODCNTL (0x1B) loaded with 0x02
  //3. CNTL3 (0x1A) loaded with 0xD8
  //4. TILT_TIMER (0x22) loaded with 0x01
  //5. CNTL1 (0x18) loaded with 0xC1 (Enable bit on)
  
  I2c.begin();
  
  I2c.write(KX122_DeviceAddress,0x18);
  I2c.write(KX122_DeviceAddress, 0x41);
  
  I2c.write(KX122_DeviceAddress,0x1B);
  I2c.write(KX122_DeviceAddress,0x02);
  
  I2c.write(KX122_DeviceAddress,0x1A);
  I2c.write(KX122_DeviceAddress,0xD8);
  
  I2c.write(KX122_DeviceAddress,0x22);
  I2c.write(KX122_DeviceAddress,0x01);
  
  I2c.write(KX122_DeviceAddress,0x18);
  I2c.write(KX122_DeviceAddress,0xC1);
  I2c.end();
  
  
}

void loop()
{
  /*
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);              // wait for 250ms
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(250);              // wait for 250ms
  */
  
  //----- START Code for Reading KX122 Accel Sensor -----
  
  I2c.begin();
  I2c.write(KX122_DeviceAddress,0x06);
  I2c.read(KX122_DeviceAddress,6);
  if (!I2c.available()){
    Serial.println("nothing to read");
  }
  KX122_Accel_X_LB = I2c.receive(); //(KX122_DeviceAddress,1);
  KX122_Accel_X_HB = I2c.receive(); //I2c.read(KX122_DeviceAddress,1);
  KX122_Accel_Y_LB = I2c.receive(); //I2c.read(KX122_DeviceAddress,1);
  KX122_Accel_Y_HB = I2c.receive(); //I2c.read(KX122_DeviceAddress,1);
  KX122_Accel_Z_LB = I2c.receive(); //I2c.read(KX122_DeviceAddress,1);
  KX122_Accel_Z_HB = I2c.receive(); //I2c.read(KX122_DeviceAddress,1);
  I2c.end();

  KX122_Accel_X_RawOUT = (KX122_Accel_X_HB<<8) | (KX122_Accel_X_LB);
  KX122_Accel_Y_RawOUT = (KX122_Accel_Y_HB<<8) | (KX122_Accel_Y_LB);
  KX122_Accel_Z_RawOUT = (KX122_Accel_Z_HB<<8) | (KX122_Accel_Z_LB);

  KX122_Accel_X_OUT = (float)KX122_Accel_X_RawOUT / 16384;
  KX122_Accel_Y_OUT = (float)KX122_Accel_Y_RawOUT / 16384;
  KX122_Accel_Z_OUT = (float)KX122_Accel_Z_RawOUT / 16384;
  
  Serial.print("KX122 (X) = ");
  Serial.print(KX122_Accel_X_OUT);
  Serial.println(" g");
  Serial.print("KX122 (Y) = ");
  Serial.print(KX122_Accel_Y_OUT);
  Serial.println(" g");
  Serial.print("KX122 (Z) = ");
  Serial.print(KX122_Accel_Z_OUT);
  Serial.println(" g");


  Serial.println();

 delay(1000);
}
/*
void I2C_CheckACK()
{
  if(I2C_check == false){
     while(1){
       Serial.write("No ACK!");
       Serial.write(0x0A); //Print Line Feed
       Serial.write(0x0D); //Print Carrage Return
       delay(500);
     }
  }
}
*/
