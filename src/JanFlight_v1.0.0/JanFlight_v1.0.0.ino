/*
Author: Dikshit Makwana
Project Start: 19/05/2026
Last Updated: 06/06/2026
Licence: GPL-3.0
Version: v1.0.0
*/

//========================================================================================================================//
//                                                    CREDIT & THANKS                                                     //
//========================================================================================================================//

/*
Inspired by:
https://github.com/nickrehm/dRehmFlight

Mahony ahrs filter adapted from:
https://ahrs.readthedocs.io/en/latest/filters/mahony.html
*/

//========================================================================================================================//
//                                              1. USER SPECIFIED DEFINES                                                 //
//========================================================================================================================//

// Choose receiver protocol (Uncomment only one)
#define USE_PPM_RX // Default
//#define USE_PWM_RX //Implementation Pending
//#define USE_IBUS_RX //Implementation Pending

// Choose IMU
#define USE_MPU6500_I2C 

// Choose full scale gyro range (deg/sec)
#define GYRO_250DPS // Default
//#define GYRO_500DPS
//#define GYRO_1000DPS
//#define GYRO_2000DPS

// Choose full scale accelerometer range (G's)
#define ACCEL_2G  // Default
//#define ACCEL_4G
//#define ACCEL_8G
//#define ACCEL_16G

// Channel Mapping (Default to FSi6 Mode 2)
#define CHAN_THROTTLE  channel_3_pwm  // CH3 = Left stick up/down
#define CHAN_ROLL      channel_1_pwm  // CH1 = Right stick left/right
#define CHAN_PITCH     channel_2_pwm  // CH2 = Right stick up/down
#define CHAN_YAW       channel_4_pwm  // CH4 = Left stick left/right
#define CHAN_KILL      channel_5_pwm  // CH5 = SwC
#define CHAN_ARM       channel_6_pwm  // CH6 = SwB

//========================================================================================================================//
//                                                  2. REQUIRED LIBRARIES                                                 //
//========================================================================================================================//

#include <Wire.h>  //I2c communication
#include <Servo.h> 

//========================================================================================================================//
//                                         3. USER-SPECIFIED VARIABLE DECLARATION                                         //
//========================================================================================================================//

// Failsafe values (If the radio signal is lost, the code defaults to these values)
unsigned long channel_1_fs = 1500; // Roll
unsigned long channel_2_fs = 1500; // Pitch
unsigned long channel_3_fs = 1000; // Throttle
unsigned long channel_4_fs = 1500; // Yaw
unsigned long channel_5_fs = 2000; // SwC
unsigned long channel_6_fs = 1000; // SwB

// IMU Filter Parameters (Tuned for 1.2kHz)

// Takes value between 0 and 1. A value of 0.10 means, 10% value from current loop & 90% from previous loop is applied to IMU Variables.
float B_accel = 0.14; // Accel Low-pass filter 
float B_gyro = 0.10;  // Gyro Low-pass filter

float twoKp = 2.0f * 0.5f; // Mahony proportional gain; dictates how heavily the accelerometer is trusted to correct gyroscope drift over time
float twoKi = 2.0f * 0.0f; // Mahony integral gain; handles long-term steady-state errors

// Controller Limits; Max tilt angles in degrees. (Basically When the radio stick is pushed to 100%, the drone will pitch/roll exactly this many degrees.)
float i_limit = 25.0;  // Maximum integrator error buildup
float maxRoll = 30.0;  // Angle mode max roll (degrees)
float maxPitch = 30.0; // Angle mode max pitch (degrees)
float maxYaw = 120.0;  // Yaw max rate (deg/sec)

// PID Gains (Kp is the immediate reaction to an error. Ki builds up over time to fix continuous sagging (like uneven weight). Kd dampens the reaction to stop oscillations.)
float Kp_roll_angle = 0.2, Ki_roll_angle = 0.3, Kd_roll_angle = 0.05;
float Kp_pitch_angle = 0.2, Ki_pitch_angle = 0.3, Kd_pitch_angle = 0.05;
float Kp_yaw = 0.3, Ki_yaw = 0.05, Kd_yaw = 0.00015;

// IMU Error Offsets (Run calculate_IMU_error() once to get these, then hardcode)
float AccErrorX = -0.22, AccErrorY = 0.26, AccErrorZ = 0.03;
float GyroErrorX = -0.07, GyroErrorY = 4.85, GyroErrorZ = 1.41;

// Data Logging frequency for troubleshooting (Higher the value, higher the stress on your MCU)
const int data_print_rate = 5;

//========================================================================================================================//
//                                                   4. PIN DECLARATION                                                   //
//========================================================================================================================//

// Based on STM32F405 Nomenclature (WeAct Studio Dev Board)

// Receiver Pins
const int PPM_Pin = PA8; // PPM Input

// LED Pin
const int ledPin = PB2; // The Blue LED

// ESC Pins (OneShot125)
const int m1Pin = PA0;
const int m2Pin = PA2;
const int m3Pin = PA6;
const int m4Pin = PA3;

// Servo Pins (Standard PWM)
const int servo1Pin = PB0;
const int servo2Pin = PB1;

Servo servo1;
Servo servo2;

//========================================================================================================================//
//                                               5. GLOBAL VARIABLE DECLARATION                                           //
//========================================================================================================================//

// Timing
float dt;
unsigned long current_time, prev_time;
unsigned long print_counter;

// Radio Data
unsigned long channel_1_pwm, channel_2_pwm, channel_3_pwm, channel_4_pwm, channel_5_pwm, channel_6_pwm;
unsigned long channel_1_raw, channel_2_raw, channel_3_raw, channel_4_raw, channel_5_raw, channel_6_raw; // Background hardware routines write to these variables continously. The main loop safely copies and filters them into respective channel_x_pwm variable.
unsigned long channel_1_pwm_prev, channel_2_pwm_prev, channel_3_pwm_prev, channel_4_pwm_prev;
int ppm_counter = 0;
unsigned long time_ms = 0;

// Flight State
bool armedFly = false;

// IMU & Mahony Data
float AccX, AccY, AccZ, AccX_prev, AccY_prev, AccZ_prev;
float GyroX, GyroY, GyroZ, GyroX_prev, GyroY_prev, GyroZ_prev;
float roll_IMU, pitch_IMU, yaw_IMU;
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f; // Quaternion for Mahony Filter
float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f; // Integral error terms

// Desired State & PID
float thro_des, roll_des, pitch_des, yaw_des;
float error_roll, integral_roll, integral_roll_prev, derivative_roll, roll_PID = 0;
float error_pitch, integral_pitch, integral_pitch_prev, derivative_pitch, pitch_PID = 0;
float error_yaw, error_yaw_prev, integral_yaw, integral_yaw_prev, derivative_yaw, yaw_PID = 0;

// Command pulses

//ESCs
float m1_command_scaled, m2_command_scaled, m3_command_scaled, m4_command_scaled;
int m1_command_PWM, m2_command_PWM, m3_command_PWM, m4_command_PWM;

//SERVOs
float s1_command_scaled, s2_command_scaled;
int s1_command_PWM, s2_command_PWM;

// Scale Factors for MPU6500
#if defined GYRO_250DPS
  #define GYRO_SCALE_FACTOR 131.0
#elif defined GYRO_500DPS
  #define GYRO_SCALE_FACTOR 65.5
#elif defined GYRO_1000DPS
  #define GYRO_SCALE_FACTOR 32.8
#elif defined GYRO_2000DPS
  #define GYRO_SCALE_FACTOR 16.4
#endif

#if defined ACCEL_2G
  #define ACCEL_SCALE_FACTOR 16384.0
#elif defined ACCEL_4G
  #define ACCEL_SCALE_FACTOR 8192.0
#elif defined ACCEL_8G
  #define ACCEL_SCALE_FACTOR 4096.0
#elif defined ACCEL_8G
  #define ACCEL_SCALE_FACTOR 2048.0
#endif

//========================================================================================================================//
//                                                       6. VOID SETUP                                                    //
//========================================================================================================================//

// pre-flight checklist
void setup() {
  // Establish serial connection to onboard USB
  Serial.begin(500000);
  while (!Serial) {
    delay(10);
  }
  delay(2000);

  //Indicate entering setup loop with 3 blinks
  pinMode(ledPin, OUTPUT);
  Serial.println("--- Boot Begins ---");
  ledBlink(3,250,250);
  
  // Initialize all pins
  Serial.print("Initializing Pins... ");
  pinMode(m1Pin, OUTPUT);
  pinMode(m2Pin, OUTPUT);
  pinMode(m3Pin, OUTPUT);
  pinMode(m4Pin, OUTPUT);
  servo1.attach(servo1Pin, 900, 2100);
  servo2.attach(servo2Pin, 900, 2100);
  Serial.println("Done.");

  // Initialize communication with desired radio type
  Serial.print("Configuring Radio... ");
  radioSetup();
  Serial.println("Done.");

  // Failsafe setup: Default to safe values before main loop starts
  channel_1_pwm = channel_1_fs;
  channel_2_pwm = channel_2_fs;
  channel_3_pwm = channel_3_fs;
  channel_4_pwm = channel_4_fs;
  channel_5_pwm = channel_5_fs;
  channel_6_pwm = channel_6_fs;

  // Initialize IMU
  Serial.print("Initializing IMU... ");
  IMUinit();
  Serial.println("Done.");
  delay(50);

  // Get IMU error offset (Uncomment to calibrate, then write them into section 3 under IMU Error Offsets and re-comment) (Keep IMU on Flat Surface during Calibration)
  //calculate_IMU_error(); 

  // Arm servo/PWM channels (write low)
  Serial.print("Arming Servos... ");
  servo1.write(90); 
  servo2.write(90);
  delay(5);
  Serial.println("Done.");

  // Arm OneShot125 channels (Write lowest valid pulse: 125us)
  Serial.print("Arming ESCs... ");
  m1_command_PWM = 125;
  m2_command_PWM = 125;
  m3_command_PWM = 125;
  m4_command_PWM = 125;
  for (int i = 0; i < 50; i++) {
    commandMotors();
    delay(2);
  }
  Serial.println("Done.");

  // Warms up IMU filter before entering main loop
  Serial.print("Calibrating Attitude... ");
  calibrateAttitude();
  Serial.println("Done.");

  //Indicate entering main loop with 2 quick blinks
  Serial.print("--- Entering Main Loop ---");
  ledBlink(2,150,150);
}

//========================================================================================================================//
//                                                        7. VOID LOOP                                                    //
//========================================================================================================================//

// MCU runs these functions on repeat at 1.2 kHZ frequency
void loop() {
  // Track timing
  prev_time = current_time;
  current_time = micros();      
  dt = (current_time - prev_time) / 1000000.0;

  // Print Data at 5hz (Uncomment one for troubleshooting)
  // printRadioData();
  // printDesiredState();
  // printRollPitchYaw();
  // printMotorCommands();
  // printServoCommands();
  // printLoopRate();

  // Pulls raw gyro/accel data and applies low-pass filter
  getIMUdata();

  // Uses IMU data to estimate absolute vehicle attitude
  Mahony(GyroX, -GyroY, -GyroZ, -AccX, AccY, AccZ, dt);

  // Convert raw radio commands to normalized bounds based on user limits
  getDesState();

  // PID control generating stabilized axis variables
  controlANGLE();

  // Mixes PID controller outputs to scaled actuator commands
  controlMixer();

  // Scale motor/actuator commands to required ranges (OneShot125 & Servo)
  scaleCommands();

  // Set motor commands to minimum if kill switch is engaged
  throttleCut();

  // Send exact pulse widths to ESCs and Servos
  commandMotors();
  servo1.write(s1_command_PWM);
  servo2.write(s2_command_PWM);

  // Get current radio commands to be used in next loop iteration
  getCommands();

  // Check validity of radio commands and overwrite with failsafe if bad
  failSafe();

  // Idle until 1.2kHz loop boundary is reached
  loopRate(1200); 
}

//========================================================================================================================//
//                                                       8. FUNCTIONS                                                     //
//========================================================================================================================//

// communicating with the MPU6500 using I2C protocol.
void IMUinit() {
  Wire.setSCL(PB6); 
  Wire.setSDA(PB7);
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C speed
  
  // Wake IMU
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  
  // Set Gyro Config (250 DPS)
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();
  
  // Set Accel Config (2G)
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission();
}

// Pulls data from IMU
void getIMUdata() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);

  int16_t AcX = Wire.read() << 8 | Wire.read();
  int16_t AcY = Wire.read() << 8 | Wire.read();
  int16_t AcZ = Wire.read() << 8 | Wire.read();
  int16_t Temp = Wire.read() << 8 | Wire.read();
  int16_t GyX = Wire.read() << 8 | Wire.read();
  int16_t GyY = Wire.read() << 8 | Wire.read();
  int16_t GyZ = Wire.read() << 8 | Wire.read();

  // Scale and apply calculated calibration errors
  AccX = (AcX / ACCEL_SCALE_FACTOR) - AccErrorX;
  AccY = (AcY / ACCEL_SCALE_FACTOR) - AccErrorY;
  AccZ = (AcZ / ACCEL_SCALE_FACTOR) - AccErrorZ;
  
  GyroX = (GyX / GYRO_SCALE_FACTOR) - GyroErrorX;
  GyroY = (GyY / GYRO_SCALE_FACTOR) - GyroErrorY;
  GyroZ = (GyZ / GYRO_SCALE_FACTOR) - GyroErrorZ;

  // Apply Low-pass filter to remove vibration noise
  AccX = (1.0 - B_accel)*AccX_prev + B_accel*AccX;
  AccY = (1.0 - B_accel)*AccY_prev + B_accel*AccY;
  AccZ = (1.0 - B_accel)*AccZ_prev + B_accel*AccZ;
  AccX_prev = AccX; AccY_prev = AccY; AccZ_prev = AccZ;

  GyroX = (1.0 - B_gyro)*GyroX_prev + B_gyro*GyroX;
  GyroY = (1.0 - B_gyro)*GyroY_prev + B_gyro*GyroY;
  GyroZ = (1.0 - B_gyro)*GyroZ_prev + B_gyro*GyroZ;
  GyroX_prev = GyroX; GyroY_prev = GyroY; GyroZ_prev = GyroZ;
}

// Run this on a flat surface to get your specific IMU biases; averages 5,000 readings to find that exact baseline offset so it can be subtracted during flight.
void calculate_IMU_error() {
  float sumAccX = 0, sumAccY = 0, sumAccZ = 0;
  float sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;
  int c = 0;
  
  while (c < 5000) {
    getIMUdata();
    sumAccX += AccX; sumAccY += AccY; sumAccZ += AccZ;
    sumGyroX += GyroX; sumGyroY += GyroY; sumGyroZ += GyroZ;
    c++;
    delay(1);
  }
  
  Serial.print("AccErrorX: "); Serial.println(sumAccX / c);
  Serial.print("AccErrorY: "); Serial.println(sumAccY / c);
  Serial.print("AccErrorZ: "); Serial.println((sumAccZ / c) - 1.0); // Gravity offset
  Serial.print("GyroErrorX: "); Serial.println(sumGyroX / c);
  Serial.print("GyroErrorY: "); Serial.println(sumGyroY / c);
  Serial.print("GyroErrorZ: "); Serial.println(sumGyroZ / c);
  
  while(1); // Halt execution so you can copy the values
}

// Mahony filter for 6DOF (Gyro + Accel) Computes absolute orientation based on gravity vector. Don't worry if you don't understand the code, it's very high tech ;)
void Mahony(float gx, float gy, float gz, float ax, float ay, float az, float invSampleFreq) {
  float recipNorm;
  float halfvx, halfvy, halfvz;
  float halfex, halfey, halfez;
  float qa, qb, qc;

  // Convert gyro to radians
  gx *= 0.0174533f; gy *= 0.0174533f; gz *= 0.0174533f;

  if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
    // Normalise accelerometer measurement
    recipNorm = 1.0f / sqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm; ay *= recipNorm; az *= recipNorm;

    // Estimated direction of gravity
    halfvx = q1 * q3 - q0 * q2;
    halfvy = q0 * q1 + q2 * q3;
    halfvz = q0 * q0 - 0.5f + q3 * q3;

    // Error is cross product between estimated and measured direction of gravity
    halfex = (ay * halfvz - az * halfvy);
    halfey = (az * halfvx - ax * halfvz);
    halfez = (ax * halfvy - ay * halfvx);

    if(twoKi > 0.0f) {
      integralFBx += twoKi * halfex * invSampleFreq;
      integralFBy += twoKi * halfey * invSampleFreq;
      integralFBz += twoKi * halfez * invSampleFreq;
      gx += integralFBx; gy += integralFBy; gz += integralFBz;
    }

    // Apply proportional feedback
    gx += twoKp * halfex;
    gy += twoKp * halfey;
    gz += twoKp * halfez;
  }

  // Integrate rate of change of quaternion
  gx *= (0.5f * invSampleFreq);
  gy *= (0.5f * invSampleFreq);
  gz *= (0.5f * invSampleFreq);
  qa = q0; qb = q1; qc = q2;
  q0 += (-qb * gx - qc * gy - q3 * gz);
  q1 += (qa * gx + qc * gz - q3 * gy);
  q2 += (qa * gy - qb * gz + q3 * gx);
  q3 += (qa * gz + qb * gy - qc * gx);

  // Normalise quaternion
  recipNorm = 1.0f / sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm; q1 *= recipNorm; q2 *= recipNorm; q3 *= recipNorm;

  // Compute roll, pitch, yaw from quaternions
  roll_IMU = atan2(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2) * 57.29577951;
  pitch_IMU = -asin(constrain(-2.0f * (q1*q3 - q0*q2), -0.999999, 0.999999)) * 57.29577951;
  yaw_IMU = -atan2(q1*q2 + q0*q3, 0.5f - q2*q2 - q3*q3) * 57.29577951;
}

// Let the Mahony filter get some data for a few seconds so orientation is stable before arming.
void calibrateAttitude() {
  for (int i = 0; i <= 5000; i++) {
    prev_time = current_time;
    current_time = micros();      
    dt = (current_time - prev_time)/1000000.0; 
    getIMUdata();
    Mahony(GyroX, -GyroY, -GyroZ, -AccX, AccY, AccZ, dt);
    loopRate(2000);
  }
}

// Convert RC microsecond inputs (1000-2000) to normalized limits
void getDesState() {
  thro_des = constrain((CHAN_THROTTLE - 1000.0)/1000.0, 0.0, 1.0); 
  roll_des = constrain((CHAN_ROLL - 1500.0)/500.0, -1.0, 1.0) * maxRoll;
  pitch_des = constrain((CHAN_PITCH - 1500.0)/500.0, -1.0, 1.0) * maxPitch;
  yaw_des = constrain((CHAN_YAW - 1500.0)/500.0, -1.0, 1.0) * maxYaw;
}

// Standard PID processing
void controlANGLE() {
  // Roll
  error_roll = roll_des - roll_IMU;
  integral_roll = constrain(integral_roll_prev + error_roll*dt, -i_limit, i_limit);
  if (CHAN_THROTTLE < 1060) integral_roll = 0;
  derivative_roll = GyroX;
  roll_PID = 0.01*(Kp_roll_angle*error_roll + Ki_roll_angle*integral_roll - Kd_roll_angle*derivative_roll); 

  // Pitch
  error_pitch = pitch_des - pitch_IMU;
  integral_pitch = constrain(integral_pitch_prev + error_pitch*dt, -i_limit, i_limit);
  if (CHAN_THROTTLE < 1060) integral_pitch = 0;
  derivative_pitch = GyroY;
  pitch_PID = 0.01*(Kp_pitch_angle*error_pitch + Ki_pitch_angle*integral_pitch - Kd_pitch_angle*derivative_pitch);

  // Yaw (Stabilizes on rate, not absolute angle)
  error_yaw = yaw_des - GyroZ;
  integral_yaw = constrain(integral_yaw_prev + error_yaw*dt, -i_limit, i_limit);
  if (CHAN_THROTTLE < 1060) integral_yaw = 0;
  derivative_yaw = (error_yaw - error_yaw_prev)/dt;
  yaw_PID = 0.01*(Kp_yaw*error_yaw + Ki_yaw*integral_yaw + Kd_yaw*derivative_yaw); 

  // Updates variables for next loop
  integral_roll_prev = integral_roll;
  integral_pitch_prev = integral_pitch;
  error_yaw_prev = error_yaw;
  integral_yaw_prev = integral_yaw;
}

// Map PID outputs to physical actuators (QuadX setup example)
void controlMixer() {
  m1_command_scaled = thro_des - pitch_PID + roll_PID + yaw_PID; // Front Left
  m2_command_scaled = thro_des - pitch_PID - roll_PID - yaw_PID; // Front Right
  m3_command_scaled = thro_des + pitch_PID - roll_PID + yaw_PID; // Back Right
  m4_command_scaled = thro_des + pitch_PID + roll_PID - yaw_PID; // Back Left

  s1_command_scaled = 0.5; // Centered
  s2_command_scaled = 0.5; // Centered
}

// Scale motor mixer outputs into OneShot125 microsecond ranges
void scaleCommands() {
  m1_command_PWM = constrain(m1_command_scaled*125 + 125, 125, 250);
  m2_command_PWM = constrain(m2_command_scaled*125 + 125, 125, 250);
  m3_command_PWM = constrain(m3_command_scaled*125 + 125, 125, 250);
  m4_command_PWM = constrain(m4_command_scaled*125 + 125, 125, 250);

  // Scale servos to 0-180 degrees
  s1_command_PWM = constrain(s1_command_scaled*180, 0, 180);
  s2_command_PWM = constrain(s2_command_scaled*180, 0, 180);
}

// Arming, disarming and Kill Switch
void throttleCut() {
  if (CHAN_KILL < 1250) {
    m1_command_PWM = 120;
    m2_command_PWM = 120;
    m3_command_PWM = 120;
    m4_command_PWM = 120;
  } else if (CHAN_KILL > 1750) {
    m1_command_PWM = 120;
    m2_command_PWM = 120;
    m3_command_PWM = 120;
    m4_command_PWM = 120;
  } else {
    if (CHAN_THROTTLE < 1050) {
      m1_command_PWM = 130;
      m2_command_PWM = 130;
      m3_command_PWM = 130;
      m4_command_PWM = 130;
    }
  }
}

// Pulls designated motor pins HIGH, waits for microsecond limits, and pulls LOW.
void commandMotors() {
  int wentLow = 0;
  int pulseStart, timer;
  int flagM1 = 0, flagM2 = 0, flagM3 = 0, flagM4 = 0;
  
  digitalWrite(m1Pin, HIGH);
  digitalWrite(m2Pin, HIGH);
  digitalWrite(m3Pin, HIGH);
  digitalWrite(m4Pin, HIGH);
  pulseStart = micros();

  while (wentLow < 4) { 
    timer = micros();
    if ((m1_command_PWM <= timer - pulseStart) && (flagM1==0)) { digitalWrite(m1Pin, LOW); wentLow++; flagM1 = 1; }
    if ((m2_command_PWM <= timer - pulseStart) && (flagM2==0)) { digitalWrite(m2Pin, LOW); wentLow++; flagM2 = 1; }
    if ((m3_command_PWM <= timer - pulseStart) && (flagM3==0)) { digitalWrite(m3Pin, LOW); wentLow++; flagM3 = 1; }
    if ((m4_command_PWM <= timer - pulseStart) && (flagM4==0)) { digitalWrite(m4Pin, LOW); wentLow++; flagM4 = 1; }
  }
}

void radioSetup() {
  #if defined USE_PPM_RX
    pinMode(PPM_Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PPM_Pin), getPPM, CHANGE);
  #endif
}

// Low-pass filter for the radio commands to iron out jitter
void getCommands() {
  float b = 0.7; 
  channel_1_pwm = (1.0 - b)*channel_1_pwm_prev + b*channel_1_raw;
  channel_2_pwm = (1.0 - b)*channel_2_pwm_prev + b*channel_2_raw;
  channel_3_pwm = (1.0 - b)*channel_3_pwm_prev + b*channel_3_raw;
  channel_4_pwm = (1.0 - b)*channel_4_pwm_prev + b*channel_4_raw;
  channel_5_pwm = channel_5_raw; // Switches Don't require filter ;)
  channel_6_pwm = channel_6_raw;

  channel_1_pwm_prev = channel_1_pwm;
  channel_2_pwm_prev = channel_2_pwm;
  channel_3_pwm_prev = channel_3_pwm;
  channel_4_pwm_prev = channel_4_pwm;
}

// Checks if radio link sends absolute garbage and overwrites with defaults
void failSafe() {
  if (CHAN_THROTTLE < 800 || CHAN_THROTTLE > 2200 || 
      CHAN_ROLL < 800 || CHAN_ROLL > 2200) {
    channel_1_pwm = channel_1_fs;
    channel_2_pwm = channel_2_fs;
    channel_3_pwm = channel_3_fs;
    channel_4_pwm = channel_4_fs;
    channel_5_pwm = channel_5_fs;
    channel_6_pwm = channel_6_fs;
  }
}

// Idle CPU until time allows for exact loop execution speed
void loopRate(int freq) {
  float invFreq = 1.0 / freq * 1000000.0;
  unsigned long checker = micros();
  while (invFreq > (checker - current_time)) {
    checker = micros();
  }
}

//----------------------------------------------------------
// PPM Interrupt Service Routine
//----------------------------------------------------------
// This runs completely outside the main loop, so we don't miss any radio signal.
// It interrupt triggers, measures the time since the last trigger, and dumps that time into the correct radio channel variable.
void getPPM() {
  unsigned long dt_ppm;
  int trig = digitalRead(PPM_Pin);

  if (trig == 1) {
    dt_ppm = micros() - time_ms;
    time_ms = micros();

    if (dt_ppm > 5000) { 
      ppm_counter = 0; 
    }

    if (ppm_counter == 1) channel_1_raw = dt_ppm;
    if (ppm_counter == 2) channel_2_raw = dt_ppm;
    if (ppm_counter == 3) channel_3_raw = dt_ppm;
    if (ppm_counter == 4) channel_4_raw = dt_ppm;
    if (ppm_counter == 5) channel_5_raw = dt_ppm;
    if (ppm_counter == 6) channel_6_raw = dt_ppm;

    ppm_counter++;
  }
}

//----------------------------------------------------------------
// Function to visually tell if we are in main loop or setup loop
//----------------------------------------------------------------
void ledBlink(int numBlinks,int upTime, int downTime) {
  for (int j = 1; j<= numBlinks; j++) {
    digitalWrite(ledPin, LOW);
    delay(downTime);
    digitalWrite(ledPin, HIGH);
    delay(upTime);
  }
}

//----------------------------------------------------------------------------------
// Functions for Troubleshooting & Data Printing. Will add more in upcoming Versions
//----------------------------------------------------------------------------------
void printRadioData() {
  if (current_time - print_counter > 1000000/data_print_rate) {
    print_counter = micros();
    Serial.print("CH1:"); Serial.print(channel_3_pwm); Serial.print(",");
    Serial.print("CH2:"); Serial.print(channel_1_pwm); Serial.print(",");
    Serial.print("CH3:"); Serial.print(channel_2_pwm); Serial.print(",");
    Serial.print("CH4:"); Serial.print(channel_4_pwm); Serial.print(",");
    Serial.print("CH5:"); Serial.print(channel_5_pwm); Serial.print(",");
    Serial.print("CH6:"); Serial.println(channel_6_pwm);
  }
}

void printDesiredState() {
  if (current_time - print_counter > 1000000/data_print_rate) {
    print_counter = micros();
    Serial.print("Throttle:"); Serial.print(thro_des); Serial.print(",");
    Serial.print("Roll:"); Serial.print(roll_des); Serial.print(",");
    Serial.print("Pitch:"); Serial.print(pitch_des); Serial.print(",");
    Serial.print("Yaw:"); Serial.println(yaw_des);
  }
}

void printRollPitchYaw() {
  if (current_time - print_counter > 1000000/data_print_rate) {
    print_counter = micros();
    Serial.print("Roll:"); Serial.print(roll_IMU); Serial.print(",");
    Serial.print("Pitch:"); Serial.print(pitch_IMU); Serial.print(",");
    Serial.print("Yaw:"); Serial.println(yaw_IMU);
  }
}

void printMotorCommands() {
  if (current_time - print_counter > 1000000/data_print_rate) {
    print_counter = micros();
    Serial.print("M1:"); Serial.print(m1_command_PWM); Serial.print(",");
    Serial.print("M2:"); Serial.print(m2_command_PWM); Serial.print(",");
    Serial.print("M3:"); Serial.print(m3_command_PWM); Serial.print(",");
    Serial.print("M4:"); Serial.println(m4_command_PWM);
  }
}

void printServoCommands() {
  if (current_time - print_counter > 1000000/data_print_rate) {
    print_counter = micros();
    Serial.print("S1: "); Serial.print(s1_command_PWM); Serial.print(",");
    Serial.print("S2: "); Serial.println(s2_command_PWM);
  }
}

void printLoopRate() {
  if (current_time - print_counter > 1000000/data_print_rate) {
    print_counter = micros();
    Serial.print("dt:"); Serial.println(dt*1000000.0);
  }
}