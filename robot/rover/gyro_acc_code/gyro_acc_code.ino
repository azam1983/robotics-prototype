//Notes
/*
- Integrate LSB and MSB as a 6 byte array. This will allow to get data
  from the sensors ASAP and thus dt will be more accurate. Currently 
  since we do xyz one by one, the dt may be inaccurate for the z compo-
  nent.

- (1) Accelerometers are better when the object is rotating on Earth and it is not moving forwards.
      However, once it stars moving, the gyroscope is more precise.
  (2) Accelerometers dont give rotation in horizontal (xy) plane. Gyroscopes do.
  
- Turn the sensor by 180 degrees (bottom up) to operate accelerometer - its more precise.

- If LSB and MSB array is integrated, review the code for calibration.
*/

//Libraries.
/*******************************************************/
#include <Wire.h>
/*******************************************************/

//General variables.
/********************************************************/
//Register addresses.
byte const DEVICE[3] = {0x6B,0x1D,0X77};//Gyroscope (L3GD20), accelerometer (LSM303C), environmental sensor ().
//Time variables.
unsigned long past = 0;//Time at the start of previous loop.
float dt = 0;//Time difference between current and previous loop.
//General data variables.
byte LSB = 0;//Storage for byte reading.
byte MSB = 0;//Storage for byte reading.
int temp_int = 0;//Integer variable to store intermediate data from sensor.
float temp_float = 0.0;//Float variable to store intermediate data from sensor.
//Weighted average variables.
float angle[3] = {0,0,0};
float gyro_weight = 0.2;
float acc_weight = 0.8;
/*********************************************************/

//Gyroscope (L3GD20).
/*********************************************************/
byte const GYRO_CTRL_REG[5] = {0x20,0x21,0x22,0x23,0x24};//Gyroscope setup registers.
int const GYRO_SCALE = 250;//Options: 250 deg/s, 500 deg/s, 2000 deg/s.
float const GYRO_SENSITIVITY = GYRO_SCALE/28.57142857;//Gyroscope sensitivity factor is found in the datasheet.
float gyro_angle[3] = {0,0,0};//Array to store pitch, roll and yaw angles.
float gyro_cal[3] = {0,0,0};//Array to store calibration data.
/*********************************************************/

//Accelerometer (LSM303C).
/*********************************************************/
byte const ACC_CTRL_REG[5] = {0x20,0x21,0x22,0x23,0x24};//Accelerometer setup registers.
int const ACC_SCALE = 2;//Options: 2g, 4g, 8g.
float const ACC_SENSITIVITY = ACC_SCALE*0.0305;//Accelerometer sensitivity factor is found in the datasheet.
float acc_temp [3] = {0,0,0};//Array to store raw data for angle calculation.
float acc_angle[3] = {0,0,0};//Array to store pitch, roll and yaw angles.
float acc_cal[3] = {0,0,0};//Array to store calibration data.
/*********************************************************/

void setup()
{
  //Starting serial communication.
  Wire.begin();
  Serial.begin(9600);

  //Configuring L3GD20 gyroscope.
  Serial.println("Setting up the gyroscope");
  if(setupGyro() == 0)
  {
    Serial.println("Setup OK");  
  }
  else
  {
    Serial.println("Setup FAILED");
  }
  
  //Configuring LSM303C accelerometer.
  Serial.println("Setting up the accelerometer");
  if(setupAcc() == 0)
  {
    Serial.println("Setup OK");  
  }
  else
  {
    Serial.println("Setup FAILED");
  }
}

void loop()
{
    dt = millis()/1000 - past;//Time since previous loop start.
    past = millis()/1000;//Recording current loop start time.

    //Gyroscope code.
    LSB = readRegister(DEVICE[0], 0x28);
    MSB = readRegister(DEVICE[0], 0x29);
    temp_int = word(MSB,LSB);
    temp_float = float(temp_int);
    temp_float = (((temp_float - gyro_cal[0])*GYRO_SENSITIVITY)/1000)*dt;
    gyro_angle[0] = gyro_angle[0] - temp_float;

    LSB = readRegister(DEVICE[0], 0x2A);
    MSB = readRegister(DEVICE[0], 0x2B);
    temp_int = word(MSB,LSB);
    temp_float = float(temp_int);
    temp_float = (((temp_float - gyro_cal[1])*GYRO_SENSITIVITY)/1000)*dt;
    gyro_angle[1] = gyro_angle[1] + temp_float;

    LSB = readRegister(DEVICE[0], 0x2C);
    MSB = readRegister(DEVICE[0], 0x2D);
    temp_int = word(MSB,LSB);
    temp_float = float(temp_int);
    temp_float = (((temp_float - gyro_cal[2])*GYRO_SENSITIVITY)/1000)*dt;
    gyro_angle[2] = gyro_angle[2] + temp_float;

    //Accelerometer code.
    LSB = readRegister(DEVICE[1], 0x28);
    MSB = readRegister(DEVICE[1], 0x29);
    temp_int = word(MSB,LSB);
    acc_temp[0] = float(temp_int);
    acc_temp[0] = ((acc_temp[0]-acc_cal[0])*ACC_SENSITIVITY)/1000;//Data is in G/s!

    LSB = readRegister(DEVICE[1], 0x2A);
    MSB = readRegister(DEVICE[1], 0x2B);
    temp_int = word(MSB,LSB);
    acc_temp[1] = float(temp_int);
    acc_temp[1] = ((acc_temp[1]-acc_cal[1])*ACC_SENSITIVITY)/1000;//Data is in G/s!

    LSB = readRegister(DEVICE[1], 0x2C);
    MSB = readRegister(DEVICE[1], 0x2D);
    temp_int = word(MSB,LSB);
    acc_temp[2] = float(temp_int);
    acc_temp[2] = ((acc_temp[2]-acc_cal[2])*ACC_SENSITIVITY)/1000;//Data is in G/s!

    //Convert acceleration to angles (radian) from axes.
    acc_angle[0] = atan(acc_temp[0]/sqrt(acc_temp[1]*acc_temp[1]+acc_temp[2]*acc_temp[2]));
    acc_angle[1] = atan(acc_temp[1]/sqrt(acc_temp[0]*acc_temp[0]+acc_temp[2]*acc_temp[2]));
    acc_angle[2] = atan(sqrt(acc_temp[0]*acc_temp[0]+acc_temp[1]*acc_temp[1])/acc_temp[2]);

    //Convert radians to degrees.
    acc_angle[0] = acc_angle[0]*(180/PI)*2;
    acc_angle[1] = acc_angle[1]*(180/PI)*2;
    acc_angle[2] = acc_angle[2]*(180/PI)*2;


//    Serial.print(gyro_angle[0]);
//    Serial.print(" , ");
//    Serial.print(gyro_angle[1]);
//    Serial.print(" , ");
//    Serial.print(gyro_angle[2]);
//    Serial.println();

    Serial.print(acc_angle[0]);
    Serial.print(" , ");
    Serial.print(acc_angle[1]);
    Serial.print(" , ");
    Serial.print(acc_angle[2]);
    Serial.println();
    
    delay(50);
}

//Function that reads a specified register of a specified device.
int readRegister(int deviceAddress, byte address)
{ 
    int result;//Stuff that was read off the specified register.
    int current;//Time before waiting for data.
    int diff;//Time waiting for data.
    Wire.beginTransmission(deviceAddress);//Serial transmission start signal.
    Wire.write(address);//Register to use.
    Wire.endTransmission(); //Serial transmission stop signal.
    Wire.requestFrom(deviceAddress, 1);//Request 1 byte from specified device.
    current = millis();//Current time.
    while(!Wire.available())
    {
        diff = millis()-current;//Making sure we dont wait for too long.
        if(diff > 1000)
          {
            break;//Wait too long, dont want to wait forever.
          }
    }
    result = Wire.read();//Read the wire.
    return result;
}

//Function that writes a byte to a specified registed with a specified address.
void writeRegister(int deviceAddress, byte address, byte val)
{
  Wire.beginTransmission(deviceAddress);//Serial transmission start signal.
  Wire.write(address);//Register to use.
  Wire.write(val);//Write value to register specified previously.
  Wire.endTransmission();//Serial transmission stop signal.
}

//Function that configures gyroscope (L3GD20) options and calibrates its offset value.
int setupGyro()
{
  //Writing options to option registers. For meaning, check the datasheet of the device.
  writeRegister(DEVICE[0], GYRO_CTRL_REG[0], 0b00001111);//Enable x,y,z and turn off the "power down".
  writeRegister(DEVICE[0], GYRO_CTRL_REG[1], 0b00000000);//HPF configuration.
  writeRegister(DEVICE[0], GYRO_CTRL_REG[2], 0b00001000);//Interrupts configuration.
  if(GYRO_SCALE == 250)
  {
    writeRegister(DEVICE[0], GYRO_CTRL_REG[3], 0b00000000);//Scale configuration (250 deg/s).
  }
  else if(GYRO_SCALE == 500)
  {
    writeRegister(DEVICE[0], GYRO_CTRL_REG[3], 0b00010000);//Scale configuration (500 deg/s).
  }
  else
  {
    writeRegister(DEVICE[0], GYRO_CTRL_REG[3], 0b00110000);//Scale configuration (2000 deg/s).
  }
  writeRegister(DEVICE[0], GYRO_CTRL_REG[4], 0b00000000);//Filtering configuration.

  //Calibrating the gyroscope offset value (to set the initial frame of reference).
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++)
  {                  
    if(cal_int % 125 == 0)Serial.print(".");
                             
    MSB = readRegister(DEVICE[0], 0x29);
    LSB = readRegister(DEVICE[0], 0x28);
    temp_int = word(MSB,LSB);
    gyro_cal[0] += temp_int;
    
    MSB = readRegister(DEVICE[0], 0x2B);
    LSB = readRegister(DEVICE[0], 0x2A);
    temp_int = word(MSB,LSB);
    gyro_cal[1] += temp_int;

    MSB = readRegister(DEVICE[0], 0x2D);
    LSB = readRegister(DEVICE[0], 0x2C);
    temp_int = word(MSB,LSB);
    gyro_cal[2] += temp_int;                                                                                                                                                  
  }
  //Average of 2000 readings at rest.
  gyro_cal[0] /= 2000;                                                  
  gyro_cal[1] /= 2000;                                                  
  gyro_cal[2] /= 2000;
  return 0;//Return check value.
}

//Function that configures accelerometer (LSM303C) options and calibrates its offset value.
int setupAcc()
{
  //Writing options to option registers. For meaning, check the datasheet of the device.
  writeRegister(DEVICE[1], ACC_CTRL_REG[0], 0b01000111);//Enable x,y,z and turn off the "power down".
  writeRegister(DEVICE[1], ACC_CTRL_REG[1], 0b00000000);//HPF configuration.
  writeRegister(DEVICE[1], ACC_CTRL_REG[2], 0b00001000);//Interrupts configuration.
  if(ACC_SCALE == 2)
  {
    writeRegister(DEVICE[1], ACC_CTRL_REG[3], 0b00000000);//Scale configuration (2g).
  }
  else if(ACC_SCALE == 4)
  {
    writeRegister(DEVICE[1], ACC_CTRL_REG[3], 0b00100000);//Scale configuration (4g).
  }
  else
  {
    writeRegister(DEVICE[1], ACC_CTRL_REG[3], 0b00110000);//Scale configuration (8g).
  }
  writeRegister(DEVICE[1], ACC_CTRL_REG[4], 0b00000000);//Self test.

  //Calibrating the accelerometer offset value (to set the initial frame of reference).
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++)
  {                  
    if(cal_int % 125 == 0)Serial.print(".");
                             
    MSB = readRegister(DEVICE[1], 0x29);
    LSB = readRegister(DEVICE[1], 0x28);
    temp_int = word(MSB,LSB);
    acc_cal[0] += temp_int;
    
    MSB = readRegister(DEVICE[1], 0x2B);
    LSB = readRegister(DEVICE[1], 0x2A);
    temp_int = word(MSB,LSB);
    acc_cal[1] += temp_int;

    MSB = readRegister(DEVICE[1], 0x2D);
    LSB = readRegister(DEVICE[1], 0x2C);
    temp_int = word(MSB,LSB);
    acc_cal[2] += temp_int;                                                                                                                                               
  }
  //Average of 2000 readings at rest.
  acc_cal[0] /= 2000;                                                  
  acc_cal[1] /= 2000;                                                  
  acc_cal[2] /= 2000;
  return 0;//Return check value.
}

