#include "FastIMU.h"
#include <Wire.h>

#define IMU_ADDRESS 0x68    //Change to the address of the IMU
#define PERFORM_CALIBRATION //Comment to disable startup calibration
#define BUTTON_PIN 5       // Define the button pin
MPU6500 IMU;               //Change to the name of any supported IMU!

// Currently supported IMUS: MPU9255 MPU9250 MPU6886 MPU6500 MPU6050 ICM20689 ICM20690 BMI055 BMX055 BMI160 LSM6DS3 LSM6DSL QMI8658

calData calib = { 0 };  //Calibration data
AccelData accelData;    //Sensor data
GyroData gyroData;
MagData magData;

bool start_data_transmission = false;
long lastPrintTime = 0;
const int samplingRate = 20; // Sample every 50ms
bool buttonState = false;  // Current state of the button
bool lastButtonState = false; // Previous state of the button

void setup() {
  Wire.begin();
  Wire.setClock(400000); //400khz clock
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Initialize button pin with internal pull-up resistor
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }

#ifdef PERFORM_CALIBRATION
  Serial.println("Keep IMU level.");
  delay(3000);
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibration done!");
  calib.accelBias[0];
  calib.accelBias[1];
  calib.accelBias[2];
  calib.gyroBias[0];
  calib.gyroBias[1];
  calib.gyroBias[2];
  IMU.init(calib, IMU_ADDRESS);
#endif

  if (err != 0) {
    Serial.print("Error Setting range: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }
  Serial.println("Ready"); // Send ready message
}

void loop() {

  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) { // Button pressed (LOW because of pullup)
      start_data_transmission = !start_data_transmission; // Toggle data transmission
      if(start_data_transmission){
        Serial.println("Recording Started");
        lastPrintTime= millis();
      }else{
         Serial.println("Recording Stopped");
      }
       
    }
    lastButtonState = buttonState;  // Update the previous button state
    delay(50); // Simple debounce delay
  }
  
   if (start_data_transmission){

     if(millis() - lastPrintTime >= (1000 / samplingRate)) //Sampling rate
     {

        IMU.update();
        IMU.getAccel(&accelData);
        Serial.print(accelData.accelX-calib.accelBias[0]);
        Serial.print(",");
        Serial.print(accelData.accelY-calib.accelBias[1]);
        Serial.print(",");
        Serial.print(accelData.accelZ-calib.accelBias[2]);
        Serial.print(",");
        IMU.getGyro(&gyroData);
        Serial.print(gyroData.gyroX-calib.gyroBias[0]);
        Serial.print(",");
        Serial.print(gyroData.gyroY-calib.gyroBias[1]);
        Serial.print(",");
        Serial.print(gyroData.gyroZ-calib.gyroBias[2]);
        Serial.print("\n");
        lastPrintTime = millis();
       }
   }
}