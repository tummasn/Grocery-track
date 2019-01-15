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
const byte DHTPIN= 13;
const long DHTTYPE = DHT22;

//Defining the humidity and temp sensor
DHT dht(DHTPIN,DHTTYPE);

//Defining the accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


// Time and interval 
unsigned long previousMillisSensor = 0;
unsigned long previousMillisUpload = 0;
unsigned long intervalSensor = 2000;
unsigned long intervalUpload = 6*1000;

// Temperature, humidity and acceleration
float tempSum = 0;
float humiditySum = 0;
float numberOfDatapoints = (float) intervalUpload/intervalSensor;
float maxAcc = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
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

    tempSum = t + tempSum;
    humiditySum = h + humiditySum;
  }
  //Upload the variables in the desired interval
  if (currentMillis - previousMillisUpload >= intervalUpload){
    previousMillisUpload = currentMillis;

    float meanTemp = tempSum/numberOfDatapoints;
    float meanHumidity = humiditySum/numberOfDatapoints;

    Serial.print("Mean Temperature: ");
    Serial.println(meanTemp);
    Serial.print("Mean Humidity: ");
    Serial.println(meanHumidity);
    Serial.print("Maximum acceleration: ");
    Serial.println(maxAcc);

    tempSum=0;
    humiditySum=0; 
    maxAcc=0;

  }
}
