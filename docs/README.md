## Overview
JanFlight is free and open source **STM32F405RGT6** based **arduino** compatible **flight stabilizer (controller?)**.

## Highlights

<div class="demo-grid">
  <div class="row mb-6">
    <div class="col-6">
        <article class="card">
        <header>
            <p><b>EFFICIENT</b></p>
            <p>Consumes <b>< 4% flash memory</b> and <b>< 2% RAM</b>, maximizing memory headroom for custom user code.</p>
        </header>
        </article>
    </div>
    <div class="col-6">
        <article class="card">
        <header>
            <p><b>CONFIGURABLE</b></p>
            <p>A single-function to easily adapt to non-standard vehicle geometries.</p>
        </header>
        </article>
    </div>
  </div>
  <div class="row">
    <div class="col-6">
        <article class="card">
        <header>
            <p><b>STABLE</b></p>
            <p>Executes at 2,000* times every second for perfectly predictable flight.</p>
        </header>
        </article>
    </div>
    <div class="col-6">
        <article class="card">
        <header>
            <p><b>FLAT FILE</b></p>
            <p>The entire code lives in one single file so you can easily see how it works from scratch.</p>
        </header>
        </article>
    </div>
  </div>
</div>

**For context ArduPilot runs at 400 Hz and PX4 at 1000 Hz.*

## Problem

Ever since I started tinkering around drones, my understanding of drone firmware was:
```mermaid
flowchart TD
    Code["`Flight Controller
    [Doing Some Magic]`"]
    User("`User
    [Gives Radio Command]`")
    User --> Code
    Code --> D(Stable Drone)
```

Production-grade open source autopilots are highly capable, but their massive, complex codebase hides the core physics & Maths of flight stabilization.

For hobbyists, this software complexity makes it incredibly difficult to learn the basics, make custom tweaks, or quickly prototype ideas.

## Why?
There exists production grade autopilots, then why [re-invent the wheel](https://programmerhumor.io/memes/reinventing-the-wheel)?

* To understand exactly what mathematical magic happens inside that silicon chip.
* It **looks cool** to fly a aircraft powered by your own flight controller.
* It's fun building your own flight controller.


## Example

The code is not yet tested on any working vehicle. Testing is planned for following standard configurations.

- [ ] QuadCopter
- [ ] Plane

## Safety

By default JanFlight has these safety features enabled:

* **Radio Failsafe**: Automatically shifts to predefined failsafe values, if the drone loses its radio signal or receives a glitched command.

* **Kill Switch**: A dedicated emergency switch that instantly cuts all power to the motors, overriding all other flight commands immediately.

## Disclamier

This code is a shared, open source flight controller for small micro aerial vehicles and is intended to be modified to suit your needs. It is NOT intended to be used on manned vehicles. I do not claim any responsibility for any damage or injury that may be inflicted as a result of the use of this code. Use and modify at your own risk.  

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*Last Updated: 22nd May 2026*