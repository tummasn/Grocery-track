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

#include <rn2xx3.h>               //liberary for the rn2483 chip
#include <SoftwareSerial.h>       //software serial liberary

SoftwareSerial mySerial(11, 10);  // RX, TX
rn2xx3 myLora(mySerial);          //assigning the serial for the Lora

//the keys needed for communication
const char *devAddr = "A8CA6E5A";
const char *nwkSKey = "7C127BAABBB316DEC7EC6A599E325CB3";
const char *appSKey = "B0EC844DB7866D31D0A43621F3CEDDA3";

//pins for the temperature and humidity sensor
const byte DHTPIN= 13;      
const long DHTTYPE = DHT22;

//Defining the humidity and temp sensor using the liberary
DHT dht(DHTPIN,DHTTYPE);

//Defining the accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


// Time and interval 
unsigned long previousMillisSensor = 0;
unsigned long previousMillisUpload = 0;
unsigned long previousMillisSend =0;
unsigned long intervalSensor = 2000;
unsigned long intervalUpload = 6*1000;
unsigned long intervalSend = 30*1000;


// Temperature, humidity and acceleration
float tempSum = 0;
float humiditySum = 0;
float numberOfDatapoints = (float) intervalUpload/intervalSensor;
float maxAcc = 0;

//threshold values
float minTemp = 5;
float maxTemp = 26;
float minHum = 0;
float maxHum = 70;
float treAcc = 15;

void setup() {
  Serial.begin(57600);          //serial port to computer
  mySerial.begin(57600);        //serial port to radio
  Serial.println("Startup");
  dht.begin();
  accel.begin();
  accel.setRange(ADXL345_RANGE_16_G);   // sets range of accelerometer

  //Initializing the radio and printing different addresses to the serial monitor
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
  
  delay(2000);      //allowing time to start up
}

//function for sending 
int sendingFunction(float meanTemp, float meanHumidity, float maxAcc){
    int norm=1;
    if (maxAcc>treAcc){       //if acceleration is over treshold
      String msg = String("Acceleration: ")+maxAcc;      //send a string
      //myLora.txUncnf(msg);
      Serial.println(msg);
      myLora.txUncnf(msg);
      norm =0;
      delay(5000);}
    else if (meanTemp>maxTemp||meanTemp<minTemp){
      String msg = String("Temperature:  ")+meanTemp;
      //myLora.txUncnf(msg);
      Serial.println(msg);
      myLora.txUncnf(msg);
      norm =0;
      delay(5000);}

    else if (meanHumidity>maxHum||meanHumidity<minHum){
      String msg = String("Humidity: ")+meanHumidity;
      //myLora.txUncnf(msg);
      Serial.println(msg);
      myLora.txUncnf(msg);
      norm =0;
      delay(5000);}
    return norm;
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

    tempSum = t + tempSum;
    humiditySum = h + humiditySum;
  }
  //Upload the variables in the desired interval
  if (currentMillis - previousMillisUpload >= intervalUpload){
    previousMillisUpload = currentMillis;

    float meanTemp = tempSum/numberOfDatapoints;
    float meanHumidity = humiditySum/numberOfDatapoints;

    int val = sendingFunction(meanTemp, meanHumidity, maxAcc);

    if (currentMillis - previousMillisSend >= intervalSend){
      if (val==1){
        previousMillisSend = currentMillis;
        myLora.txUncnf("OK");
        Serial.println("Sent OK");
        }
      }
    
    tempSum=0;
    humiditySum=0; 
    maxAcc=0;

  }
}
