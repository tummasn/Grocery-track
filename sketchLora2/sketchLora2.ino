#include <rn2xx3.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 10);  // RX, TX
rn2xx3 myLora(mySerial);

const char *devAddr = "A8CA6E5A";
const char *nwkSKey = "7C127BAABBB316DEC7EC6A599E325CB3";
const char *appSKey = "B0EC844DB7866D31D0A43621F3CEDDA3";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(57600); //serial port to radio
  Serial.println("Startup");

  myLora.autobaud();

  Serial.print("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.print("RN2483 version number: ");
  Serial.println(myLora.sysver());

  myLora.initABP(devAddr, appSKey, nwkSKey);
  Serial.print("devAddr: ");
  Serial.println(devAddr);
  Serial.print("appSKey: ");
  Serial.println(appSKey);
  Serial.print("nwkSKey: ");
  Serial.println(nwkSKey);
  
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  String msg = "";
  Serial.print("Sending: ");
  Serial.println(msg);
  
  myLora.txUncnf("OK");

  Serial.println("Sent");

  delay(9000);
}
