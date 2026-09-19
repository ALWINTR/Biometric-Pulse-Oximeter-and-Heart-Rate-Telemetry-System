# Biometric Pulse Oximeter and Heart Rate Telemetry System

[![GitHub Repository](https://img.shields.io/badge/GitHub-Repository-00f0ff?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ALWINTR/esp32-max30102-pulse-oximeter)
[![Developer](https://img.shields.io/badge/Developer-Alwin_T_R-0284c7?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/alwintr)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

An optical photoplethysmography (PPG) biometric monitoring station powered by the **MAX30102** optical biosensor and **ESP32**, executing discrete peak-detection algorithms to compute Heart Rate (BPM) and blood oxygen saturation (SpO2) with live waveform rendering on an OLED display.

---

## Biosensing Theory and Absorption Ratios

The MAX30102 measures changes in blood volume in subcutaneous tissue by emitting alternating Red (660nm) and Infrared (880nm) optical wavelengths and measuring reflected absorption ratios ($R$):

$$R = \frac{(AC_{\text{Red}} / DC_{\text{Red}})}{(AC_{\text{IR}} / DC_{\text{IR}})}$$

$$\text{SpO}_2 (\%) = -45.060 \cdot R^2 + 30.354 \cdot R + 94.845$$

---

## Circuit Pinout Table

| MAX30102 Pin | ESP32 Pin | Description |
| :--- | :--- | :--- |
| **VIN** | 3.3V / 5V | Clean regulated biosensor power rail |
| **GND** | Common GND | System ground reference |
| **SDA** | GPIO 21 | I2C Serial Data line with 4.7k pull-up |
| **SCL** | GPIO 22 | I2C Serial Clock line with 4.7k pull-up |
| **INT** | GPIO 19 | Hardware FIFO sample ready interrupt |

---

## Author

**Alwin T R** - Robotics and Automation Engineer  
- LinkedIn: [linkedin.com/in/alwintr](https://www.linkedin.com/in/alwintr)  
- Portfolio: [alwintr.github.io](https://alwintr.github.io)  
- GitHub: [github.com/ALWINTR](https://github.com/ALWINTR)

---

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.
