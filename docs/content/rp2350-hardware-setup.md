# Raspberry Pi Pico Hardware Setup

This guide covers building a custom flight controller using a Raspberry Pi Pico 2 (RP2350).

This example uses the [Raspberry Pi Pico 2](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#pico2).

![Image](../static/rp2350-pinout.webp)

## Required Components

* Raspberry Pi Pico 2
* MPU6500 IMU
* FlySky FS-iA6B Receiver (or any PPM-output receiver)
* Header pins
* Soldering iron, flux, soldering wire, jumper wires, and cutting plier

## Soldering

1. Get the required parts.

![Image](../static/parts_rp2350.jpg)

2. Solder header pins along both GPIO rows, matching the pin groups used in the wiring table below.

Solder a dedicated 5V/GND power supply from your Power Distribution Board (PDB) to the Pico 2's `VSYS`/`GND` pins (or a spare 5V/GND header pair) to prevent voltage sags.

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
      <td>GP5</td>
      <td>SCL</td>
    </tr>
    <tr>
      <td>GP4</td>
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
      <td>GP18</td>
      <td>SCL</td>
    </tr>
    <tr>
      <td>GP19</td>
      <td>SDA</td>
    </tr>
    <tr>
      <td>GP16</td>
      <td>ADO</td>
    </tr>
    <tr>
      <td>GP1</td>
      <td>NCS</td>
    </tr>
    <tr>
      <td rowspan="4"><strong>Radio</strong></td>
      <td>GP2</td>
      <td>PPM/CH1</td>
      <td rowspan="4">PPM</td>
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
      <td>-</td>
      <td>-</td>
    </tr>
    <tr>
      <td rowspan="4"><strong>ESC</strong></td>
      <td>GP6</td>
      <td>SIGNAL (ESC1)</td>
      <td rowspan="4">PWM/OneShot125</td>
    </tr>
    <tr>
      <td>GP7</td>
      <td>SIGNAL (ESC2)</td>
    </tr>
    <tr>
      <td>GP8</td>
      <td>SIGNAL (ESC3)</td>
    </tr>
    <tr>
      <td>GP9</td>
      <td>SIGNAL (ESC4)</td>
    </tr>
  </tbody>
</table>

!> **Warning**: Cut or remove the positive (red) power wire from all ESC signal connectors. Connecting them directly to the board will cause voltage back-feeding, potentially damaging your ESCs or the MCU.

## Firmware Flash & Verification

1. Connect the RP2340 to your laptop via USB and flash the [Janflight firmware](https://github.com/oyegunmen/JanFlight/blob/main/src/RP2350/JanFlight_v1.0.0/JanFlight_v1.0.0.ino).

2. Onboard LED Indicators (GP25 on a plain Pico 2):
    * Three quick blinks indicating the start of the setup.
    * Two quick blinks indicating the start of the main loop.
    * Consistent 1-second interval blinking confirming the loop is running.

3. Open the code in the Arduino IDE, scroll down to the main loop, and uncomment the following debug functions one by one, flashing the code each time to verify data in the Serial Monitor:
    * `printRadioData()`
    * `printRollPitchYaw()`
    * `printMotorCommands()`

If you are seeing data being printed in your serial monitor then your connections are fine.

Congratulations, your RP2350-based flight controller is ready for flying!

*Last Updated: 20th July 2026*
