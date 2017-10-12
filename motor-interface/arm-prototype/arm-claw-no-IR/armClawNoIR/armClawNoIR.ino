#include <Servo.h>
 
Servo myservo;  // create servo object to control a servo
Servo myservo1;
 
int potpin = 1;  // analog pin used to connect the potentiometer
int potpin1 = 4;
int val,value;    // variable to read the value from the analog pin
 
void setup() {
  Serial.begin(9600);
  myservo.attach(2);  // attaches the servo on pin 2 to the servo object
  myservo1.attach(4);
}
 
void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  value = analogRead(potpin1);
  value = map(value, 0, 1023, 0, 180);  
  if(value < 10) value = 10;
  if(value > 122) value = 121;
  Serial.print("Value of wrist roll angle: ");
  Serial.println(value);
  myservo1.write(value);
  //if(val > 105) val=140;
  //if(val < 85) val=80;
  Serial.print("Value of wrist pitch angle: ");
  Serial.println(val);
  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there
}
