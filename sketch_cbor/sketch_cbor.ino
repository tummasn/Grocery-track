
#include <CborBuilder.h>
CborBuilder payload(device);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  payload.reset();
  payload.map(1);
  payload.addInteger(counter, "15");
  payload.addToQueue(false);
  process();
}
