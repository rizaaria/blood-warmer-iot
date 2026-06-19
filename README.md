# Blood Warmer IoT

## Overview

Blood Warmer IoT is a portable temperature monitoring and heating system designed to maintain blood temperature within a safe range during transfusion procedures. The system uses an ESP32 microcontroller, DS18B20 temperature sensors, a nichrome wire heater, OLED display, and Blynk cloud integration for real-time monitoring.

This project demonstrates the integration of embedded systems, sensor acquisition, temperature control, and IoT technology in a biomedical engineering application.

---

## Features

- Real-time temperature monitoring
- Dual DS18B20 temperature sensors
- PWM-based heater control
- OLED SSD1306 temperature display
- Blynk cloud monitoring
- Over-temperature alarm using buzzer
- Portable embedded system architecture
- WiFi connectivity for remote monitoring

---

## Hardware Components

| Component | Quantity |
|------------|----------|
| ESP32 DevKit V1 | 1 |
| DS18B20 Temperature Sensor | 2 |
| IRLZ44N MOSFET | 1 |
| Nichrome Wire Heater | 1 |
| OLED SSD1306 (0.96") | 1 |
| Active Buzzer | 1 |
| 12V Power Supply | 1 |
| Resistors and Wiring | Several |

---

## System Architecture

```text
DS18B20 Sensors
        │
        ▼
     ESP32
        │
 ┌──────┼──────┐
 │      │      │
 ▼      ▼      ▼
OLED  Blynk  Buzzer
        │
        ▼
    MOSFET Driver
        │
        ▼
 Nichrome Heater
```

---

## Pin Configuration

| Device | ESP32 Pin |
|----------|-----------|
| DS18B20 | GPIO15 |
| Heater MOSFET | GPIO13 |
| Buzzer | GPIO26 |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |

---

## Working Principle

1. The DS18B20 sensors continuously measure temperature.
2. Temperature data is processed by the ESP32.
3. If the temperature falls below the target range, the heater is activated through a MOSFET driver using PWM control.
4. If the temperature exceeds the safety threshold, an alarm is triggered through the buzzer.
5. Temperature information is displayed on the OLED screen.
6. Monitoring data is transmitted to the Blynk cloud platform via WiFi.

---

## Project Structure

```text
blood-warmer-iot/
│
├── README.md
├── LICENSE
├── .gitignore
│
├── firmware/
│   └── blood_warmer.ino
│
├── hardware/
│   ├── wiring_diagram.png
│   ├── schematic.pdf
│   └── pcb_design.png
│
├── images/
│   ├── prototype.jpg
│   ├── oled_display.jpg
│   ├── blynk_dashboard.jpg
│   └── system_overview.png
│
├── docs/
│   ├── project_report.pdf
│   ├── testing_results.pdf
│   └── user_manual.pdf
│
└── results/
    ├── temperature_response.png
    ├── heating_curve.png
    └── experiment_data.csv
```

---

## Prototype

### System Prototype

![Prototype](images/prototype.jpg)

### OLED Display

![OLED](images/oled_display.jpg)

### Blynk Dashboard

![Blynk Dashboard](images/blynk_dashboard.jpg)

### Wiring Diagram

![Wiring Diagram](hardware/wiring_diagram.png)

---

## Testing Results

The system was tested to evaluate:

- Temperature monitoring performance
- Heater response characteristics
- Sensor stability
- Alarm functionality
- Remote monitoring capability

Example results include:

- Heating response curve
- Temperature stability analysis
- Sensor accuracy evaluation
- IoT communication performance

---

## Software Requirements

- Arduino IDE
- ESP32 Board Package
- Blynk Library
- DallasTemperature Library
- OneWire Library
- Adafruit SSD1306 Library
- Adafruit GFX Library

---

## Future Improvements

- PID temperature control
- Battery-powered operation
- Cloud database integration
- Mobile push notifications
- Data logging and analytics
- Medical-grade enclosure design
- Web dashboard monitoring

---

## Applications

- Biomedical engineering research
- Blood transfusion support systems
- Medical device prototyping
- Temperature control systems
- IoT healthcare applications

---

## Author

**Riza Aria Komara**

Biomedical Engineering Student

GitHub: https://github.com/USERNAME

LinkedIn: https://linkedin.com/in/YOUR-LINKEDIN

---

## License

This project is released under the MIT License.
