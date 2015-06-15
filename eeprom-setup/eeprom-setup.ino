#include <EEPROM.h>

uint8_t greenLED = 6;
uint8_t redLED = 8;

void setup(){
EEPROM.write(0,2);
pinMode(greenLED, OUTPUT);
pinMode(redLED, OUTPUT);
}

void loop(){
digitalWrite(greenLED, HIGH);
digitalWrite(redLED, LOW);
delay(500);
digitalWrite(greenLED, LOW);
digitalWrite(redLED, HIGH);
delay(500);
}


