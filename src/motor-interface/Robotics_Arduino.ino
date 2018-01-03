//Required libraries for Adafruit Motor Shield
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Wire.h>

//DC motor encoder library
#include <Encoder.h>

//Servo motor library
#include <Servo.h>

//Create motor shield classes
Adafruit_MotorShield AFMS1 = Adafruit_MotorShield(0x61); //Creates class for first shield with address 61. Hardware must be set to this address
Adafruit_MotorShield AFMS2 = Adafruit_MotorShield(0x62); //Creates class for second shield with address 62. Hardware must be set to this address

//Connect stepper motors to shields
Adafruit_StepperMotor *Motor1 = AFMS2.getStepper(200, 1); //Connects the first motor to the second shield
Adafruit_DCMotor *Motor2 = AFMS1.getMotor(2); //Connects the second motor to the first shield
Adafruit_StepperMotor *Motor3 = AFMS1.getStepper(200, 1); //Connects the third motor to the first shield
Adafruit_StepperMotor *Motor4 = AFMS2.getStepper(200, 2); //Connects the fourth motor to the first shield

//Stepper motor objects
Servo Motor5;
Servo Motor6;

//Initialization
double kp[2], ki[2], kd[2];
unsigned long lastTime;
double Output;
double ITerm, lastInput, error;
double outMin, outMax;

Encoder DCEnc(3,2);
const int PIpin1 = 1;
const int PIpin3 = 2;
const int PIpin4 = 3;
const int PIpin5 = 4;
const double Max_Angle[6] = {90, 90, 90, 90, 90, 90}; // Minimum and maximum angles for each motor
const double Min_Angle[6] = {0 , 0 , 0 , 0 , 0 , 0 }; // Modify these lines when actual values are available
const double PIthresh = 60.0; //Photo interrupter values below this number would indicate motor movement. To be optimized with motors
const double ratio = 1.8; //Ratio of angles per tooth. Can be calculated by dividing 360 by the total number of teeth. If PI has different number of teeth, a multiple must be initialized
bool PI1move = false; //The following logicals indicate whether something is passing 
bool PI3move = false;
bool PI4move = false;
bool PI5move = false;
double PIval1,PIval3, PIval4, PIval5;
double Enc_dis[5] = {0, 0, 0, 0, 0};
double Motor_cmd_ang[6] = {0, 0, 0, 0, 0, 0};
double Motor_ang[6] = {0, 0, 0, 0, 0, 0};
int steps;
int i;

/////////////////////////////////////////////////////////////
//////////////////////////VOID SETUP//////////////////////////
/////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600); //Set up serial library at 9600 bps
  
  //Initializes the motor shields with the default frequency of 1.6KHz
  AFMS1.begin();
  AFMS2.begin(); 
  
  //Sets speeds for motors (Optimize with some quick maths)
  Motor1->setSpeed(150);
  Motor2->setSpeed(150);
  Motor2->run(RELEASE);
  Motor3->setSpeed(150);
  Motor4->setSpeed(150);

  //Attach servo motors
  Motor5.attach(10);
  Motor6.attach(9);
}

/////////////////////////////////////////////////////////////
//////////////////////////VOID LOOP//////////////////////////
/////////////////////////////////////////////////////////////

void loop() {
  //Read photo interrupter values
  PIval1 = analogRead(PIpin1);
  PIval3 = analogRead(PIpin3);
  PIval4 = analogRead(PIpin4);
  PIval5 = analogRead(PIpin5);
  
  
  //PI status checks
  if(PIval1 < PIthresh && !PI1move) {
    PI1move = true; 
    Serial.println("Photo interrupter 1 has detected movement. A distance of 1.8 degrees is assumed to be traveled"); //1.8 to be changed once PIs are installed
    if(Motor_cmd_ang[0] - Motor_ang[0] < 0) Enc_dis[0] -= ratio;
    else Enc_dis[0] += ratio;
  }
  
  else if(PIval1 > PIthresh && PI1move) {
    PI1move = false;
  }
  
  if(PIval3 < PIthresh && !PI3move) {
    PI3move = true; 
    Serial.println("Photo interrupter 3 has detected movement. A distance of 1.8 degrees is assumed to be traveled"); //1.8 to be changed once PIs are installed
    if(Motor_cmd_ang[2] - Motor_ang[2] < 0) Enc_dis[2] -= ratio;
    else Enc_dis[2] += ratio;
  }
  
  else if(PIval3 > PIthresh && PI3move) {
    PI3move = false;
  }
  
  if(PIval4 < PIthresh && !PI4move) {
    PI4move = true; 
    Serial.println("Photo interrupter 4 has detected movement. A distance of 1.8 degrees is assumed to be traveled"); //1.8 to be changed once PIs are installed
    if(Motor_cmd_ang[3] - Motor_ang[3] < 0) Enc_dis[3] -= ratio;
    else Enc_dis[3] += ratio;
  }
  else if(PIval4 > PIthresh && PI4move) {
    PI4move = false;
  }
  
  if(PIval5 < PIthresh && !PI5move) {
    PI5move = true; 
    Serial.println("Photo interrupter 5 has detected movement. A distance of 1.8 degrees is assumed to be traveled"); //1.8 to be changed once PIs are installed
    if(Motor_cmd_ang[4] - Motor_ang[4] < 0) Enc_dis[4] -= ratio;
    else Enc_dis[4] += ratio;
  }
  
   else if(PIval5 > PIthresh && PI5move) {
    PI5move = false;
  }

  //Check DC Encoder
  Motor_ang[1] = DCEnc.read();
  
  for(i=0;i<=5;i++) {
    //Verify motor position with encoder position
    if( (int)Enc_dis[i] != (int)Motor_ang[i] && i != 5) {
      Serial.print("ERROR: Motor ");
      Serial.print(i);
      Serial.println(" has discrepancies in position sensing. Calibrating to use encoder data.");
      Serial.print("Encoder distance: ");
      Serial.println((int)Enc_dis[i]);
      Serial.print("Motor distance: ");
      Serial.println((int)Motor_ang[i]);
      Motor_ang[i] = Enc_dis[i];
    }
    //Verify stepper motor command is in range
    if(Motor_cmd_ang[i] > Max_Angle[i] || Motor_cmd_ang[i] < Min_Angle[i]) {
      Serial.println("Error: Requested angle is not within bounds");
      Serial.print("Motor #");
      Serial.println(i+1);
      Serial.print(" angle: ");
      Serial.println(Motor_cmd_ang[i]);
    }
    else {
      //Compute Output and move motor 2
      Compute(1);
      if(Output < 0) {
        Motor2->setSpeed(-Output);
        Motor2->run(BACKWARD);
      }
      else {
        Motor2->setSpeed(Output);
        Motor2->run(FORWARD);
      }
      //Compute Output and move motor 5
      Compute(4);
      Motor5.write(Output);
      
      //Move stepper motors
      error = Motor_cmd_ang[i] - Motor_ang[i];
      steps = error/ratio; 
      if(i == 0) {
        if(error < 0) Motor1->step(-steps, BACKWARD, SINGLE);
        else Motor1->step(steps,FORWARD,SINGLE);
        Motor_ang[0] += steps;
      }
      if(i == 2) {
        if(error < 0) Motor3->step(-steps, BACKWARD, SINGLE);
        else Motor3->step(steps,FORWARD,SINGLE);
        Motor_ang[2] += steps;
      }
      if(i == 3) {
        if(error < 0) Motor4->step(-steps, BACKWARD, SINGLE);
        else Motor4->step(steps,FORWARD,SINGLE);
        Motor_ang[3] += steps;
      }

      //Move finger servo motor
      if(i == 5) Motor6.write(Motor_cmd_ang[i]);
    }
  }
  
  delay(50); //Delay between each loop. Could optimize depending on requirements
}


/////////////////////////////////////////////////////////////
//////////////////////////FUNCTIONS//////////////////////////
/////////////////////////////////////////////////////////////

double Compute(int i)
{
   /*How long since we last calculated*/
   unsigned long now = millis();
   double timeChange = (double)(now - lastTime);
  
   /*Compute all the working error variables*/
   error = Motor_cmd_ang[i] - Motor_ang[i];
   ITerm += (ki[i] * error * timeChange);
   if(ITerm > outMax) ITerm = outMax;
   else if(ITerm < outMin) ITerm - outMin;
   double dInput = (Motor_ang[i] - lastInput) / timeChange;
  
   /*Compute PID Output*/
   Output = kp[i] * error + ITerm - kd[i] * dInput;
   if(Output > outMax) Output = outMax;
   else if(Output < outMin) Output - outMin;
  
   /*Remember some variables for next time*/
   lastInput = Motor_ang[i];
   lastTime = now;
   return Output;
}

void SetTunings(int i, double Kp, double Ki, double Kd)
{
   kp[i] = Kp;
   ki[i] = Ki;
   kd[i] = Kd;
}

void SetOutputLimits(double Min, double Max)
{
   if(Min > Max) {
    Serial.println("Error: Controller output limit values make no sense.");
    return;
   }
   outMin = Min;
   outMax = Max;
    
   if(Output > outMax) Output = outMax;
   else if(Output < outMin) Output = outMin;
 
   if(ITerm> outMax) ITerm= outMax;
   else if(ITerm< outMin) ITerm= outMin;
}
