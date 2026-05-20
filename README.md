JanFlight is a lightweight, high-performance flight controller designed to run natively on the STM32F405 development board. The code is written in single clean flat-file structure, so it's  highly intuitive and readable. It handles a 2kHz control loop and state estimation without the overhead of external libraries.

## Hardware Requirements
* **MCU Development Board:** STM32F405RGT6 (Approx. 1000 INR)
* **IMU Sensor:** MPU6500 I2C/SPI Module (Approx. 130 INR)
* **RC Receiver:** FlySky FS-iA6B or any receiver capable of outputting a standard PPM, PWM, or SBUS stream.

## Software Requirements
JanFlight is compiled and flashed using the standard Arduino IDE. Detailed steps are included in the JanFlight documentation website.

## Performance

The code is highly optimized With only 37 KB of 1024 KB Flash and 2.1 KB of 128 KB RAM in use, you retain over 96% and 98% free space respectively, leaving massive headroom for complex automation, additional control modes, logging scripts, and operational variables.

## License
This project is open-source and distributed under the [GPL-3.0 License](https://github.com/oyegunmen/JanFlight?tab=GPL-3.0-1-ov-file#readme).

## Acknowledgement
Special thanks to Nicholas Rehm and his [dRehmFlight](https://github.com/nickrehm/dRehmFlight) project.

## Disclaimer
This code is a shared, open source flight controller for small micro aerial vehicles and is intended to be modified to suit your needs. It is NOT intended to be used on manned vehicles. I do not claim any responsibility for any damage or injury that may be inflicted as a result of the use of this code. Use and modify at your own risk. More specifically put:

> THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.