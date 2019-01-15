int out_pin = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(out_pin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(out_pin,HIGH);
  delay(1000);
  digitalWrite(out_pin,LOW);
  delay(1000);
}
