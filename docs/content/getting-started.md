# Getting Started

1. Get the required hardware
2. Setup Arduino IDE development environment
3. Configure the Control Mixer
4. Calibrate
5. Compile & Upload
6. FLY!

## 1. Get the required hardware
* STM32F405RGT6 development board (₹1000)
* MPU6500 IMU module (₹130)

and other compatible drone related components.

See this [guide](content/hardware-setup.md) to build your flight controller hardware.

## 2. Development Environment
Download the latest version of the [Arduino IDE](https://www.arduino.cc/en/software/) for your operating system.

Open your Arduino IDE and follow these steps to add STM32 support:

1. Open Arduino IDE, Go to **File > Preferences** Under the Settings tab, locate the **Additional boards manager URLs** field and paste this exact link:

```Arduino
https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
```

![Image](../static/img-pref.png)

2. Open **Tools > Board > Boards Manager**. In the search bar, type **STM32**. Locate STM32 MCU based boards by **STMicroelectronics** and click **Install**.

3. Go to **Tools > Board > STM32 Boards (STM32Duino)** and select **Generic STM32F4** series.

4. Go to **Tools > Board part number** and select **Generic F405RGTx**.

5. Go to **Tools > U(S)ART support** and change the setting to **Enabled (generic 'Serial')**. This activates USB CDC support so you can stream raw telemetry directly through your USB cable.

![Image](../static/img-tools.png)

!> **Info**: Your laptop might need the [Virtual COM Port driver](https://www.st.com/en/development-tools/stsw-stm32102.html) installed if it doesn't automatically detect the board when flashing via DFU.

## 3. Configure the Control Mixer
Download `src/JanFlight_v1.0.0/JanFlight_v1.0.0.ino` from [GitHub](https://github.com/oyegunmen/JanFlight/blob/main/src/JanFlight_v1.0.0/JanFlight_v1.0.0.ino) and locate the `controlMixer()` function. This is where your control inputs map to the motor pins.

Leave the default for a standard QuadX drone, or simply change the plus and minus signs inside this function to match your custom setup.

## 4. Calibrate
Connect your board via USB, select your COM port, and click Upload. Once complete, keep the IMU perfectly flat and uncomment `calculate_IMU_error()` in `setup()`.

Open the Serial Monitor to read your calibration offsets, data will be printed in bottom output panel, copy those numbers into the error variables at the top of your file, and then comment `calculate_IMU_error()` the function back out.

!> Ensure the initial IMU offset errors in Section 3 are set to zero to allow for accurate calibration.

## 5. Compile & Upload
Reconnect your board via USB and Upload the code once more with calibrated IMU offset data.

!> You may need to tune the PID parameters in Section 3 to achieve optimal flight stability. If the drone feels sluggish or unresponsive, adjust these values to suit your specific build.

## 6. FLY!
Disconnect from your computer, double-check your failsafe and throttle cut switches with propellers off, verify the orientation, mount your gears, and head out for a test flight.

*Last Updated: 4th July 2026*