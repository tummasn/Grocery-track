#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include<CayenneLPP.h>

CayenneLPP lpp(51);

SoftwareSerial mySerial(11, 10);  // RX, TX
rn2xx3 myLora(mySerial);

const char *devAddr = "00AA2531";
const char *nwkSKey = "2E58B49044825A56F0599B58C080BB1F";
const char *appSKey = "23DD85B730E0E323304EC850847A2523";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(57600); //serial port to radio
  delay(1000);

  myLora.tx("TTN Mapper on ESP8266 node");
  initialize_radio();

}

void initialize_radio(){
  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;



  //OTAA: initOTAA(String AppEUI, String AppKey);
  join_result = myLora.initOTAA("BE7A000000001109", "8EE6B8477D855AB78E386A047F37ACA6");

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}



void loop() {
  // put your main code here, to run repeatedly:
  String json ="10";//"{\"sense\":[\"asset\":\"t\", \"value\":{10}]}"; //"{\"sense\":[\"asset\":\"t\",\"type\":\"number\", \"value\":\"10\"]}"; //"{\"sense\":[\"asset\":{\"byte\":4,\"bytelength\":2,\"type\":\"integer\"}]}";
  //"{\"temperature\":23}";
  lpp.reset();
  lpp.addTemperature(1,22.5);

  Serial.print(lpp.getBuffer());
  Serial.print(lpp.getSize());
  
  //myLora.tx(lpp.getBuffer(),lpp.getSize()); 
  
  delay(2000);}
