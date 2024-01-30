
/*

  Span UDP send sample

  Read the value from a potentiometer when a button is pressed and send it to the Span service
  (see https://span.lab5e.com/)

*/
#include <MKRNB.h>

GPRS gprs;
NB nbAccess;
NBUDP Udp;

// This is the local port
unsigned int localPort = 4200;

#define SPAN_APN "mda.lab5e"

// Use this value for the APN and comment out the line above if you are using 
// a COM4 SIM card
//#define SPAN_APN "lab5e.com4.net"

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Configure the pin for the microswitch
  pinMode(10, INPUT_PULLUP);

  // Connect to the mobile network. Initialise the modem, then GPRS
  Serial.println("Connecting...."); 
  boolean connected = false;
  while (!connected) {
    if ((nbAccess.begin(NULL, SPAN_APN) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected, retrying...");
      delay(1000);
    }
  }

  // Initialise the UDP sender class
  Udp.begin(localPort);
}

// This is the address and port of the Span service (172.16.15.14:1234)
IPAddress spanAddress(172, 16, 15, 14); 
unsigned int spanPort = 1234;

// This is the buffer we're going to send
#define PACKET_SIZE 128
byte packetBuffer[PACKET_SIZE];

// This is the potentiometer reading and button state (pressed or not, HIGH when not pressed, LOW when pressed)
int potmeterValue = 0;
int lastButtonState = HIGH;
int buttonState = HIGH;

void loop() {
  buttonState = digitalRead(10);

  // Check if the button has changed state (from HIGH to LOW) and that we haven't triggered 
  // earlier.
  if (buttonState == LOW && lastButtonState != buttonState) {
    potmeterValue = analogRead(A0);
    
    Serial.print("Sending packet with value ");
    Serial.println(potmeterValue);

    memset(packetBuffer, 0, PACKET_SIZE);
    sprintf((char*)packetBuffer, "Value is %d", potmeterValue);

    // Send the packet. This is sent as plain text but a proper firmware implementation should
    // use some kind of encoding to save power (and time).
    Udp.beginPacket(spanAddress, spanPort); 
    Udp.write(packetBuffer, strlen((const char*)packetBuffer));
    Udp.endPacket();
  }
  lastButtonState = buttonState;
  delay(100);  
}
