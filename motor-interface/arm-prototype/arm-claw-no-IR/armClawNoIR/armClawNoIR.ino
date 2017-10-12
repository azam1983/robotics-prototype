#include <Servo.h>
 
Servo pitchServo;  // create servo object to control a servo, pitch
Servo rollServo; // roll
 
int potpin = 1;  // analog pin used to connect the potentiometer
int potpin1 = 4;
int pitchPotentioVal,rollPotentioVal;    // variables to read the value from the analog pin
const int MIN_VAL = 10;
const int MAX_VAL = 122;

void setup() {
    Serial.begin(9600);
    pitchServo.attach(2);  // attaches the servo on pin 2 to the servo object
    rollServo.attach(4);
}
 
void loop() {
    pitchPotentioVal = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
    pitchPotentioVal = map(pitchPotentioVal, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
    rollPotentioVal = analogRead(potpin1);
    rollPotentioVal = map(rollPotentioVal, 0, 1023, 0, 180);  
  
    if (rollPotentioVal < MIN_VAL) {
        rollPotentioVal = 10;
    }
    
    if (rollPotentioVal > MAX_VAL) {
        rollPotentioVal = 121;
    }
    
    Serial.print("Value of wrist roll angle: ");
    Serial.println(rollPotentioVal);
    rollServo.write(rollPotentioVal);
    
    //if(val > 105) val=140;
    //if(val < 85) val=80;
    
    Serial.print("Value of wrist pitch angle: ");
    Serial.println(pitchPotentioVal);
    pitchServo.write(pitchPotentioVal);  // sets the servo position according to the scaled value
    delay(15);                           // waits for the servo to get there
}

