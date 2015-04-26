#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>

#define ADDRESS 3

uint8_t greenLED = 6;
uint8_t redLED = 8;
// Singleton instance of the radio driver
RH_RF22 driver(10,3);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, ADDRESS);

void setup() 
{
  pinMode(greenLED, OUTPUT);
  pinMode(redLED,OUTPUT);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  Serial.println("System Started");
}

uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];

void loop()
{
  while (Serial.available() > 0){
  uint8_t len = Serial.readBytesUntil('\n', buf, RH_RF22_MAX_MESSAGE_LEN);
  Serial.print("#");
  Serial.print(ADDRESS);
  Serial.print("> ");
  Serial.println((char*)buf);
  digitalWrite(redLED, HIGH);
  manager.sendtoWait(buf, len, RH_BROADCAST_ADDRESS);
  digitalWrite(redLED, LOW);
  for( int i = 0; i < sizeof(buf);  ++i )
  buf[i] = (char)0;
}

uint8_t len = sizeof(buf);
uint8_t from;
while (manager.recvfromAck(buf, &len, &from)){
  Serial.print("#");
  Serial.print(from);
  Serial.print("> ");
  Serial.println((char*)buf);
  for( int i = 0; i < sizeof(buf);  ++i )
  buf[i] = (char)0;
  }
}

