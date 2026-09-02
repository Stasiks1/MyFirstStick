# MyFirstStick OS

Multi-application embedded firmware with a **Graphical Launcher Menu** and **Finite State Machine (FSM)** architecture for **M5StickC PLUS (ESP32)**.

## 📱 System Architecture & Menu

The system boots into an interactive **GUI Launcher Menu** (Mode 0) that allows launching 5 standalone hardware applications:

1. **Interactive Clicker (Mode 1)**
   - Bidirectional counter (+1 / -1) clamped to non-negative values using `max()`.
2. **3D IMU Motion Tracker (Mode 2)**
   - Real-time 3-axis acceleration monitoring (`X`, `Y`, `Z`) via the onboard MPU6886 sensor.
3. **Engineering Stopwatch (Mode 3)**
   - Non-blocking millisecond timer built on `millis()`.
   - Start / Pause handling and pause-only Reset logic with buzzer audio feedback.
4. **I2S Noise Level Meter / VU-Meter (Mode 4)**
   - Real-time digital audio stream processing via high-speed **I2S bus** and SPM1423 PDM microphone.
   - Dynamic peak-to-peak wave analysis with a flicker-free visual progress bar.
5. **Universal IR Remote Controller (Mode 5)**
   - 38 kHz infrared signal transmitter on GPIO 9.
   - Dual-burst TV power toggle supporting **NEC (LG)** and **SAMSUNG** protocols simultaneously.

---

## 🎮 Navigation & Controls

### Main Menu (Launcher):
* **Button B (Side, Click):** Scroll cursor (`>`) down the app list.
* **Button A (Front, Click):** Launch the selected application.

### Inside Any Application:
* **Hold Button B (Side, 700ms):** Universal **«BACK TO MENU»** navigation.
* **App Controls:**
  * **Clicker:** `Btn A` (+1), `Btn B` (-1)
  * **Stopwatch:** `Btn A` (Start/Pause), `Btn B` (Reset on pause)
  * **IR Remote:** `Btn A` (Fire TV Power command)
  * **IMU & Noise Meter:** Automated real-time sensor streams.

---

## 🛠 Tech Stack
* **Language:** C++ (Embedded)
* **Platform:** ESP32 / PlatformIO / VS Code
* **Hardware:** M5StickC PLUS (ESP32-PICO, LCD 135x240, MPU6886 IMU, SPM1423 Mic, IR LED, Buzzer)
* **Concepts:** GUI List Rendering, I2S Audio DMA, State Machine, Non-blocking `millis()`, Top-to-Bottom C++ modular architecture.

## 📄 License
MIT
