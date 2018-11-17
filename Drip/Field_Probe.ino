/*
    -----------------------------------
    Drip - Field Probe Transmitter (TEST)
    -----------------------------------

    Created by: Waleed Sawan
    Date: Feb. 1, 2018
    Description: Simply reads data from the moisture sensor and sends it through the RF pipeline to the Central Communication Hub. 
    Basic program to merely test the underlying capabilities of the hardware and identify any stress points. Does not integerate
    mesh.
    
    Change Log:
    
    Date       | Change   
    ------------------------------------------------------------------------------
    02/01/2018 | Created preliminary program to read data and put on Serial prompt
    02/06/2018 | Added RF transmission capabilities
    11/09/2018 | Removed Serial Prompt
*/

// Intializing different libraries to control RF data transmitter
#include <SPI.h> 
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

// Intializing sensor
int mstrSens = A0;
int mstrRead = 0;

void setup() {
  // Setting up the sensor
  pinMode(mstrSens, INPUT);
  
  // Setting up the probe to transmit
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(addresses[0]); // 00002
}

void loop() {
  // Delaying data to slow down transmission slightly
  delay(5);
  
  // Reading data
  myData = analogRead(mstrSens);
  
  // Writing data through pipeline
  radio.write(&myData, sizeof(myData));
}
