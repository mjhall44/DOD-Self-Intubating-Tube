//final CO2 Sensor algo
//feel free to add anything

//COZIR sensor initialization
#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 13); // RX, TX pins on Ardunio

//global variables
int co2 =0;
double multiplier = 1;// 1 for 2% =20000 PPM, 10 for 20% = 200,000 PPM
double prev[3];
uint8_t buffer[25];
uint8_t ind =0;
uint8_t index =0;

int fill_buffer();  // function prototypes here
int format_output();



//copyrights
/*
AN128_ardunio_cozir CO2 Demonstration code 11/29/2017 Runs on Ardunio UNO, MEGA or MEGA2560
  Written by: Marv Kausch 11/22/2016 or Co2meter.com
  Revised 11/29/17 by John Houck
   
  This sketch connects a COZIR sensor and reports readings back to the host computer over USB.
  The value is stored in a global variable 'co2' and can be used for any number of applications.
   
  pin connections:
   
  Arduino________COZIR Sensor
   GND ------------------ 1 (gnd)
   3.3v------------------- 3 (Vcc)  
    13 -------------------- 5 (Rx)
    12 -------------------- 7 (Tx)
*/

//Edits by UT BME Team 5 SeniorDesign (Kolby Killion and Matthew Hall)
// Created previous value buffer for CO2 sensor
// added variables to ensure accuracy of values
// lowered refresh rate
// Last Edit CO2 03/08

void setup() {
  // begins CO2 print with test to serial port ONLY
Serial.begin(9600);
  //Serial.print("\n\n");
  //Serial.println("             AN128 Ardunio to Cozir CO2 Sensor - Demonstration code 11/29/2017\n\n"); 
  mySerial.begin(9600); // Start serial communications with sensor
  //mySerial.println("K 0");  // Set Command mode
  mySerial.println("M 6"); // send Mode for Z and z outputs
  // "Z xxxxx z xxxxx" (CO2 filtered and unfiltered)

  mySerial.println("K 1");  // set streaming mode
  fill_buffer();



}

void loop() {
  // put your main code here, to run repeatedly:
  index = 8; //filters output to correct value from raw data (binary to PPM)
  while(format_output() < 0){
  delay(20);
  fill_buffer();
  delay(20);
  } 
  Serial.println();
  fill_buffer(); // function call that reacds CO2 sensor and fills buffer
  /* Serial.print("Buffer contains: ");     // Uncomment for raw output
 // for(int j=0; j<ind; j++)Serial.print(buffer[j],HEX);
 index = 0;
 format_output();
 Serial.print(" Raw PPM        ");

*/
 
  index = 8;  // In ASCII buffer, filtered value is offset from raw by 8 bytes
  format_output();
  Serial.println(" Filtered PPM\n");
  delay(200);
}

int fill_buffer(void){
// Fill buffer with sensor ascii data
//partially edited from freedomain ascii code (Marv Kausch and Rev by John Houck)
  ind = 0;
  while(buffer[ind-1] != 0x0A){  // Read sensor and fill buffer up to 0XA = CR
    if(mySerial.available()){
      buffer[ind] = mySerial.read();
      ind++;
      } 
    }
  // buffer() now filled with sensor ascii data
  // ind contains the number of characters loaded into buffer up to 0xA =  CR
  ind = ind -2; // decrement buffer to exactly match last numerical character
  }

 int format_output(void){ // read buffer, extract 6 ASCII chars, convert to PPM and print
   co2 = buffer[15-index]-0x30;
  co2 = co2+((buffer[14-index]-0x30)*10);
  co2 +=(buffer[13-index]-0x30)*100;
  co2 +=(buffer[12-index]-0x30)*1000;
  co2 +=(buffer[11-index]-0x30)*10000;
  delay(100);
  //Serial.print("\n CO2 = ");
  
  Serial.print(co2*multiplier,0);
// Serial.print(" PPM,");
//    Serial.print("\n");
return co2*multiplier;
 }
