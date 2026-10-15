# Biometric Pulse Oximeter and Heart Rate Telemetry System

<div align="center">

[![GitHub Repository](https://img.shields.io/badge/GitHub-Repository-00f0ff?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ALWINTR/Biometric-Pulse-Oximeter-and-Heart-Rate-Telemetry-System)
[![Developer](https://img.shields.io/badge/Developer-Alwin_T_R-0284c7?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/alwintr)
[![Portfolio](https://img.shields.io/badge/Portfolio-alwintr.github.io-00f0ff?style=for-the-badge&logo=googlechrome&logoColor=black)](https://alwintr.github.io)
[![License](https://img.shields.io/badge/License-MIT-10b981?style=for-the-badge)](LICENSE)

</div>

> Optical photoplethysmography (PPG) pulse oximeter computing Heart Rate (BPM) and blood oxygen saturation (SpO2) using dual Red (660nm) and Infrared (880nm) LEDs with live OLED waveform display.

---

## 📌 Executive Overview

Optical photoplethysmography (PPG) pulse oximeter computing Heart Rate (BPM) and blood oxygen saturation (SpO2) using dual Red (660nm) and Infrared (880nm) LEDs with live OLED waveform display. This repository contains production-ready firmware implementations, hardware wiring pinouts, signal conditioning routines, and real-time telemetry pipelines engineered for reliable physical operation.

- **Primary Domain**: Radar & Sensing
- **Microcontroller & Hardware Core**: MAX30102 / PPG
- **Specialization**: Biomedical Instrumentation
- **Live Specs & BOM Viewer**: [alwintr.github.io](https://alwintr.github.io)

---

## 📐 System Architecture & Signal Flow

```
[ Human Finger Capillaries ] ===> [ MAX30102 Optical Sensor ]
                                           | Red (660nm) & IR (880nm) Reflection
                                           v
+--------------------------------------------------------------+
|                MAX30102 Integrated Analog Front-End          |
|  - 18-Bit Optical ADC          - 400Hz Sampling Rate FIFO    |
|  - Ambient Light Cancellation  - I2C Interface (400kHz)      |
+--------------------------------------------------------------+
        |
        | Fast I2C Bus (400kHz)
        v
+--------------------------------------------------------------+
|                  ESP32 Dual-Core Microcontroller             |
|  - DC Baseline Subtraction     - Bandpass Filter (0.5-4Hz)   |
|  - Peak-to-Peak BPM Extraction - SpO2 Empirical Ratio Curve  |
+--------------------------------------------------------------+
        |
        v I2C Waveform Stream
+--------------------------------------------------------------+
|        SSD1306 OLED Display (Live Real-Time PPG Waveform)    |
+--------------------------------------------------------------+
```

---

## 📋 Hardware Bill of Materials (BOM)

| Component Name | Operating Voltage | Interface / Functionality |
| :--- | :--- | :--- |
| **ESP32-WROOM-32** | `3.3V DC` | Dual-Core 240MHz SoC |
| **MAX30102 Sensor Module** | `3.3V / 1.8V` | Optical Biosensor |
| **0.96" SSD1306 I2C OLED** | `3.3V DC` | Graphic display for live PPG trace |
| **Piezo Clicker** | `3.3V DC` | Heartbeat acoustic feedback |

---

## 🔌 Circuit Pinout & Microcontroller Interconnect

| MCU Pin / Channel | Peripheral Pin | Signal Function / Bus |
| :--- | :--- | :--- |
| `ESP32 GPIO 21 (SDA)` | `MAX30102 SDA` | I2C Data Bus |
| `ESP32 GPIO 22 (SCL)` | `MAX30102 SCL` | I2C Clock Bus |
| `ESP32 GPIO 19 (INT)` | `MAX30102 INT` | Hardware Data-Ready Interrupt |
| `ESP32 GPIO 25` | `Piezo Clicker` | Heartbeat pulse tone |

---

## ⚙️ Firmware Architecture & Control Logic

Dual-wavelength photoplethysmography digital signal processing extracting AC and DC light absorption ratios to calculate SpO2 percentages and heart rate peaks.

### Key Firmware Capabilities:
- **Zero-Blocking Architecture**: Real-time non-blocking scheduling eliminating delay() deadlocks.
- **Hardware Interrupt Servicing**: Microsecond-precision edge-triggered pulse detection and safety shutoffs.
- **Diagnostic Telemetry**: Continuous serial datastreaming and state-machine transition logging.

---

## 📁 Repository Structure

```
Biometric-Pulse-Oximeter-and-Heart-Rate-Telemetry-System/
├── .gitignore
├── esp32_max30102.ino
├── LICENSE
└── README.md
```

---

## 🚀 Build & Deployment Instructions

### 1. Hardware Assembly
- Verify common system ground across all microcontrollers, drivers, and peripheral sensor boards.
- Ensure dedicated power supply rails match the operating voltage specified in the Hardware Bill of Materials.

### 2. Firmware Compilation & Flashing
```bash
# Clone the repository
git clone https://github.com/ALWINTR/Biometric-Pulse-Oximeter-and-Heart-Rate-Telemetry-System.git

# Navigate to the project directory
cd Biometric-Pulse-Oximeter-and-Heart-Rate-Telemetry-System
```

- Open the project in **Arduino IDE**, **PlatformIO**, or **ESP-IDF**.
- Select the target board configuration and COM port.
- Compile and upload firmware directly to the microcontroller.

---

## 👨‍💻 Author

**Alwin T R** — Robotics & Automation Engineer  
- 💼 **LinkedIn**: [linkedin.com/in/alwintr](https://www.linkedin.com/in/alwintr)  
- 🌐 **Interactive Portfolio**: [alwintr.github.io](https://alwintr.github.io)  
- 🐙 **GitHub**: [github.com/ALWINTR](https://github.com/ALWINTR)  
- ✉️ **Direct Email**: [alwintr2003@gmail.com](mailto:alwintr2003@gmail.com)

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
