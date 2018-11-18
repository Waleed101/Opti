/*
    -----------------------------------
    Drip - Communication Hub Uplink (TEST)
    -----------------------------------
    Created by: Waleed Sawan
    Date: Dec. 18, 2018
    Description: Reads data recieved from the Gateway (Arduino Uno) via Serial and publishes data to the Particle cloud. Also can
    recieve data from the Drip MySQL database and send it back to the Gateway (Arduino Uno) to be sent to the valve. A event must
    be setup on the Particle cloud for the valve portion to work.
    
    
    Change Log:
    
    Date       | Change   
    ----------------------------------------------------------------------------------------------
    12/26/2017 | Created preliminary program to publish random data to the Particle console
    12/28/2017 | Added recieving with Arduino Uno (Gateway)
    12/30/2017 | Added reading data from MySQL database
    12/31/2017 | Added sending with Arduino Uno (Gateway)
    01/18/2018 | Removed ability to send and recieve from more than two stations to reduce error
    04/18/2018 | Fixed several bugs with communication with Arduino Uno (Gateway)
*/

// Control publishing the data
char publishString[60];
unsigned long lastTime = 0UL;

// Hold valve status
int valveStatus = 0; 

void setup()
{
    Serial1.begin(9600); // Initialize Serial
    for(int i = 0; i < 11; i++)
    {   
        Serial1.write(1); // Send to Communication Hub Gateway
        Serial1.flush(); // Wait for Serial to finish sending
    }
    Particle.publish("Status", "Currently On and Check!"); // Ensure publish is on
    Particle.subscribe("hook-response/valve", TokenHandler, MY_DEVICES); // Subscribe to the valve event on the Particle Cloud
}

void loop() {
    unsigned long now = millis(); // Get current time from system clock
    if (now - lastTime > 6000UL) // Only publish every 6s to not exceed Particle Cloud limit of 10 events per minute
    {
      valveStatus = Serial1.read();
      sprintf(publishString, "%i", valveStatus); 
      Particle.publish("value", publishString); // Publish data recieved from the Gateway
      sprintf(publishString, "%i", 298); 
      Particle.publish("valve", publishString); // Publish valve ID to trigger event
      lastTime=millis();
    }
    Serial1.write(valveStatus); // Write valve status to Gateway 
    delay(10);
}

// Function to recieve data from Valve event on the Particle Cloud
void TokenHandler(const char *name, const char *data) {
    valveStatus = atoi(String(data));
}
