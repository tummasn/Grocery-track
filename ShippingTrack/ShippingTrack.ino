/*What the code does
 * 
 * Creators 
 * Lise Hanson
 * Tummas Napoleon Arge
 * 
 *Initialize 
 */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>     //accelerometer
#include "DHT.h"                    //humidity and temperature sensor
#include <rn2xx3.h>                 //Lora radio liberary
#include <SoftwareSerial.h>         //Software serial
const byte DHTPIN= 13;
const long DHTTYPE = DHT22;


//Defining the humidity and temp sensor
DHT dht(DHTPIN,DHTTYPE);

//Defining the accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


//Defining the software serial and the lora device
SoftwareSerial mySerial(11, 10);  // RX, TX
rn2xx3 myLora(mySerial);

//Addresses used for the Lora transmission
const char *devAddr = "00AA2531";
const char *nwkSKey = "2E58B49044825A56F0599B58C080BB1F";
const char *appSKey = "23DD85B730E0E323304EC850847A2523";

// Time and interval 
unsigned long previousMillisSensor = 0;
unsigned long previousMillisUpload = 0;
unsigned long previousMillisRate = 0;
unsigned long intervalSensor = 2000;
unsigned long intervalUpload = 15000;
unsigned long intervalRate = 8000;

// Temperature, humidity and acceleration
float tempSum = 0;
float humiditySum = 0;
float numberOfDatapoints = (float) intervalRate/intervalSensor;
float maxAcc = 0;
float meanTemp;
float meanHumidity;

//threshold values
float minTemp = 10;
float maxTemp = 20;
float minHum = 0;
float maxHum = 70;
float treAcc = 15;



void initialize_radio(){        //Initialises radio and connects to network
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
  Serial.println("Trying to join Loriot");
  bool join_result = false;



  //OTAA: initOTAA(String AppEUI, String AppKey);
  join_result = myLora.initOTAA("BE7A000000001109", "8EE6B8477D855AB78E386A047F37ACA6");

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined Loriot");

}


bool sendingFunction(float meanTemp, float meanHumidity, float maxAcc){
    bool norm = true;
    if (maxAcc>treAcc){       //if acceleration is over treshold
      String msg = String("A")+maxAcc;      //send a string
      myLora.txUncnf(msg);
      norm =false;}
    else if (meanTemp>maxTemp||meanTemp<minTemp){
      String msg = String("T ")+meanTemp;
      myLora.txUncnf(msg);
      Serial.println(msg);
      norm =false;}

    else if (meanHumidity>maxHum||meanHumidity<minHum){
      String msg = String("H")+meanHumidity;
      myLora.txUncnf(msg);
      norm =false;}
}

void setup() {  
  Serial.begin(57600);       //serial port to computer
  mySerial.begin(57600);     //serial port to radio
  delay(1000);              // give the radio time to start up
  initialize_radio();
  dht.begin();              // start humidity and temp sensor
  accel.begin();
  accel.setRange(ADXL345_RANGE_16_G);   // sets range of accelerometer
}

void loop() {
  sensors_event_t event;      
  accel.getEvent(&event);     //calls the event

  float acc = (float) sqrt(pow(event.acceleration.x,2)+pow(event.acceleration.y,2)+pow(event.acceleration.z,2));
  maxAcc = max(acc,maxAcc);   //calculate length of acceleration vector and compare it to the previous max

  //Temperature and humidity in intervals of 2 seconds 
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisSensor >= intervalSensor) {   
    previousMillisSensor = currentMillis;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.println(t);
    tempSum = t + tempSum;
    humiditySum = h + humiditySum;
  }
  //Update the variables in the desired interval
  if (currentMillis - previousMillisRate >= intervalRate){
    previousMillisRate = currentMillis;

    meanTemp = tempSum/numberOfDatapoints;  //taking the mean
    meanHumidity = humiditySum/numberOfDatapoints;
    Serial.print("Mean Temp: ");Serial.println(meanTemp);

    sendingFunction(meanTemp, meanHumidity, maxAcc);
    tempSum=0;
    humiditySum=0; 
    maxAcc=0;
    }
   
   if (currentMillis - previousMillisUpload >= intervalUpload){
      if (sendingFunction(meanTemp,meanHumidity,maxAcc)){
        previousMillisUpload = currentMillis;
        myLora.txUncnf("OK");
        Serial.print("Sent OK");}
      }

}
