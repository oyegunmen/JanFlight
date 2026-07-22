![Image](static/janflight-banner.jpg)

JanFlight is a guide or toolkit to build DIY Flight Controller for any STM32, ESP32 and Raspberry Pi Pico boards.

Highly readable, intutive and single file Arduino based Flight Stabilizer code is also provided to support rapid prototyping and development.

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

If I just wanted to fly drones, off-the-shelf hardware/software would have been fine. But I wanted to build drones and test by weirdest ideas cheaply (In terms of time and money).

The problem is, most mainstream autopilots are way too complex for a **beginner** to learn anything useful cheaply. I really needed something DIY that felt intuitive and approachable for a beginner.

## Why?

You could use [dRehmFlight](https://github.com/nickrehm/dRehmFlight), why [re-invent the wheel](https://programmerhumor.io/memes/reinventing-the-wheel)?

* To understand the physics and math at the hardware and software levels.
* Unavailability of teensy boards.
* To increase the **cool factor** of my content creation ;)

## Example

The code is tested on following builds. Future builds and results will be published on this site and my instagram handle.

- [x] QuadCopter (Will publish a video soon)
- [ ] Plane

## Disclamier

This code is a shared, open source flight controller for small micro aerial vehicles and is intended to be modified to suit your needs. It is NOT intended to be used on manned vehicles. I do not claim any responsibility for any damage or injury that may be inflicted as a result of the use of this code. Use and modify at your own risk.  

!> THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*Last Updated: 4th July 2026*