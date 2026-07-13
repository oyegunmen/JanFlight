# ESP32 Hardware Setup

This guide covers building a custom flight controller using an ESP32 breakout board.

This example uses a generic **ESP32 DevKitC** (38-pin breakout, sometimes labeled "ESP32 DevKitC" or "ESP32-DevKitV1").

![Image](../static/esp32-pinout.webp)

## Required Components

* ESP32 DevKitC
* MPU6500 IMU
* FlySky FS-iA6B Receiver (or any PPM-output receiver)
* Header pins
* Soldering iron, flux, soldering wire, jumper wires, and cutting plier

## Soldering

1. Get the required parts.

2. Solder header pins to the board, matching the pin groups used in the wiring table below.

Solder a dedicated 5V/GND power supply from your Power Distribution Board (PDB) to the ESP32's `VIN`/`GND` pins (or a spare 5V/GND header pair) to prevent voltage sags.

!> Ensure heat shrink tubes are used to avoid short circuit while keeping wiring clean.

## Wiring

Connect components as outlined in the table below:

<table>
  <thead>
    <tr>
      <th>Component</th>
      <th>MCU Pin</th>
      <th>Part Pin</th>
      <th>Protocol</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="10"><strong>IMU</strong></td>
      <td>3.3 V</td>
      <td>VCC</td>
      <td rowspan="4">I2C</td>
    </tr>
    <tr>
      <td>GND</td>
      <td>GND</td>
    </tr>
    <tr>
      <td>GPIO22</td>
      <td>SCL</td>
    </tr>
    <tr>
      <td>GPIO21</td>
      <td>SDA</td>
    </tr>
    <tr>
      <td>3.3V</td>
      <td>VCC</td>
      <td rowspan="6">SPI</td>
    </tr>
    <tr>
      <td>GND</td>
      <td>GND</td>
    </tr>
    <tr>
      <td>GPIO18</td>
      <td>SCL</td>
    </tr>
    <tr>
      <td>GPIO23</td>
      <td>SDA</td>
    </tr>
    <tr>
      <td>GPIO19</td>
      <td>ADO</td>
    </tr>
    <tr>
      <td>GPIO5</td>
      <td>NCS</td>
    </tr>
    <tr>
      <td rowspan="3"><strong>Radio</strong></td>
      <td>GPIO4</td>
      <td>PPM/CH1</td>
      <td rowspan="3">PPM</td>
    </tr>
    <tr>
      <td>5 V</td>
      <td>Power</td>
    </tr>
    <tr>
      <td>GND</td>
      <td>GND</td>
    </tr>
    <tr>
      <td rowspan="4"><strong>ESC</strong></td>
      <td>GPIO25</td>
      <td>SIGNAL (ESC1)</td>
      <td rowspan="4">PWM/OneShot125</td>
    </tr>
    <tr>
      <td>GPIO26</td>
      <td>SIGNAL (ESC2)</td>
    </tr>
    <tr>
      <td>GPIO32</td>
      <td>SIGNAL (ESC3)</td>
    </tr>
    <tr>
      <td>GPIO33</td>
      <td>SIGNAL (ESC4)</td>
    </tr>
  </tbody>
</table>

!> **Warning**: Cut or remove the positive (red) power wire from all ESC signal connectors. Connecting them directly to the board will cause voltage back-feeding, potentially damaging your ESCs or the MCU.

## Firmware Flash & Verification

1. Connect the ESP32 to your laptop via USB and flash the [Janflight firmware](ttps://github.com/oyegunmen/JanFlight/blob/main/src/ESP32/JanFlight_v1.0.0/JanFlight_v1.0.0.ino).

2. Blue LED Indicators:
    * Three quick blinks indicating the start of the setup.
    * Two quick blinks indicating the start of the main loop.
    * Consistent 1-second interval blinking confirming the loop is running.

!> **Warning:** On the 38-pin ESP32 DevKitC, the onboard blue LED is physically hardwired to GPIO 1, which is the exact same pin used for the USB Serial Transmit (TX) line. You cannot use both simultaneously. Attempting to blink the onboard LED while sending data to the Serial Monitor will cause signal collisions and one of the function will work.

3. Open the code in the Arduino IDE, scroll down to the main loop, and uncomment the following debug functions one by one, flashing the code each time to verify data in the Serial Monitor:
    * `printRadioData()`
    * `printRollPitchYaw()`
    * `printMotorCommands()`

If you are seeing data being printed in your serial monitor then your connections are fine.

Congratulations, your ESP32 based flight controller is ready for flying!

*Last Updated: 13th July 2026*
