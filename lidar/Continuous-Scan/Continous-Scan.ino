// this is an attempt to reverse engineer the code that ran on the
// arduino connected to the lidar for last years team
// all references to other programs are in the robotics-rover repository,
// in branch aa-software2112-patch-1, in the folder robotics-rover/roboticsrov/ucode
void setup () {
    Serial.begin(9600);
}

void loop () {
    int incomingByte = 0;

    // based off lines 15-19 in Radar.py, and scan, write_then_read functions
    if (Serial.available() > 0) {
        incomingByte = Serial.read();

        switch (incomingByte) {
        case 'd':
            Serial.println("returning distance");
        case 'x':
            Serial.println("rotating x servo");
        case 'y':
            Serial.println("rotating y servo");
        case 'r':
            Serial.println("resetting variables");
        case 'c':
            Serial.println("centering servos");
        }
    }
}
