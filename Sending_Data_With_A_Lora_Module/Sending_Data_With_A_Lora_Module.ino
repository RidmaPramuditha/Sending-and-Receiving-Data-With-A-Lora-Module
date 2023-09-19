// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      4   // My node ID
//#define TONODEID      1   // Destination node ID
byte TONODEID    =  0;   // Destination node ID

// RFM69 frequency, uncomment the frequency of your module:

#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT       true // Set to "true" to use encryption
//#define ENCRYPT       false // Set to "true" to use encryption
//#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes
#define ENCRYPTKEY    "1234567812345678" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        true // Request ACKs or not

// Packet sent/received indicator LED (optional):

//#define LED           9 // LED positive pin
//#define GND           8 // LED ground pin

#define LED_RED             4 // LED positive pin
#define LED_GREEN           5 // LED positive pin
#define LED_BLUE            6 // LED ground pin
const int BUTTON_PIN = 8; // PUSH BUTTON positive pin

// Create a library object for our RFM69HCW module:

RFM69 radio;

//----------------------------------------------------------------------------------------
int ledState = LOW;             // ledState used to set the LED
int buttonState = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated 
unsigned long previousMillis_data = 0;
// constants won't change:
const long interval = 3000; 
const long interval_data = 16000;
byte Send_Data_Flag = 0;
//----------------------------------------------------------------------------------------
void setup()
{
  // Open a serial port so we can send keystrokes to the module:

  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println("ready");  

  // Set up the indicator LED (optional):

  pinMode(LED_GREEN,OUTPUT);
  digitalWrite(LED_GREEN,LOW);
  pinMode(LED_BLUE,OUTPUT);
  digitalWrite(LED_BLUE,LOW);
  pinMode(BUTTON_PIN, INPUT);


  // Initialize the RFM69HCW:
  // radio.setCS(10);  //uncomment this if using Pro Micro
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:

  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

void loop()
{
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH) {
    
    lora();

    
  } else {
    
  }
  
}

void lora()
{
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[62];
  static int sendlength = 0;

  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).

  // If there is any serial input, add it to the buffer:

  if (Send_Data_Flag == 1) // CR or buffer full
  {
    // Send the packet!
    sendlength = 1;
    sendbuffer[0] = 0x41 ; 
    
    Serial.print("sending to node ");
    Serial.print(TONODEID, DEC);
    Serial.print(", message [");
    for (byte i = 0; i < sendlength; i++)
      Serial.print(sendbuffer[i]);
    Serial.println("]");
    
    // There are two ways to send packets. If you want
    // acknowledgements, use sendWithRetry():

    if (USEACK)
    {
      if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
        Serial.println("ACK received!");
      else
        Serial.println("no ACK received");
    }

    // If you don't need acknowledgements, just use send():

    else // don't use ACK
    {
      radio.send(TONODEID, sendbuffer, sendlength);
    }

    sendlength = 0; // reset the packet
    Send_Data_Flag = 0;
    Blink(LED_RED,300);
  }


  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    Blink(LED_GREEN,300);
  }
//-----------------------------------------------------------
unsigned long currentMillis = millis(); // HB LED

  if (currentMillis - previousMillis >= interval) 
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    digitalWrite(LED_BLUE, HIGH);
    delay(10);
    digitalWrite(LED_BLUE, LOW);
  }

  if (currentMillis - previousMillis_data >= interval_data)  // DATA Tx
  {
    previousMillis_data = currentMillis;
    Send_Data_Flag = 1;
//    Serial.print("Send_Data_Flag = 1");
    if (TONODEID == 1)
    {
      TONODEID = 0;
    }
    TONODEID++;
  }
//-----------------------------------------------------------
}


void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
