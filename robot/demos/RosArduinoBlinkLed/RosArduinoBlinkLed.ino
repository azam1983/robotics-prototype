#define WAIT_TIME 1000 // in ms
#include "TimerOne.h"

int lowLedPin = 6; // 6th of PORTD
int lowLedInputPin = 1; // 2nd of PORTC
int distanceSensorPin = 4; // 5th of PORTC
int pwmPin = 3; // used as 4th of PORTD (D3) and 4th of 4th of PORTB (D11)
int currentBrightness = 255;
int requestedBrightness = 255;
int readBlinkPeriod = 10000000; // default 1 second
String targetPeriod = "";
String targetBrightness = "";
bool knownCmd = false;

void setupSensor() {
    // the higher the prescaler, the lower the frequency
    ADCSRA |= (1 << ADEN) | (1 << ADPS2);
    ADMUX |= 4 | (1 << REFS0);
}

// use ADCSRA and ADMUX
void readSensorAndPrint() { 
    ADCSRA |= (1 << ADSC); // start conversion
    while(!(ADCSRA & (1 << ADIF)));
    ADCSRA |= (1 << ADIF); // Reset to 1 for the next conversion
    Serial.print("Sensor reading: ");
    Serial.println(ADC);
}

void setBrightnessAndWait(int brightness, int waitTime) {
  Serial.print("Setting brightness to: ");
  Serial.print(brightness);
  Serial.println("/255");
  OCR2A = brightness;
  OCR2B = brightness;
  _delay_ms(waitTime);
}

void setup() {
    Serial.begin(9600);
    DDRD = (1 << lowLedPin | 1 << pwmPin); // set pinmodes to output
    DDRC = (0 << lowLedInputPin); // set pinmode to input
    DDRC = (0 << distanceSensorPin); // set pinmode to input
    PORTC = (1 << lowLedInputPin); // enable output mode and internal pullup resistor

    DDRB = (1 << pwmPin);

    // set duty cycle
    // OCR = Output Compare Register
    OCR2A = 255;
    OCR2B = 255;

    // side note: _BV(6) <--> (1 << 6)
    
    // timer/counter2 control register A
    // set fast PWM mode, non inverting
    TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // WGM --> Wave Form Generation Mode
    // set prescaler to 64 (i.e. divide clock speed of timer 2)
    TCCR2B = (1 << CS22);

    setupSensor();

    // Note this breaks analogWrite() for digital pins 9 and 10 on Nano
    Timer1.initialize(readBlinkPeriod);
    Timer1.attachInterrupt(readSensorAndPrint);
}

void loop() {
    if (PINC & (1 << lowLedInputPin)) {
        //Serial.println("inputPin set to HIGH");
        PORTD &= B10111111;  //(0 << ledPin);
    }
    else {
        //Serial.println("inputPin set to LOW");
        PORTD |= (1 << lowLedPin);
    }

    if (Serial.available()) {
        String cmd = Serial.readString();
        Serial.print("cmd:");
        Serial.println(cmd);

        int pIndex = cmd.indexOf("p:");
        int bIndex = cmd.indexOf("b:");
        int delimIndex = cmd.indexOf(";");

        if (pIndex > -1) {
            // if on left side of semicolon
            if (delimIndex > pIndex) {
                targetPeriod = cmd.substring((pIndex + 2), delimIndex);
            }
            else {
                targetPeriod = cmd.substring(pIndex + 2);
            }

            Serial.print("Target period: ");
            Serial.println(targetPeriod);

            readBlinkPeriod = targetPeriod.toInt();
        }

        if (readBlinkPeriod < 100000 || readBlinkPeriod > 100000000) {
            Serial.println("Read/Blink period input out of range");
            Serial.println("Min: 1000, Max: 100000000 (input in microseconds)");
            Serial.println("i.e Min: 0.1 seconds, Max: 100 seconds");
        }
        else {
            Serial.print("Setting timer period to: ");
            Serial.print(readBlinkPeriod);
            Serial.println(" microseconds");

            // when setting to 100000 (0.1 second)
            // triggering error handling (input: 10000) fails and loop seems to hang
            Timer1.setPeriod(readBlinkPeriod);
        }

        if (bIndex > -1) {
            // on the left side of semicolon
            if (delimIndex > bIndex) {
                targetBrightness = cmd.substring((bIndex + 2), delimIndex);
            }
            else {
                targetBrightness = cmd.substring(bIndex + 2);
            }
    
            Serial.print("Target brightness: ");
            Serial.println(targetBrightness);
    
            if (targetBrightness.toInt() < 0 || targetBrightness.toInt() > 255) {
                Serial.println("Brightness input out of range");
                Serial.println("Min: 0, Max: 255");
            }
            else {
                Serial.print("Setting brightness to: ");
                Serial.println(targetBrightness);
    
                requestedBrightness = targetBrightness.toInt();
            }
        }
    }
    
    setBrightnessAndWait(currentBrightness, WAIT_TIME);
    currentBrightness = (currentBrightness == 0) ? requestedBrightness : 0;
}
