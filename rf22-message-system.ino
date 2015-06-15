/*
 rf22-message-system (timvrakas)
 this is an implementaion of the RadioHead Packet library for an advanced text-based communication system.
 */

#include <RHDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>
#include <EEPROM.h>

uint8_t greenLED = 6;
uint8_t redLED = 8;

uint8_t address = EEPROM.read(0);

// Singleton instance of the radio driver
RH_RF22 driver(10, 3);

// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, address);

uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];
uint8_t len;
uint8_t from;
uint8_t null[1] = { 6 };
boolean nodes[256];
int slowLoopSpeed = 5000;
long timer = millis();

void setup() {
	//Phase 1: Hardware initialization
	pinMode(greenLED, OUTPUT);
	pinMode(redLED, OUTPUT);
	digitalWrite(greenLED, HIGH);
	digitalWrite(redLED, LOW);
	Serial.begin(115200);
	if (!manager.init())
		Serial.println("Hardware Init Failed"); //@TODO This needs cause a reboot or throw error
	else
		Serial.println("Hardware Initialized");
}

void loop() {
	//read serial buffer and transmit
	while (Serial.available() > 0) {
		uint8_t len = Serial.readBytesUntil('\n', buf, RH_RF22_MAX_MESSAGE_LEN);
		Serial.print("#");
		Serial.print(address);
		Serial.print("> ");
		Serial.println((char*) buf);
		digitalWrite(redLED, HIGH);
		manager.sendto(buf, len, RH_BROADCAST_ADDRESS);
		digitalWrite(redLED, LOW);
		for (int i = 0; i < (RH_RF22_MAX_MESSAGE_LEN - 1); ++i)
			buf[i] = (char) 0;
	}
	//read radio buffer and print to serial
	while (manager.available()) {
		len = sizeof(buf);
		manager.recvfrom(buf, &len, &from);
		nodes[from] = 1;
		if (!(buf[0] == 6)) {
			Serial.print("#");
			Serial.print(from);
			Serial.print("> ");
			Serial.println((char*) buf);
		}
		for (int i = 0; i < (RH_RF22_MAX_MESSAGE_LEN - 1); ++i)
			buf[i] = (char) 0;
	}
	//this is 2sec loop, handles the peer identification.
	if ((millis() - timer) > slowLoopSpeed) {
		digitalWrite(redLED, HIGH);
		manager.sendto(null, 1, RH_BROADCAST_ADDRESS);
		digitalWrite(redLED, LOW);
		Serial.print("Found Nodes: ");
		for (uint8_t i = 1; i < 255; i++) {
			if (nodes[i]) {
				Serial.print(i);
				Serial.print(" ");
			}
		}
		Serial.println();
		timer = millis();
		for (uint8_t i = 1; i < 255; i++) {
			nodes[i] = 0;
		}
	}
}

