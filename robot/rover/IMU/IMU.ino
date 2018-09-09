#include <Wire.h>

// Addresses for control registers
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

// Full reading rage values
#define scale_acc 2 // 2g, 4g or 8g
#define scale_gyro 250 // 250, 500 or 2000 deg/s

// Define constants
#define PI 3.1415926535897932384626433832795

// Device addresses
int L3GD20_Address = 0x6B; //I2C address of the L3GD20
int LSM303C_Address = 0x1D; //I2C address of the LSM303C

// Gyro raw outputs
int x_gyro_raw;
int y_gyro_raw;
int z_gyro_raw;

// Gyro converted raw output
int x_gyro;
int y_gyro;
int z_gyro;

// Angular position give by gyro
float gyro_pitch_angle;
float gyro_roll_angle;
float gyro_yaw_angle;

// For calibration purposes
bool set_gyro_angles = false;
float gyro_x_cal,gyro_y_cal,gyro_z_cal;
float acc_x_cal, acc_y_cal, acc_z_cal;

// Accelerometer raw output
int x_acc_raw;
int y_acc_raw;
int z_acc_raw;


// Accelerometer converted raw output
float x_acc;
float y_acc;
float z_acc;

// Angular position given by acc outputs
float acc_pitch_angle;
float acc_roll_angle;
float acc_yaw_angle;

// Time related variables
unsigned long start, finished, elapsed;
float dt = 0.015;

// Averaged pitch, roll, and yaw angles
float angle_pitch, angle_roll, angle_yaw;

// Weight
float gyro_weight = 0.20000;
float acc_weight = 0.80000;

// Filters
bool simple_w_avg = true;

// Kalman Filter variables
float KG; // Kalman gain
float EST_t_0 = 0.00; // Previous estimate
float EST_t_1; // Current Estimate
float EST_error = 0.10; // Estimate Error
float MEA = 0;
float MEA_error = 10.00; // Measurement
 
void setup(){
 
  Wire.begin();
  Serial.begin(9600);
 
  Serial.println("starting up L3GD20");
  setupL3GD20(scale_gyro); // Configure L3GD20  - 250, 500 or 2000 deg/sec
  
  // Digital pins setup for gyro
  pinMode(2, OUTPUT); // X +Ve value
  pinMode(3, OUTPUT); // X -Ve value
  pinMode(4, OUTPUT); // Y +Ve value
  pinMode(5, OUTPUT); // Y -Ve value
  pinMode(6, OUTPUT); // Z +Ve value
  pinMode(7, OUTPUT); // Z -Ve value

  Serial.println("starting up LSM303C"); 
  setupLSM303C(scale_acc); // Configure LSM303C - 2, 4, or 8 g

  // Find gyro offset and store it in gyro_cal variable
  calibrateGyro();

  // Calibrate accelerometer
  calibrateAccelerometer();
 
  delay(100); //wait for the sensor to be ready
}
 
void loop(){

  start = millis(); // Time elapsed since program started
  
  getGyroValues();  // This will update x, y, and z with new values
  
  applyGyroCalibrationValues(); // Compensate for Gyro readings' offset
  
  convertGyroValues(scale_gyro); // This will convert the raw outputs into a factor of 1dps depending on the full range specified: 250, 500 or 2000 dps
  
  convertGyroValuesToAngle(); // This calculates the angular position from the gyro's angular rate
  
  getAccelerometerValues(); // This will update x, y, and z with new values
  
  convertAccelerometerValues(scale_acc); // This will convert the raw outputs into a factor of 1g depending on the full range specified: 2g, 4g or 8g
  
  convertAccelerometerValuesToAngle(); // This calculates the x, y and z angles from the accelerometer's outputs

  // Calibrate Acc outputs:
  applyAccCalibrationValues();

  // Apply weighted average
  applyWeigthedAverage(simple_w_avg);

  // Accelerometer Values
    Serial.print(acc_pitch_angle);
    Serial.print(",");
    Serial.print(acc_roll_angle);
    Serial.print(",");
    // Serial.println();

  // Gyro Values
   Serial.print(gyro_pitch_angle);
   Serial.print(",");
   Serial.print(gyro_roll_angle);
   Serial.print(",");
   Serial.print(gyro_yaw_angle);
   Serial.println();
 
  delay(50); //Just here to slow down the serial to make it more readable

  // Calculate dt
  calculateChangeInTime(true);
}

// This routine takes both the accelerometer readings and the gyro readings to create a weighted average reading as per the specified weights declares above
void applyWeigthedAverage(bool cond){

  if (cond){
      if(set_gyro_angles){
      angle_pitch = gyro_pitch_angle*gyro_weight + acc_pitch_angle*acc_weight;
      angle_roll = gyro_roll_angle*gyro_weight + acc_roll_angle*acc_weight;  
    } else{
      angle_pitch = acc_pitch_angle;
      angle_roll = acc_roll_angle;
      set_gyro_angles = true;
    }
  }
}

// This routine finds the average of 2000 gyro readings and stores it in gyro_[axis]_cal variables
void calibrateGyro(){

  Serial.println();
  Serial.print("Calibrating Gyro");
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  
    if(cal_int % 125 == 0)Serial.print(".");                           
    getGyroValues();                                                   
    gyro_x_cal += x_gyro_raw;                                          
    gyro_y_cal += y_gyro_raw;                                          
    gyro_z_cal += z_gyro_raw;                                          
    delay(3);                                                          
  }
  gyro_x_cal /= 2000;                                                  
  gyro_y_cal /= 2000;                                                  
  gyro_z_cal /= 2000;
}

// This routine finds the average of 2000 accelerometer readings and stores it in acc_[axis]_cal variables
void calibrateAccelerometer(){

  Serial.println();
  Serial.print("Calibrating Accelerometer");
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  
    if(cal_int % 125 == 0)Serial.print(".");                           
    getAccelerometerValues();
    convertAccelerometerValues(scale_acc);
    convertAccelerometerValuesToAngle();                                          
    acc_x_cal += acc_pitch_angle;                                          
    acc_y_cal += acc_roll_angle;                                          
    acc_z_cal += acc_yaw_angle;                                                                                          
  }
  acc_x_cal /= 2000;
  acc_y_cal /= 2000;
  acc_z_cal /= 2000;
}

// This routine updates the values given by convertAccelerometerValuesToAngle(); with the offset obtain with the calibrateAccelerometer(); routine
void applyAccCalibrationValues(){

  acc_pitch_angle -= acc_x_cal;
  acc_roll_angle -= acc_y_cal;
  acc_yaw_angle -= acc_z_cal;
}

// This routine updates the values given by getGyroValues(); with the offset obtain with the calibrateGyro(); routine
void applyGyroCalibrationValues(){

  x_gyro_raw -= gyro_x_cal;
  y_gyro_raw -= gyro_y_cal;
  z_gyro_raw -= gyro_z_cal;
}

// This routine calculates the time that has elapsed since the void loop(); function last executed
void calculateChangeInTime(bool reset){

  finished = millis(); // Time since program started
  elapsed = finished - start; // Time since void loop() started in msec
  dt = elapsed / 1000.0; // Convert to sec
  
  // Reset start and elapsed if condition is true
  if (reset)
  {
    start = elapsed = 0;
  }
}

// This function outputs a digital high to the Arduino pins everytime the angular rate goes above a certain treshold
void LEDFunctions() {

  //*********** X- position LEDs*********// 
  if (x_gyro >= 1000)
  {
   
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }
   else if (x_gyro <= -1000)
   {
     digitalWrite(3, HIGH);
     digitalWrite(2, LOW);
   }
   else
   {
       digitalWrite(2, LOW);
       digitalWrite(3, LOW);
   }
  //*********** Y- position LEDs*********// 
  if (y_gyro >= 1000)
  {
    digitalWrite(4, HIGH);
    digitalWrite(5,LOW);
  }
  else if (y_gyro <= -1000)
  {
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW);
  }
   else
   {
       digitalWrite(4, LOW);
       digitalWrite(5, LOW);
   }
  //*********** Z- position LEDs*********// 
  if (z_gyro >= 1000)
   {
     digitalWrite(6, HIGH);
     digitalWrite(7, LOW);
   }
   else if (z_gyro <= -1000)
   {
     digitalWrite(7, HIGH);
     digitalWrite(6, LOW);
   }
   else
   {
       digitalWrite(7, LOW);
       digitalWrite(6, LOW);
   }
  //************ LED funcitons ENDs****************//
}

// This routine converts the binary values given by the getAccelerometerValues() routine into acceleration values expressed in Gs (factors of 9.8116m/s)
void convertAccelerometerValues(int scale){

  float sensitivity_factor;
  if(scale == 2){
    sensitivity_factor = 0.061; // in mg/LSB from datasheet
  } else if(scale == 4){
    sensitivity_factor = 0.122; // in mg/LSB from datasheet
  } else if(scale == 8){
    sensitivity_factor = 0.244; // in mg/LSB from datasheet
  }
  
  x_acc = (x_acc_raw*sensitivity_factor)/1000;
  y_acc = (y_acc_raw*sensitivity_factor)/1000;
  z_acc = (z_acc_raw*sensitivity_factor)/1000;
}

// This routine converts the acceleration values given by the convertAccelerometerValues() routine into angular displacement for pitch, roll and yaw
void convertAccelerometerValuesToAngle(){

  float R = sqrt(x_acc*x_acc + y_acc*y_acc + z_acc*z_acc); // Calculate the magnitude of the acceleration vector

  // Calculate angle in radians
  acc_roll_angle = asin(y_acc/sqrt(R*R + x_acc*x_acc));
  acc_pitch_angle = acos(x_acc/sqrt(x_acc*x_acc + z_acc*z_acc));
  acc_yaw_angle = acos(x_acc/sqrt(R*R + z_acc*z_acc));

  // onvert radians to degrees
  acc_roll_angle = acc_roll_angle*(180/PI);
  acc_pitch_angle = acc_pitch_angle*(180/PI) - 90;
  acc_yaw_angle = acc_yaw_angle*(180/PI);
}

// This routine converts the binary values given by the getGyroValues() routine into angular rates expressed in deg/sec
void convertGyroValues(int scale){

    float sensitivity_factor;
     if(scale == 250){
      sensitivity_factor = 8.75; // in mdps/LSB from datasheet
    } else if(scale == 500){
      sensitivity_factor = 17.5; // in mdps/LSB from datasheet
    } else if(scale == 2000){
      sensitivity_factor = 70; // in mdps/LSB from datasheet
    }
    
    // Gyro values in deg/s
    x_gyro = (x_gyro_raw*sensitivity_factor)/1000;
    y_gyro = (y_gyro_raw*sensitivity_factor)/1000;
    z_gyro = (z_gyro_raw*sensitivity_factor)/1000;
}

// This routine converts the angular rate values given by the convertGyroValues() routine into angular displacement for pitch, roll and yaw
void convertGyroValuesToAngle(){

  // Gyro dps to deg
  float angle_x_change = x_gyro*dt;
  float angle_y_change = y_gyro*dt;
  float angle_z_change = z_gyro*dt;

  // Uptade old angular position
  gyro_pitch_angle = gyro_pitch_angle - angle_x_change;
  gyro_roll_angle = gyro_roll_angle + angle_y_change;
  gyro_yaw_angle = gyro_yaw_angle + angle_z_change;
}

// This routine reads the angular rate values from the LSM303C accelerometer and stores them in the [axis]_acc_raw variables
void getAccelerometerValues(){

  byte xMSB = readRegister(LSM303C_Address, 0x29);
  byte xLSB = readRegister(LSM303C_Address, 0x28);
  x_acc_raw = ((xMSB << 8) | xLSB);
 
  byte yMSB = readRegister(LSM303C_Address, 0x2B);
  byte yLSB = readRegister(LSM303C_Address, 0x2A);
  y_acc_raw = ((yMSB << 8) | yLSB);
 
  byte zMSB = readRegister(LSM303C_Address, 0x2D);
  byte zLSB = readRegister(LSM303C_Address, 0x2C);
  z_acc_raw = ((zMSB << 8) | zLSB);
}

// This routine reads the angular rate values from the L3GD20 gyroscope and stores them in the [axis]_gyro_raw variables
void getGyroValues(){
 
  byte xMSB = readRegister(L3GD20_Address, 0x29);
  byte xLSB = readRegister(L3GD20_Address, 0x28);
  x_gyro_raw = ((xMSB << 8) | xLSB);
 
  byte yMSB = readRegister(L3GD20_Address, 0x2B);
  byte yLSB = readRegister(L3GD20_Address, 0x2A);
  y_gyro_raw = ((yMSB << 8) | yLSB);
 
  byte zMSB = readRegister(L3GD20_Address, 0x2D);
  byte zLSB = readRegister(L3GD20_Address, 0x2C);
  z_gyro_raw = ((zMSB << 8) | zLSB);
}

// This routine sets up the control registers for the LSM303C accelerometer
int setupLSM303C(int scale){
  
  // Enable x, y, z and turn off power down:
  writeRegister(LSM303C_Address, CTRL_REG1, 0b01000111);

  // HPF configuration
  writeRegister(LSM303C_Address, CTRL_REG2, 0b00000000);

  // Interupts configuration
  writeRegister(LSM303C_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:
 
  if(scale == 2){
    writeRegister(LSM303C_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 4){
    writeRegister(LSM303C_Address, CTRL_REG4, 0b00100000);
  }else{
    writeRegister(LSM303C_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls the self test among other things
  writeRegister(L3GD20_Address, CTRL_REG5, 0b00000000);
}

// This routine sets up the control registers for the L3GD20 gyroscope
int setupL3GD20(int scale){
  
  // Enable x, y, z and turn off power down:
  writeRegister(L3GD20_Address, CTRL_REG1, 0b00001111);
 
  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3GD20_Address, CTRL_REG2, 0b00000000);
 
  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3GD20_Address, CTRL_REG3, 0b00001000);
 
  // CTRL_REG4 controls the full-scale range, among other things:
 
  if(scale == 250){
    writeRegister(L3GD20_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister(L3GD20_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister(L3GD20_Address, CTRL_REG4, 0b00110000);
  }
 
  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3GD20_Address, CTRL_REG5, 0b00000000);
}

// This routine writes to a 1 byte register with I2C protocol 
void writeRegister(int deviceAddress, byte address, byte val) {
    Wire.beginTransmission(deviceAddress); // start transmission to device
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

// This routine reads a 1 byte register with I2C protocol 
int readRegister(int deviceAddress, byte address){
 
    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();
 
    Wire.requestFrom(deviceAddress, 1); // read a byte
 
    while(!Wire.available()) {
        // waiting
    }
    v = Wire.read();
    return v;
}
