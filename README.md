### JanFlight
JanFlight is a lightweight, high-performance flight controller designed to run natively on the STM32F405 development board. The code is written in single clean flat-file structure, so it's  highly intuitive and readable. It handles a 2kHz control loop and state estimation without the overhead of external libraries.

Documentation -

### Hardware Requirements
* **MCU Development Board:** STM32F405RGT6 (Approx. 1000 INR)
* **IMU Sensor:** MPU6500 I2C/SPI Module (Approx. 130 INR)
* **RC Receiver:** FlySky FS-iA6B or any receiver capable of outputting a standard PPM, PWM, or SBUS stream.

### Software Requirements
JanFlight is compiled and flashed using the standard Arduino IDE. Follow these steps to configure your environment:

1. Setting up Arduino IDE for STM32
By default, the Arduino IDE does not support STM32 chips. You need to add the official STM32duino core package:
    1. Open the Arduino IDE and navigate to **File > Preferences**.
    2. Locate the **Additional Boards Manager URLs** field and paste the following link:
    `https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json`
    3. Click **OK**, then go to **Tools > Board > Boards Manager...**
    4. Search for **STM32** and install the package titled **STM32 MCU based boards** by STMicroelectronics.

2. Selecting Your Board Configurations
Before compiling, ensure your target hardware parameters are set correctly under the \*\*Tools\*\* menu:
* **Board:** "Generic STM32F4 series"

3. Compiling and Uploading
    1. Open the `JanFlight_v1.0.0.ino` file in the IDE.
    2. Connect your STM32F405 board to your computer via USB.
    3. Select the correct COM port under **Tools > Port**.
    4. Click the **Upload** arrow to compile and flash the code directly to the hardware.

### Performance

The code is highly optimized and written at a low level to minimize memory usage, ensuring low latency and maximum compute space for custom modifications.

* **Flash Memory (ROM Headroom):** Uses roughly 37 KB out of 1024 KB available. You have over **96% of free space remaining** for complex automation, additional control modes, or logging scripts.

* **Dynamic Memory (RAM Headroom):** Uses roughly 2.1 KB out of 128 KB allocated. Nearly the entire memory capacity (**over 98% free space**) is completely open for local workspace and operational variables.

### License
This project is open-source and distributed under the [GPL-3.0 License](https://github.com/oyegunmen/JanFlight?tab=GPL-3.0-1-ov-file#readme).

### Acknowledgement
Special thanks to Nicholas Rehm and his [dRehmFlight](https://github.com/nickrehm/dRehmFlight) project.

## Disclaimer
This code is a shared, open source flight controller for small micro aerial vehicles and is intended to be modified to suit your needs. It is NOT intended to be used on manned vehicles. I do not claim any responsibility for any damage or injury that may be inflicted as a result of the use of this code. Use and modify at your own risk. More specifically put:

> THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.