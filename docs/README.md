![Image](static/janflight-banner.jpg)

JanFlight is a toolkit for building DIY flight controllers using STM32, ESP32, or Raspberry Pi Pico boards. Flight tested Arduino-based code, [video tutorials](content/quick-start.md#video-tutorials) and extensive documentation are provided to support rapid prototyping and development.

This is for hobbyists, tinkerers, DIY enthusiasts and ofcourse, non-coders too. Everything is kept as simple and approachable as possible. That said, don't just blindly copy-paste, I encourage you to ask why things are done in a certain way, to understand low level working. 

The goal is anyone should be able to build a working flight controller for under $10, using off the shelf development boards and sensor breakout boards and learn the basics.

If you like JanFlight, please give it a ☆ star on [GitHub](https://github.com/oyegunmen/JanFlight).

## Problem

Ever since I started tinkering around drones, my understanding of drone was:
```mermaid
flowchart TD
    Code["`Flight Controller
    [Doing Some Magic]`"]
    User("`User
    [Gives Radio Command]`")
    User --> Code
    Code --> D(Stable Drone)
```

If I just wanted to fly drones, off-the-shelf hardware/software would have been fine. But I wanted to build drones, while understanding that `magic`, to test my ideas cheaply (In terms of time and money).

Trying to learn drone fundamentals from a fully mature autopilot felt really daunting. The sheer volume of nested files and system dependencies introduces decent learning curve. This guide and codebase provide a readable, low-overhead platform to help you master the essentials first before transitioning to industry-standard software.

## Why?

You could use [dRehmFlight](https://github.com/nickrehm/dRehmFlight), why re-invent the wheel?

* To learn and teach fundamentals of drone related physics, maths and electronics.
* Unavailability of teensy boards.
* To increase the **cool factor** of my content creation ;)

## What this is not?

- This is not a substitute for production-grade flight stacks like Betaflight, ArduPilot, or PX4.
- It should not be used for commercial operations, payload delivery, or high-reliability professional builds.
- It isn't designed to skip the fundamentals; you will need to configure code, debug hardware, and understand the core working.

If you just want to fly without caring how the PID loops work, a commercial flight controller will serve you much better.

## Example

The code is tested on following builds.

- [x] QuadCopter
- [x] Plane

Other configurations are also planned, full list and build status can be [found here](content/initial-setup.md?id=example-guides).

## Functions

Here’s the current feature roadmap and build status:

- Control Mode
    - [x] Angle
    - [ ] Acro
    - [ ] Horizon
- ESC Protocol
    - [X] PWM
    - [X] OneShot125
    - [ ] DShot
- Radio Protocol
    - [X] PPM
    - [ ] i-BUS 
- AHRS
    - [X] Mahony
    - [ ] EKF 

## Disclamier

This code is a shared, open source flight controller for small micro aerial vehicles and is intended to be modified to suit your needs. It is NOT intended to be used on manned vehicles. I do not claim any responsibility for any damage or injury that may be inflicted as a result of the use of this code. Use and modify at your own risk.  

!> THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*Last Updated: 14th Sept 2026*