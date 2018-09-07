#ifndef PINSETUP_H
#define PINSETUP_H

// pin constants defined in core_pins.h, pins_teensy.c, pins_arduino.h

/*
   m1 base stepper (rotation)
   m2 shoulder dc (flexion)
   m3 elbow stepper (flexion)
   m4 wrist stepper (flexion
   m5 wrist servo (rotation)
   m6 end effector servo (pinching)
*/

// 3 pwm pins
// 4-6 limit switch interrupt pins
// 8-14 encoder interrupt pins
// 3 direction pins, 3 step pins

enum motor_code {MOTOR1 = 1, MOTOR2, MOTOR3, MOTOR4, MOTOR5, MOTOR6}; // defines 6 motors
enum motor_direction {CLOCKWISE, COUNTER_CLOCKWISE}; // defines motor directions // can go into ArmMotor
enum motor_speed {SPEED0, SPEED1, SPEED2, SPEED3}; // defines motor speed // can go into ArmMotor

const int dir [16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; //quadrature encoder matrix. Corresponds to the correct direction for a specific set of prev and current encoder states

#define NUM_LIM_SW          4

// can go into ArmMotor
// limits in ms for amount of time the motor can budge
#define MAX_BUDGE_TIME 3000
#define MIN_BUDGE_TIME 100
#define DEFAULT_BUDGE_TIME 500
#define DEFAULT_SPEED 0
#define MAX_SPEED 3
#define MAX_COUNTS 3

// can go into ArmMotor
#define SERVO_STOP 189 // for 3.3v
//#define SERVO_STOP 127 // for 5v
#define DC_STOP 189//194 // for 3.3v
//#define DC_STOP 127 // for 5v

// steppers

#define M1_ENABLE_PIN       2
#define M1_DIR_PIN          5
#define M1_STEP_PIN         6
// 7&8 are on port D with bits 2&3 respectively
#define M1_ENCODER_PORT     GPIOD_PDIR
#define M1_ENCODER_A        7
#define M1_ENCODER_B        8
#define M1_LIMIT_SW_CW      9
#define M1_LIMIT_SW_CCW    10

#define M3_ENABLE_PIN      17
#define M3_DIR_PIN         20
#define M3_STEP_PIN        21
// 18&19 are on port B with bits 2&3 respectively
#define M3_ENCODER_PORT    GPIOB_PDIR
#define M3_ENCODER_A       18
#define M3_ENCODER_B       19
#define M3_LIMIT_SW_FLEX   22
#define M3_LIMIT_SW_EXTEND 23

#define M4_ENABLE_PIN      24
#define M4_DIR_PIN         25
#define M4_STEP_PIN        14
// 11&12 are on port C with bits 6&7 respectively
#define M4_ENCODER_PORT    GPIOC_PDIR
#define M4_ENCODER_A       11
#define M4_ENCODER_B       12
#define M4_LIMIT_SW_FLEX   15
#define M4_LIMIT_SW_EXTEND 16

// DC motor

#define M2_PWM_PIN         30
#define M2_DIR_PIN         31 // for new driver
//#define M2_UART_TX       CORE_TXD4_PIN  // 32 for sabertooth
//#define M2_UART_RX       CORE_RXD4_PIN  // 31 for sabertooth
// 26&27 are on port A with bits 14&15 respectively
#define M2_ENCODER_PORT    GPIOA_PDIR
#define M2_ENCODER_SHIFT    CORE_PIN26_BIT
#define M2_ENCODER_A       26
#define M2_ENCODER_B       27
#define M2_LIMIT_SW_FLEX   28
#define M2_LIMIT_SW_EXTEND 29

// servos

#define M5_PWM_PIN         35
// 33&34 are on port E with bits 24&25 respectively
#define M5_ENCODER_PORT    GPIOE_PDIR
#define M5_ENCODER_A       33
#define M5_ENCODER_B       34

#define M6_PWM_PIN         36
// 37&38 are on port C with bits 10&11 respectively
#define M6_ENCODER_PORT    GPIOC_PDIR
#define M6_ENCODER_A       37
#define M6_ENCODER_B       38

void pinSetup(void);

#endif