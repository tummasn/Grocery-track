#include <SoftwareSerial.h>   //software serial for the rn2483
#include <rn2xx3.h>
#include <CborBuilder.h>

SoftwareSerial mySerial(11, 10);  // RX, TX

#include <ATT_LoRaWAN.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600
#define CBOR
rn2xx3 myLora(mySerial);

#ifdef CBOR
  #include <CborBuilder.h>
  CborBuilder payload(mySerial);
#endif



MicrochipLoRaModem modem(&loraSerial, &debugSerial);
ATTDevice device(&modem, &debugSerial, false, 7000);  // minimum time between 2 messages set at 7000 milliseconds



void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(57600); //serial port to radio

  myLora.autobaud();

  Serial.print("DevEUI: ");
  Serial.println(myLora.hweui());
}

void loop() {
  // put your main code here, to run repeatedly:

}
