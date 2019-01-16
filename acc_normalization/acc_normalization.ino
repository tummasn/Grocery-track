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
#include <math.h>

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
unsigned long intervalUpload = 1*1000;

// Temperature, humidity and acceleration
float tempSum = 0;
float humiditySum = 0;
float numberOfDatapoints = (float) intervalUpload/intervalSensor;
float maxAcc_xy = 0;
float maxAcc_z  = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  accel.begin();
  accel.setRange(ADXL345_RANGE_16_G);   // sets range of accelerometer
}

void loop() {
  sensors_event_t event;      
  accel.getEvent(&event);     //calls the event
  unsigned long currentMillis = millis();

  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;

  

  
  //Upload the variables in the desired interval
  if (currentMillis - previousMillisUpload >= intervalUpload){
    previousMillisUpload = currentMillis;
    normalization(ax,ay,az);
    
    /*
    Serial.print("Maximum acceleration in xy-plane: ");
    Serial.println(maxAcc_xy);
    Serial.print("Maximum acceleration in z: ");
    Serial.println(maxAcc_z);
    tempSum=0;
    humiditySum=0; 
    maxAcc_xy=0;
    maxAcc_z=0;
    */
  }
}

void normalization(float ax, float ay, float az){
  float theta = (float) atan(az/ax);
  float ax1 = sin(theta)*ax;
  float ax2 = sin(3.14/2-theta)*az;

  float az1 = cos(theta)*ax;
  float az2 = cos(3.14/2-theta)*az;

  Serial.print("X: ");Serial.println(ax1+ax2);
  Serial.print("Z: ");Serial.println(az1+az2);
  Serial.print("angle: ");Serial.println(theta);


}
