/*
  RC Receiver -> USB Joystick (Gamepad) Bridge
  ---------------------------------------------
  Board     : Raspberry Pi Pico 2 / RP2350
  Radio     : FlySky FS-i6 (PPM or iBUS output from receiver)
  Purpose   : Decode PPM or iBUS from the receiver and present the stick /
              switch positions to the PC as a native USB HID joystick -
              the format FPV simulators (FPV SkyDive, Liftoff, VelociDrone,
              FPV Freerider, etc.) actually read.

  Data flow : Radio TX -> Radio RX -> RP2350 -> PC (USB)

  IDE setup : Tools > Board      > Raspberry Pi Pico 2 (or your RP2350 board)
              Tools > USB Stack  > Pico SDK   <-- required, Joystick is not
                                                  available under Adafruit TinyUSB

  Wiring    : Receiver signal pin -> GP28
              Receiver GND        -> GND
              Receiver VCC        -> per receiver's supply requirement
              NOTE: RP2350 GPIOs are 3.3V only. If your receiver's signal
              pin idles/swings above 3.3V, use a level shifter or a simple
              resistor divider before GP28.
*/

// ============================================================
// 1. USER SPECIFIED DEFINES
// ============================================================

// Choose receiver protocol (uncomment only one)
//#define USE_PPM_RX          // Default
#define USE_IBUS_RX

// Number of channels sent by the receiver
#define NUM_CHANNELS   6

// Channel mapping - which receiver channel drives which stick/switch
// (1-indexed, matches FlySky FS-i6 default channel order: AETR)
#define CH_AILERON     5   // Roll     -> Joystick X
#define CH_ELEVATOR    6   // Pitch    -> Joystick Y
#define CH_THROTTLE    3   // Throttle -> Joystick Z
#define CH_RUDDER      4   // Yaw      -> Joystick Z-rotate
#define CH_AUX1        1   // Switch   -> Joystick Slider Left
#define CH_AUX2        2   // Switch   -> Joystick Slider Right

// Reverse a channel's direction if it feels inverted in the simulator
// (1 = normal, -1 = reversed)
#define REV_AILERON     1
#define REV_ELEVATOR    1
#define REV_THROTTLE    1
#define REV_RUDDER      1
#define REV_AUX1        1
#define REV_AUX2        1

// Raw pulse width range from the receiver, in microseconds
#define RC_PULSE_MIN   1000
#define RC_PULSE_MAX   2000

// ============================================================
// 2. REQUIRED LIBRARIES
// ============================================================

#include <Joystick.h>          // Bundled with Arduino-Pico core - USB HID gamepad

#if defined(USE_IBUS_RX)
#include <SerialPIO.h>         // Bundled with Arduino-Pico core - UART on any GPIO
#endif

// ============================================================
// 3. PIN DECLARATION
// ============================================================

#define RX_PIN   28   // GP28 - PPM signal OR iBUS serial data from the receiver

// ============================================================
// 4. GLOBAL VARIABLE DECLARATION
// ============================================================

// Shared output: latest value of each channel, 1-indexed, in microseconds
uint16_t channelValue[NUM_CHANNELS + 1];
bool     newFrameAvailable = false;

#if defined(USE_PPM_RX)
volatile uint16_t ppmValues[NUM_CHANNELS + 1];
volatile uint8_t  ppmChannelIndex = 0;
volatile uint32_t lastPulseTime   = 0;
volatile bool     ppmFrameReady   = false;
#endif

#if defined(USE_IBUS_RX)
SerialPIO ibusSerial(NOPIN, RX_PIN);  // TX unused, RX on RX_PIN
uint8_t   ibusBuffer[32];
uint8_t   ibusIndex = 0;
#endif

// ============================================================
// 5. VOID SETUP
// ============================================================

void setup() {
  setupReceiver();
  setupJoystick();
}

// ============================================================
// 6. VOID LOOP
// ============================================================

void loop() {
  readReceiver();

  if (newFrameAvailable) {
    updateJoystick();
    newFrameAvailable = false;
  }
}

// ============================================================
// 7. FUNCTIONS
// ============================================================

// ---- Setup helpers ----

void setupReceiver() {
#if defined(USE_PPM_RX)
  pinMode(RX_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RX_PIN), ppmISR, RISING);
#elif defined(USE_IBUS_RX)
  ibusSerial.begin(115200);
#endif
}

void setupJoystick() {
  Joystick.begin();
  Joystick.useManualSend(true);   // We control exactly when a report is sent
}

// ---- Receiver reading (protocol dispatch) ----

void readReceiver() {
#if defined(USE_PPM_RX)
  readPPM();
#elif defined(USE_IBUS_RX)
  readIBUS();
#endif
}

#if defined(USE_PPM_RX)
// Copies a completed PPM frame from the ISR buffer into channelValue[]
void readPPM() {
  if (!ppmFrameReady) return;

  noInterrupts();
  for (uint8_t ch = 1; ch <= NUM_CHANNELS; ch++) {
    channelValue[ch] = ppmValues[ch];
  }
  ppmFrameReady = false;
  interrupts();

  newFrameAvailable = true;
}

// Interrupt Service Routine - measures the time between rising edges.
// A long gap marks the start of a new PPM frame; shorter gaps are channels.
void ppmISR() {
  uint32_t now = micros();
  uint32_t pulseWidth = now - lastPulseTime;
  lastPulseTime = now;

  if (pulseWidth > 3000) {
    ppmChannelIndex = 0;
  } else if (ppmChannelIndex < NUM_CHANNELS) {
    ppmValues[ppmChannelIndex + 1] = pulseWidth;
    ppmChannelIndex++;
    if (ppmChannelIndex == NUM_CHANNELS) {
      ppmFrameReady = true;
    }
  }
}
#endif

#if defined(USE_IBUS_RX)
// Parses FlySky iBUS packets (32 bytes, 115200 baud) as bytes arrive
void readIBUS() {
  while (ibusSerial.available()) {
    uint8_t b = ibusSerial.read();

    if (ibusIndex == 0 && b != 0x20) {
      continue;   // Wait for the frame length/start byte
    }

    ibusBuffer[ibusIndex++] = b;

    if (ibusIndex == 32) {
      if (isIBUSChecksumValid()) {
        for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
          channelValue[ch + 1] = ibusBuffer[2 + ch * 2] | (ibusBuffer[3 + ch * 2] << 8);
        }
        newFrameAvailable = true;
      }
      ibusIndex = 0;
    }
  }
}

bool isIBUSChecksumValid() {
  uint16_t checksum = 0xFFFF;
  for (uint8_t i = 0; i < 30; i++) {
    checksum -= ibusBuffer[i];
  }
  uint16_t receivedChecksum = ibusBuffer[30] | (ibusBuffer[31] << 8);
  return checksum == receivedChecksum;
}
#endif

// ---- Joystick output ----

void updateJoystick() {
  Joystick.X(mapChannel(CH_AILERON,  REV_AILERON));
  Joystick.Y(mapChannel(CH_ELEVATOR, REV_ELEVATOR));
  Joystick.Z(mapChannel(CH_THROTTLE, REV_THROTTLE));
  Joystick.Zrotate(mapChannel(CH_RUDDER, REV_RUDDER));
  Joystick.sliderLeft(mapChannel(CH_AUX1, REV_AUX1));
  Joystick.sliderRight(mapChannel(CH_AUX2, REV_AUX2));
  Joystick.send_now();
}

// Converts a channel's raw pulse width (RC_PULSE_MIN..RC_PULSE_MAX) into a
// 0-1023 HID axis value, optionally reversed.
int mapChannel(uint8_t channel, int8_t reverseDirection) {
  uint16_t pulseWidth = constrain(channelValue[channel], RC_PULSE_MIN, RC_PULSE_MAX);
  int value = map(pulseWidth, RC_PULSE_MIN, RC_PULSE_MAX, 0, 1023);
  return (reverseDirection < 0) ? (1023 - value) : value;
}
