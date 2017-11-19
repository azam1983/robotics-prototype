/*------------------------------------------------------------------------------

  LIDARLite Arduino Library
  ShortRangeHighSpeed

  This example shows a method for running the LIDAR-Lite at high speeds for
  short range operation. It uses an different approach than the default
  LIDAR-Lite library, showing how to read and write to the device using
  Wire directly, as well as various custom settings. See GetDistancePWM and
  GetDistanceI2C for simpler demonstrations of the device.

  Connections:
  LIDAR-Lite 5 Vdc (red) to Arduino 5v
  LIDAR-Lite I2C SCL (green) to Arduino SCL
  LIDAR-Lite I2C SDA (blue) to Arduino SDA
  LIDAR-Lite Ground (black) to Arduino GND

  (Capacitor recommended to mitigate inrush current when device is enabled)
  680uF capacitor (+) to Arduino 5v
  680uF capacitor (-) to Arduino GND

  See the Operation Manual for wiring diagrams and more information:
  http://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf

------------------------------------------------------------------------------*/

#include <Wire.h>
#include "LIDARLite.h"

LIDARLite myLidarLite;

void setup() {
  Serial.begin(115200); // Initialize serial connection to display distance readings

  /*
    begin(int configuration, bool fasti2c, char lidarliteAddress)

    Starts the sensor and I2C.

    Parameters
    ----------------------------------------------------------------------------
    configuration: Default 0. Selects one of several preset configurations.
    fasti2c: Default 100 kHz. I2C base frequency.
      If true I2C frequency is set to 400kHz.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */
  myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz

  /*
    Write
     Perform I2C write to device.
     
     Parameters
    ----------------------------------------------------------------------------
    myAddress: register address to write to.
    myValue: value to write.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */
  myLidarLite.write(0x02, 0x0d); // Maximum acquisition count of 0x0d. (default is 0x80)
  myLidarLite.write(0x04, 0b00000100); // Use non-default reference acquisition count
  myLidarLite.write(0x12, 0x03); // Reference acquisition count of 3 (default is 5)

  Serial.println("After each test, wait for 1s before starting next test.");  // can be changed by modifying delay(1000) at the end of loop() function
  Serial.println("First measurement done with receiver bias correction, all 99 others with out receiver bias correction.\n\n");
}
  
void loop() {
  // for testing the elapsed times for taking measurements
  long measureCount = 100;
  long measureTimes[measureCount]; 
  // Take a measurement with receiver bias correction and print to serial terminal
//   Serial.println(distanceFast(true));

  long startTest = micros();
  long startTime = micros(); // returns number of ms since arduino starts program loop, overflows after ~70min
  distanceFast(true); // no need to store value for the purpose of this test
  long endTime = micros();

  
  measureTimes[0] = endTime - startTime; // in micro seconds
    
  // Take 99 measurements without receiver bias correction and print to serial terminal
  for (int i = 1; i < measureCount; i++) {
//    Serial.println(distanceFast(false));
      startTime = micros();
      distanceFast(false);
      endTime = micros();
      measureTimes[i] = endTime - startTime;
  }
  long endTest = micros();
  long testDuration = endTest - startTest;

  // find max, min, avg
  long sum = 0;
  double max = measureTimes[0];
  double min = max;

  for (int i = 0; i < measureCount; i++) {
    sum += measureTimes[i];

    if (measureTimes[i] > max) {
      max = measureTimes[i];
    }

    if (measureTimes[i] < min) {
      min = measureTimes[i];
    }
  }

  double avg = (sum/measureCount);
  double duration = endTest - startTest;
  
  String countStr = "Measurement times for last ";
  countStr.concat(measureCount);
  countStr += " measurements.";
  String minStr = "min = ";
  minStr.concat(min);
  minStr += " micro-seconds, ";
  min = min/1000.;
  minStr.concat(min);
  minStr += " milli-seconds";
  String maxStr = "max = ";
  maxStr.concat(max);
  maxStr += " micro-seconds, ";
  max = max/1000.;
  maxStr.concat(max);
  maxStr += " milli-seconds";
  String avgStr = "avg = ";
  avgStr.concat(avg);
  avg = avg/1000.;
  avgStr += " micro-seconds, ";
  avgStr.concat(avg);
  avgStr += " milli-seconds";
  String durationStr = "Done in ";
  durationStr.concat(duration);
  durationStr += " micro-seconds, ";
  duration = duration/1000.;
  durationStr.concat(duration);
  durationStr += " milli-seconds, ";
  duration = duration/1000.;
  durationStr.concat(duration);
  durationStr += " seconds";
  String line = "-------------------------------------------------------------\n";

  Serial.println(countStr);
  Serial.println(minStr);
  Serial.println(maxStr);
  Serial.println(avgStr);
  Serial.println(durationStr);
  Serial.println(line);
  
  
  delay(1000);
}

// Read distance. The approach is to poll the status register until the device goes
// idle after finishing a measurement, send a new measurement command, then read the
// previous distance data while it is performing the new command.
int distanceFast(bool biasCorrection)
{
  byte isBusy = 1;
  int distance;
  int loopCount;

  // Poll busy bit in status register until device is idle
  while(isBusy)
  {
    // Read status register
    Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
    Wire.write(0x01);
    Wire.endTransmission();
    Wire.requestFrom(LIDARLITE_ADDR_DEFAULT, 1);
    isBusy = Wire.read();
    isBusy = bitRead(isBusy,0); // Take LSB of status register, busy bit

    loopCount++; // Increment loop counter
    // Stop status register polling if stuck in loop
    if(loopCount > 9999)
    {
      break;
    }
  }

  // Send measurement command
  Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
  Wire.write(0X00); // Prepare write to register 0x00
  if(biasCorrection == true)
  {
    Wire.write(0X04); // Perform measurement with receiver bias correction
  }
  else
  {
    Wire.write(0X03); // Perform measurement without receiver bias correction
  }
  Wire.endTransmission();

  // Immediately read previous distance measurement data. This is valid until the next measurement finishes.
  // The I2C transaction finishes before new distance measurement data is acquired.
  // Prepare 2 byte read from registers 0x0f and 0x10
  Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
  Wire.write(0x8f);
  Wire.endTransmission();

  // Perform the read and repack the 2 bytes into 16-bit word
  Wire.requestFrom(LIDARLITE_ADDR_DEFAULT, 2);
  distance = Wire.read();
  distance <<= 8;
  distance |= Wire.read();

  // Return the measured distance
  return distance;
}
