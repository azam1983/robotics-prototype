// 3 things need to be synched up between the arduino code and the GUI code: serial read/write frequency (1s for now), baudrate, and agreed upon format of serial output
void setup() {
  Serial.begin(9600);
  // format of motor table output will be 6 comma separated values of status, angle, current, each respectively speparated via semicolon
  // e.g "-,-,-,-,-,-;0,0,0,0,0,0;-,-,-,-,-,-"
}

void loop() {
  String str = "";
  //String statusi[] = {"unknown", "unknown", "unknown", "unknown", "unknown", "unknown"};
  //double angles[] = {0, 0, 0, 0, 0, 0};
  //double currents[] = {0, 0, 0, 0, 0, 0};
  long randomNumber;
  
  for (int i = 0; i < 18; i++) {
    if (i < 6) {
      str.concat("unknown");
    } else if (i < 12) {
      randomNumber = random(180);
      str.concat(randomNumber);
    } else {
      randomNumber = random(13);
      str.concat(randomNumber);
    }

    if (i == 5 || i == 11) {
      str.concat(";");
    } else if (i != 17) {
      str.concat(","); // to separate collumns
    }
  }
  
  Serial.println(str);
  delay(1000);
}
