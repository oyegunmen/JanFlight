JanFlight is a guide or toolkit to build DIY Flight Controller for any [High Performance](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html) STM32 MCU.

Highly readable, intutive and single file Arduino based Flight Stabilizer code is also provided to support rapid prototyping and development.

Read the [Documentation](https://oyegunmen.github.io/JanFlight/#/)

## Hardware Requirements
* **MCU Development Board:** STM32F405RGT6 (₹1000)
* **IMU Sensor:** MPU6500 (₹130)

and other required drone related parts.

> [!NOTE]
> This code is compatible with any [STM32 High Performance](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html) series chip. While it has been specifically tested on the STM32F405, but it should hopefully work on other series as well.

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