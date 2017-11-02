/*------------------------------------------------------------------------------

  LIDARLite Arduino Library
  GetDistanceI2c

  This example shows how to initialize, configure, and read distance from a
  LIDAR-Lite connected over the I2C interface.

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

void setup()
{
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
    configure(int configuration, char lidarliteAddress)

    Selects one of several preset configurations.

    Parameters
    ----------------------------------------------------------------------------
    configuration:  Default 0.
      0: Default mode, balanced performance.
      1: Short range, high speed. Uses 0x1d maximum acquisition count.
      2: Default range, higher speed short range. Turns on quick termination
          detection for faster measurements at short range (with decreased
          accuracy)
      3: Maximum range. Uses 0xff maximum acquisition count.
      4: High sensitivity detection. Overrides default valid measurement detection
          algorithm, and uses a threshold value for high sensitivity and noise.
      5: Low sensitivity detection. Overrides default valid measurement detection
          algorithm, and uses a threshold value for low sensitivity and noise.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */
  myLidarLite.configure(0); // Change this number to try out alternate configurations

  Serial.println("After each test, wait for 1s before starting next test."); // can be changed by modifying delay(1000) at the end of loop() function
  Serial.println("First measurement done with receiver bias correction, all other 99 with out receiver bias correction.\n\n");
}

void loop()
{
  /*
    distance(bool biasCorrection, char lidarliteAddress)

    Take a distance measurement and read the result.

    Parameters
    ----------------------------------------------------------------------------
    biasCorrection: Default true. Take aquisition with receiver bias
      correction. If set to false measurements will be faster. Receiver bias
      correction must be performed periodically. (e.g. 1 out of every 100
      readings).
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */


  long measureCount = 100;
  long measureTimes[measureCount];
  long startTest = micros();
  long startTime = micros(); // returns number of ms since arduino starts program loop, overflows after ~70min
  myLidarLite.distance();
  long endTime = micros();
  
  measureTimes[0] = endTime - startTime; // in micro seconds
  
  myLidarLite.distance();
  
  
  // Take 99 measurements without receiver bias correction and print to serial terminal
  for (int i = 1; i < measureCount; i++) {
    startTime = micros();
    myLidarLite.distance();
    endTime = micros();
    measureTimes[i] = endTime - startTime;
  }
  long endTest = micros();

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
