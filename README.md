# IoT-Based Bidirectional Occupancy Counter & Energy-Saving System

An independent undergraduate Computer Engineering capstone project at Michael Okpara University of Agriculture Umudike, designed to monitor building occupancy in real time, enforce crowd-capacity thresholds, and dynamically regulate electrical loads to reduce energy waste.

## System Architecture & Hardware Stack
- **Microcontroller:** NodeMCU ESP8266 (programmed via Arduino IDE in C++)
- **Sensing:** Paired Infrared (IR) sensors for directional entry/exit detection
- **Local Interface:** 0.96" I2C OLED display (SSD1306) for real-time on-site occupancy readout
- **Cloud & Remote Monitoring:** Blynk IoT platform for real-time, remote occupancy tracking, and capacity alerts
- **Actuation & Load Control:** Single-channel relay module switching an AC lighting load based on occupancy state
- **Power:** Regulated DC power supply unit

## Software & Required Arduino Libraries
To compile and flash the firmware in Arduino IDE, install the **ESP8266 Board Package** along with the following libraries via the Arduino Library Manager:
- **`Blynk`**
- **`Adafruit_SSD1306`** – Driver for the 0.96" I2C OLED display
- **`Adafruit_GFX`** – Core graphics library required for text and UI rendering on the SSD1306 OLED
- **`Wire`** & **`ESP8266WiFi`** *(built-in)* – I2C communication and Wi-Fi connectivity

## Core Features
- **Bidirectional State-Machine Logic:** Evaluates sequential IR beam-break triggers to distinguish entries from exits and maintain an accurate net occupancy count.
- **Dual Local & Cloud Analysis:** Displays live counts locally on the SSD1306 OLED while streaming telemetry to a remote observer via Blynk IoT for crowd-flow management.
- **Automated Energy Optimization:** Energizes the relay-controlled lighting load only when occupancy is greater than zero, cutting idle power consumption.

## Experimental Evaluation & Research Motivation
- **Linear Energy Cost Savings:** Empirical testing confirmed a direct linear reduction in energy consumption costs proportional to the duration of building occupancy.
- **Identified Sensor Limitations:** Controlled stress testing revealed accuracy degradation under high ambient infrared interference, physical occlusion, and simultaneous bidirectional entry/exit.

## Repository Structure
- `firmware/` – ESP8266 Arduino (`.ino`) source code for directional counting logic, OLED rendering, relay control, and Blynk IoT integration
- `diagrams/` – Circuit schematics and wiring diagrams
- `docs/` – Evaluation notes
- `results/` – Analytics from Blynk App
