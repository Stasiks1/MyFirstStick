# MyFirstStick

Multi-function embedded firmware built with a **Finite State Machine (FSM)** architecture for **M5StickC PLUS (ESP32)**.

## 🚀 Modes & Features

The firmware operates as a non-blocking state machine with 4 distinct modes:

1. **Interactive Clicker (Mode 0)**
   - Bidirectional counter (+1 / -1) clamped to non-negative values using `max()`.
2. **3D IMU Accelerometer (Mode 1)**
   - Real-time 3-axis acceleration tracking (`X`, `Y`, `Z`) via onboard MPU6886 sensor.
3. **Engineering Stopwatch (Mode 2)**
   - Non-blocking millisecond timer built on `millis()`.
   - Start / Pause state handling and pause-only Reset logic.
   - Audio feedback using onboard buzzer (`M5.Beep`).
4. **Noise Level Meter / VU-Meter (Mode 3)**
   - High-speed digital audio streaming via **I2S protocol** and SPM1423 PDM microphone.
   - Real-time peak amplitude wave analysis.
   - Flicker-free dynamic progress bar rendering.

---

## 🎮 Controls

* **Hold Button B (Side, 700ms):** Cycle through modes (`0 -> 1 -> 2 -> 3 -> 0`).

### Mode Specific Controls:
* **Mode 0 (Clicker):**
  * `Button A (Front)`: Increment counter (+1)
  * `Button B (Side)`: Decrement counter (-1, min 0)
* **Mode 2 (Stopwatch):**
  * `Button A (Front)`: Start / Pause (with beep)
  * `Button B (Side)`: Reset counter to `00` (works only while paused)
* **Modes 1 & 3 (IMU & Noise Meter):** Fully automated real-time sensors.

---

## 🛠 Tech Stack
* **Language:** C++ (Embedded)
* **Platform:** ESP32 / PlatformIO
* **Hardware:** M5StickC PLUS (ESP32-PICO, LCD 135x240, MPU6886, SPM1423 Mic, Buzzer)
* **Drivers & Concepts:** I2S Audio DMA, State Machine, Non-blocking `millis()` timing, Display partial redrawing.

## 📄 License
MIT
