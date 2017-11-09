// this is an attempt to reverse engineer the code that ran on the
// arduino connected to the lidar for last years team
// all references to other programs are in the robotics-rover repository,
// in branch aa-software2112-patch-1, in the folder robotics-rover/roboticsrov/ucode
void setup () {
    Serial.begin(9600);

    boolean readyReceived = false;

    // based of arduino_ready function in ArduinoControlRev2.py
    // which seems to wait for an "r" from the arduino to start.
    while (!readyReceived) {
        Serial.println("r");

        //TODO: if handshake received, set readyReceived = true
    }
}

void loop () {
    // do the thing
}
