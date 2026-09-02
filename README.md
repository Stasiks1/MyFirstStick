# MyFirstStick OS

Multi-application embedded firmware with a **Graphical Launcher Menu** and **Finite State Machine (FSM)** architecture for **M5StickC PLUS (ESP32)**.

## 📱 System Architecture & Applications

The system boots into an interactive **GUI Launcher Menu** (Mode 0) that manages 6 standalone hardware applications:

1. **Interactive Clicker (Mode 1)**
   - Bidirectional counter (+1 / -1) clamped to non-negative values using `max()`.
2. **3D IMU Motion Tracker (Mode 2)**
   - Real-time 3-axis acceleration monitoring (`X`, `Y`, `Z`) via onboard MPU6886 sensor.
3. **Engineering Stopwatch (Mode 3)**
   - Non-blocking millisecond timer built on `millis()`.
   - Start / Pause handling and pause-only Reset logic with buzzer audio feedback.
4. **I2S Noise Level Meter / VU-Meter (Mode 4)**
   - Real-time digital audio stream processing via high-speed **I2S bus** and SPM1423 PDM microphone.
   - Dynamic peak-to-peak wave analysis with a flicker-free visual progress bar.
5. **Universal IR Remote Controller (Mode 5)**
   - 38 kHz infrared signal transmitter on GPIO 9.
   - Dual-burst TV power toggle supporting **NEC (LG)** and **SAMSUNG** protocols simultaneously.
6. **Parametric Heart Animation (Mode 6)**
   - Real-time trigonometric rendering using parametric cardioid equations:
     $$X = 16 \cdot \sin^3(t)$$
     $$Y = 13 \cdot \cos(t) - 5 \cdot \cos(2t) - 2 \cdot \cos(3t) - \cos(4t)$$
   - Progressive 3.5-second cinematic contour trace upon launch.
   - Kinetic text particle rendering (`"I LOVE YOU"`, `"LOVE"`) with harmonic pulsating scale.
   - Multi-layered Z-index rendering with embedded easter eggs (`MAMA`, `PAPA`, and a dynamic dual-color pulsating `H` beacon).

---

## 🎮 Navigation & Controls

### Main Menu (Launcher):
* **Button B (Side, Click):** Scroll cursor (`>`) down through apps.
* **Button A (Front, Click):** Launch the selected application.
