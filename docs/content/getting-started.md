# Janflight

1. Get the required hardware
2. Setup Arduino IDE development environment
3. Configure the Control Mixer
4. Calibrate
5. Compile & Upload
6. FLY!

For additional help see [JanFlight Discussions](https://github.com/oyegunmen/JanFlight)

## 1. Get the required hardware
* STM32F405RGT6 development board
* MPU6500 IMU module
* RC Receiver: PPM
* BEC or DC-DC converter to power your board from a battery
* ESC (OneShot125) and/or servos (PWM)

## 2. Development Environment
Install the official STM32duino core via the Arduino Boards Manager using the STMicroelectronics index URL. Once installed, select the Generic STM32F4 series board, choose the F405RGTx part number, and ensure USB CDC support is enabled so you can receive raw telemetry through your USB cable.

## 3. Configure the Control Mixer
Open the file and look for the `controlMixer()` function. This is where you map your PID outputs to physical pins. If you are running a standard QuadX, the default math balances pitch, roll, and yaw forces across four motors. If you are building something custom like a wing or a dual-motor setup, alter the additions and subtractions here to match your layout.

## 4. Calibrate
Connect your STM32 board via USB. Select the correct COM port in the IDE, click the Upload button, and wait for the compilation to finish. With the board (basically IMU Needs to be on flat surface) sitting perfectly flat on a level surface, uncomment the `calculate_IMU_error()` function in `setup()`. Open the serial monitor to read your IMU offset values. Copy these numbers, hardcode them into your error variables at the top of the file, and re-comment the calibration function so it does not run on every boot.

## 5. Compile & Upload
Connect your STM32 board via USB again and upload the code again. The IDE will flash the binary directly to the board's internal memory.

## 6. FLY!
Disconnect from your computer, double-check your failsafe and throttle cut switches on the bench with propellers off, ensure the orientation moves correctly in a telemetry check, mount your gear, and head out for a test flight.

*Last Updated: 21st May 2026*