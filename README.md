![Image](docs/static/janflight-banner.jpg)

JanFlight is a guide or toolkit to build DIY Flight Controller for any [High Performance](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html) STM32, ESP32 and RP2350 boards.

Highly readable, intutive and single file Arduino based Flight Stabilizer code is also provided to support rapid prototyping and development.

Read the [Documentation](https://oyegunmen.github.io/JanFlight/#/)

## Hardware Requirements
* **Breakout Board:** STM32/ESP32/RP2350
* **IMU Sensor:** MPU6500

and other drone related parts.

> [!NOTE]
> This firmware is tested on the following boards, but should support the broader STM32, ESP32, and Raspberry Pi microcontroller families. Update the pin definitions in the code to match your specific board's hardware datasheet.

| MCU Family | Breakout Board | Clock | Flash | RAM | Price |
|-------------|-----------------|-----------------|-----------------|-----------------|-----------------|
| **STM32** | [WeAct Studio STM32F405RGT6](https://github.com/WeActStudio/WeActStudio.STM32F4_64Pin_CoreBoard) | 168MHz | 1MB | 128KB | ₹1000/$7/€7 |
| **ESP32** | [ESP32 DevKitC](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html) | 240MHz | 4MB | 520KB | ₹450/$5/€5 |
| **RP2350** | (Coming Soon) | 240 MHz (overclock 300MHz) | 4MB | 520KB | ₹540/$6/€6 |

## Software Requirements
JanFlight is compiled and flashed using the standard Arduino IDE. Detailed steps are included in the JanFlight documentation website.

## Testing
Testing is planned for following standard configurations.
- [ ] QuadCopter (The build is complete; PID Tuning ongoing)
- [ ] Plane

## License
This project is open-source and distributed under the [GPL-3.0 License](https://github.com/oyegunmen/JanFlight?tab=GPL-3.0-1-ov-file#readme).

## Disclaimer
This code is a shared, open source flight controller for small micro aerial vehicles and is intended to be modified to suit your needs. It is NOT intended to be used on manned vehicles. I do not claim any responsibility for any damage or injury that may be inflicted as a result of the use of this code. Use and modify at your own risk. More specifically put:

> [!WARNING]
> THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.