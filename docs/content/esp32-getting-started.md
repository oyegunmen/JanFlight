# ESP32

1. Get the required hardware
2. Development Environment
3. Configuration
4. Calibrate
5. Compile & Upload
6. FLY!

## 1. Get the required hardware
* ESP32 Breakout Board (30-pin or 38-pin)
* MPU6500 IMU module

![Image](../static/parts_esp32.jpg)

and other drone related parts.

See this [guide](esp32-hardware-setup.md) to build your flight controller hardware.

## 2. Development Environment
Download the latest version of the [Arduino IDE](https://www.arduino.cc/en/software/) for your operating system.

Open your Arduino IDE and follow these steps to add ESP32 support:

1. Open Arduino IDE, go to **File > Preferences**. Under the Settings tab, locate the **Additional boards manager URLs** field and paste this exact link:

```Arduino
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

![Image](../static/esp32-pref.png)

2. Open **Tools > Board > Boards Manager**. In the search bar, type **esp32**. Locate **esp32 by Espressif Systems** and click **Install**.

3. Go to **Tools > Board > ESP32 Arduino** and select **ESP32 Dev Module** (this matches most generic WROOM-32 DevKit boards; pick your board's exact name if listed separately).

4. Go to **Tools > Upload Speed** and set it to **921600**. If you see repeated "Connecting..." failures during upload, drop this to **115200**.

5. Go to **Tools > Port** and select the COM port your board enumerates as once connected via USB.

![Image](../static/esp32-tools.png)

!> **Info**: Your laptop might need the Virtual COM Port driver installed if it doesn't automatically detect the board when flashing; just the standard [CP210x](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)/[CH340](https://sparks.gogo.co.nz/ch340.html) driver for your OS if your board isn't detected automatically.

## 3. Configuration
Download STM32 based firmware from [GitHub](https://github.com/oyegunmen/JanFlight/blob/main/src/STM32/JanFlight_v1.0.0/JanFlight_v1.0.0.ino).

(a) **Update the Pin Declaration:** Refer to your specific STM32 board's datasheet and pinout diagram to determine the correct pins for your needs. Navigate to the section 4 of the code and change the pin assignments to match your respective board.

![Image](../static/esp32-pinout.webp)

b) **Adjust the Control Mixer:** Locate the `controlMixer()` function. This is where your radio control inputs map to the motor pins you just defined. Leave the default for a standard QuadX drone, or simply change the plus and minus signs inside this function to match your custom motor layout and rotation setup.

## 4. Calibrate
Connect your board via USB, select your COM port, and click Upload. Once complete, keep the IMU perfectly flat and uncomment `calculate_IMU_error()` in `setup()`.

Open the Serial Monitor to read your calibration offsets, data will be printed in bottom output panel, copy those numbers into the error variables at the top of your file, and then comment `calculate_IMU_error()` the function back out.

!> Ensure the initial IMU offset errors in Section 3 are set to zero to allow for accurate calibration.

## 5. Compile & Upload
Reconnect your board via USB and Upload the code once more with calibrated IMU offset data.

!> You may need to tune the PID parameters in Section 3 to achieve optimal flight stability. If the drone feels sluggish or unresponsive, adjust these values to suit your specific build.

## 6. FLY!
Disconnect from your computer, double-check your failsafe and throttle cut switches with propellers off, verify the orientation, mount your gears, and head out for a test flight.

*Last Updated: 13th July 2026*
