/*
    -----------------------------------
    Drip - Communication Hub Gateway (TEST)
    -----------------------------------
    Created by: Waleed Sawan
    Date: Dec. 18, 2018
    Description: Reads data recieved from the RF data transmitter (from the Field Probe) and relays it via Serial to the Uplink (Particle
    Photon). Also reads data from the Uplink and transmits it to the Field Valve. Integerates some basic RGB status lights. Does not integerate
    mesh. Works with one Field Probe and one Field Valve.
    
    Change Log:
    
    Date       | Change   
    ----------------------------------------------------------------------------------------------
    12/18/2017 | Created preliminary program to recieve data and put onto Serial prompt
    12/28/2017 | Added sending with Particle Photon (Uplink)
    12/29/2017 | Removed Serial Prompt
    12/31/2017 | Added recieving with Particle Photon (Uplink)
    01/14/2018 | Added sending to Field Valve
    01/18/2018 | Removed ability to send and recieve from more than two stations to reduce error
    03/13/2018 | Added RGB status light
    04/14/2018 | Added more RGB status lights, such as connecting, error, and local mode
    04/18/2018 | Fixed several bugs with communication with Particle Photon (Uplink)
*/

// Intializing different libraries to control RF data transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

// Intializing different variables to manage communication with Uplink, Probe, and Valve
int curMode = 0, esData = 0, connected = 0, cycle = 0, prevSend = 0;

// Intializing the RGB LED status light
const int numLEDs = 3;
int redPin = 6, greenPin = 5, bluePin = 3;
int redValue = 0, greenValue = 0, blueValue = 0;

// Setting up system clock for RGB LED delay
unsigned long previousMillis = 0, previousMillisSerial = 0; 
const long interval = 1000;

void setup()
{
  // Setting up the LEDs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Intializing Serial Communication with Uplink (Particle Photon)
  Serial.begin(9600);
  
  // Setting up the gateway to transmit and recieve
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002

  // Variable to determine whether or not to flash red for error
  boolean attemptConnect = true;
  
  // Milliseconds between flashes for the connecting time
  int flashSpeed = 250;
  
  // Recieving current time from onboard system clock
  unsigned long currentMillis = millis();
  
  while(connected != 1) // Ensure that the 
  {
      // Recieving current time from onboard system clock
      currentMillis = millis();
      
      // Check if the status light should be on or off
      if(currentMillis - previousMillis >= flashSpeed)
      {  
        previousMillis = currentMillis;
        if(attemptConnect)
        {
          if(currentMillis%15000 <= 2000 && currentMillis > 2000) // Turn RED for error
          {
            redValue = 255; greenValue = 0; blueValue = 0;
            flashSpeed = 100;
          }
          else // Turn GREEN for attempting connection
          {
            redValue = 0; greenValue = 255; blueValue = 0;
            flashSpeed = 250;  
          }  
          attemptConnect = false; // Turn status light off
        }
        else
        {
          if(currentMillis%15000 <= 2000 && currentMillis > 2000) // Different time interval for RED light vs. GREEN light
          {
            redValue = 0; greenValue = 0; blueValue = 0; 
            flashSpeed = 100;
          } 
          else
          {
            redValue = 0; greenValue = 0; blueValue = 0; 
            flashSpeed = 250;  
          } 
          attemptConnect = true; // Turn status light on
        }
      }
      setColor(redValue, greenValue, blueValue); // Set RGB colors accordingly
      connected = Serial.read(); // Attempt to connect to Uplink
  }
}

void loop() {
  // Recieving current time from onboard system clock
  unsigned long currentMillis = millis();
  
  if(prevSend < 2500 && (currentMillis - previousMillisSerial >= 6000)) // Reading data from Field Probe
  {
    previousMillisSerial = currentMillis;
    radio.startListening();
    if (radio.available())
    {
      radio.read(&esData, sizeof(esData));
      esData = esData / 10; // Dividing value by 10 to stay within the integer range of the Serial
      Serial.write(esData); // Writing value to the Uplink
    }
  }
  
  curMode = Serial.read(); // Recieve current mode for the Field Valve
  
  if(curMode != -1) // If new value is recieved
  {
    prevSend = 0;
    radio.stopListening();
    radio.write(&curMode, sizeof(curMode)); // Send value through transmission pipeline
  }
  
  if(currentMillis - previousMillis >= 10) // Make RGB LED breath CYAN 
  {
    prevSend++; 
    previousMillis = currentMillis;
    if(prevSend >= 2500) // If disconnected for over 25s, change to YELLOW breathing
      setColor((sin(cycle * 0.0174533) + 1) * 127, 0, (sin(cycle * 0.0174533) + 1) * 127);
    else // Breath CYAN
      setColor(0, (sin(cycle * 0.0174533) + 1) * 127, (sin(cycle * 0.0174533) + 1) * 127);
    cycle++;
  }
}

// Function to set color of the RGB LED
void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
}
