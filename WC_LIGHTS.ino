/*
 * 
 * WC Lights...
 * When your family forgets to turn off lights :D
 * 
 * 1 PIR
 * 1 Arduino Nano
 * 1 NRF24L01
 * 1 LDR
 * 1 10K
 * 
 * Connections:
 * 
 * LDR : 
 * A0 
 * 10K V divider 
 * 
 * PIR : 
 * VCC: Nano 5V
 * GND: Nano GND
 * Data: Nano D2
 * 
 * NRF24L01
 * - - - ------
 * - - -
 * - - -
 * - - - 
 * 
 * 2 4 6 8
 * 1 3 5 7
 * 
 * 1 GND
 * 2 VCC (3.3)
 * 3 CE D9
 * 4 CSN D10
 * 5 SCK D13
 * 6 MOSI D11
 * 7 MISO D12
 * 8 Floating
 * 
 * Aim is to check PIR if high & LDR > 2.5V then send I'm on
 * else if was high then send I'm off.
 * 
 * @copy 2017 CG, MIT License
 */

#include <SPI.h>
#include "RF24.h"

#define PIR 2
#define CE 9
#define CS 10

int calib = 30;
int oneSec = 1000;


RF24 radio(CE, CS);

byte address[][6] = {"Node1"};
boolean pirWasOn = false;

struct WCState {
  byte pirState;
  float ldr;
};

void setup() {
  Serial.begin(9600);
  pinMode(PIR, INPUT_PULLUP);
  Serial.print("Calibrating PIR");
  for (int i = 0; i < calib; i++) {
    Serial.print(".");
    delay(oneSec);
  }
  Serial.println(" done");
  Serial.println("Sensor active");

  digitalWrite(PIR, LOW);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(address[0]);
  
}

void loop() {
  int raw = 0;

  float avgRaw = analogRead(0);
  
  float resistorV = (avgRaw / 1023) * 5;
  float ldr = 5 - resistorV;
  boolean pirState = digitalRead(PIR);
/*
  Serial.print("Raw: ");
  Serial.print(avgRaw);
  Serial.print(" ResistorV: ");
  Serial.print(resistorV);
  Serial.print(" ldr: ");
  Serial.print(ldr);

  Serial.print(" PIR: ");
  Serial.print(pirState);
  Serial.println("");
  */

  if (pirState == HIGH) {
    if (ldr > 2.5 && !pirWasOn) {
      WCState wc = {HIGH, ldr};
      radio.write(&wc, sizeof(wc));
      Serial.print(F("HIGH & "));
      Serial.println(ldr);
      pirWasOn = true;
    }
  } else {
    if (pirWasOn) {
      pirWasOn = false;
      WCState wc = {LOW, ldr};
      radio.write(&wc, sizeof(wc));
      Serial.print(F("LOW & "));
      Serial.println(ldr);
    }
    
  }

  delay(500);
}

