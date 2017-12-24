## lidar
`Continous-Scan.ino` is an attempt to reverse-engineer the missing Arduino sketch that was working with last year's team's lidar code.
You can run `sudo python ArduinoSpeaker.py` in the `init-arm-comms` branch in the `communications` 
while running `sudo python ArduinoListener.py` to observe a test of the interaction.

## How to connect lidar module to odroid (no servos involved)

- Connect power wire (red wire sticking out of capacitor) to pin 2
- Connect ground wire (black wire sticking out of capacitor) to pin 6
- Connect I2C wires (Lidar I2C SCA from [lidar-board-interface-thingy.png](https://github.com/space-concordia-robotics/robotics-prototype/blob/34efe5dc4cd1a26d1a3c19e78ccac35cd4398f59/lidar/images/lidar-board-interface-thingy.png)). Grey wire (SDA) to pin 3 and green wire (SCL) to pin 5.



