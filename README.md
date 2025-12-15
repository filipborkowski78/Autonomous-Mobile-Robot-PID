# Autonomous Mobile Robot with PID Control

## Project Overview
This project focuses on developing a control system for a mobile robot aimed at precise line following and trajectory stability. The system integrates low-level hardware control (C++) with high-level scripting for telemetry and configuration (Python).

**Key Features:**
* **Real-time Control:** Implemented PID algorithm to stabilize movement and eliminate oscillations on curves.
* **Hybrid Architecture:**
    * **Arduino (C++):** Motor control, sensor data acquisition, and PID logic.
    * **Python:** Desktop script for remote control and parameter tuning via Bluetooth (UART).
* **Optimization:** Tuned P, I, and D coefficients to balance speed vs. stability.

## Tech Stack
* **Language:** C++, Python
* **Hardware:** Arduino, DC Motors, Bluetooth Module (HC-05/06)
* **Concepts:** Control Theory (PID), PWM, Serial Communication, UART

## How it works
The robot reads sensor data (IR sensors) to determine its position relative to the line. The PID controller calculates the error and adjusts the PWM signal for the motors in real-time. The Python script allows for dynamic adjustment of PID coefficients without recompiling the code.
