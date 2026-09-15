# ❤️ ESP32 MAX30102 Heart Rate (BPM) & SpO2 Pulse Oximeter

[![GitHub Repository](https://img.shields.io/badge/GitHub-Repository-00f0ff?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ALWINTR/esp32-max30102-pulse-oximeter)
[![Developer](https://img.shields.io/badge/Developer-Alwin_T_R-0284c7?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/alwintr)
[![Platform](https://img.shields.io/badge/Platform-ESP32_&_MAX30102-38bdf8?style=for-the-badge&logo=espressif&logoColor=white)](https://github.com/ALWINTR)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

Biometric Heart Rate (BPM) and SpO2 Blood Oxygen Pulse Oximeter telemetry system using ESP32 and MAX30102.

---

## 📌 Biosensing Theory & Photoplethysmography (PPG)

The MAX30102 measures changes in blood volume in subcutaneous microvascular tissue by emitting alternating Red (660nm) and Infrared (880nm) optical wavelengths and measuring reflected absorption ratios (\( R \)):

$$R = rac{(AC_{	ext{Red}} / DC_{	ext{Red}})}{(AC_{	ext{IR}} / DC_{	ext{IR}})}$$

$$	ext{SpO}_2 (\%) = -45.060 \cdot R^2 + 30.354 \cdot R + 94.845$$

---

## ⚙️ Hardware Specifications

| Component | Technical Specification | Function |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32-WROOM-32 (240MHz) | High-speed I2C sampling & peak-detection filter |
| **Optical Biosensor** | Maxim Integrated MAX30102 | Integrated Red/IR LEDs + Photodetector |
| **Sampling Rate** | 100Hz - 400Hz (Configurable) | High-fidelity photoplethysmogram signal |
| **Display** | SSD1306 0.96" OLED (128x64) | Real-time live heart pulse waveform & vitals |

---

## 🔌 Circuit Pinout Table

| MAX30102 Pin | ESP32 Pin | Description |
| :--- | :--- | :--- |
| **VIN** | 3.3V / 5V | Clean regulated biosensor power rail |
| **GND** | Common GND | System ground reference |
| **SDA** | GPIO 21 | I2C Serial Data line (4.7kΩ pull-up) |
| **SCL** | GPIO 22 | I2C Serial Clock line (4.7kΩ pull-up) |
| **INT** | GPIO 19 | Hardware FIFO sample ready interrupt |

---

## 👨‍💻 Author

**Alwin T R** — Robotics & Automation Engineer  
- 💼 LinkedIn: [linkedin.com/in/alwintr](https://www.linkedin.com/in/alwintr)  
- 🌌 Portfolio: [alwintr.github.io](https://alwintr.github.io)  
- 💻 GitHub: [github.com/ALWINTR](https://github.com/ALWINTR)

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
