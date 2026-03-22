![IMG_20260322_134337](https://github.com/user-attachments/assets/4cc6937b-a7b9-47d6-954c-af7812b310cb)
![IMG_20260322_134309](https://github.com/user-attachments/assets/822617b6-2cc1-4f84-b161-1c6c972d3359)

VIDEO DEMONSTRATION (X2 SPEED)
https://github.com/user-attachments/assets/73d244dd-04bb-4b1c-905a-7a765b790c7e

Advanced ESP32 IoT Robotics Platform
Overview
This repository contains the firmware for an advanced, autonomous IoT robotics platform built on the ESP32 (ESP-IDF). The project demonstrates a production-ready approach to embedded software engineering, combining real-time operating system (FreeRTOS) principles, control theory, sensor fusion, and secure cloud connectivity.

Instead of a simple "super-loop," this system uses a heavily decoupled, multi-tasking architecture to ensure high reliability, precise motor control, and seamless Over-The-Air (OTA) updates.
Core Architecture & FreeRTOS

The system is built on FreeRTOS, ensuring deterministic behavior and strict resource management:

    Task Management: Dedicated initialization and execution tasks for sensors, motor control, networking, and UI.

    Inter-Task Communication: Extensive use of Queues for safely passing telemetry and commands between tasks without data races.

    Synchronization: Event Groups manage global application states (e.g., WIFI_CONNECTED, MQTT_READY), allowing tasks to block efficiently without CPU polling.

    Resource Protection: Mutexes protect shared buses (like I2C for the OLED and IMU), while Critical Sections are used to protect fast Interrupt Service Routines (ISR) from scheduler preemptions.

Control Theory & Motor Control

    PID Controller: A fully implemented Proportional-Integral-Derivative controller for precise speed and distance maintenance.
    u(t)=Kp​e(t)+Ki​∫e(t)dt+Kd​dtde(t)​

    Hardware PWM (MCPWM): Utilizes the ESP32's dedicated Motor Control PWM peripheral for the DC motors, offloading PWM generation from the CPU and allowing exact control over duty cycles and directional logic.

    Servo Kinematics: Independent servo motor control for steering or sensor panning, mapped to exact angles.

Sensor Fusion & Navigation

    IMU (Accelerometer & Gyroscope): Raw data extraction, calibration, and normalization to standard units (g and ∘/s).

    Complementary Filter: Combines high-frequency gyroscope data with stable, low-frequency accelerometer data to calculate accurate tilt/pitch angles while eliminating drift and motor vibration noise:
    θ=α×(θ+ω×dt)+(1−α)×a

    Distance Logic: Ultrasonic/ToF distance measurement utilizing the hardware GPTimer. Microsecond-accurate pulse timing is handled via Interrupts and Callbacks, ensuring millimeter precision completely independent of the RTOS tick rate.
IoT, Cloud & Security

    Secure MQTT (MQTTS): Bidirectional telemetry and command execution via HiveMQ using TLS/SSL encryption.

    Time Synchronization: Global time sync via SNTP (esp_sntp_setoperatingmode) to validate X.509 certificates and timestamp logs accurately.

    OTA Updates (Over-The-Air): Automated firmware deployment directly from GitHub Releases via HTTPS.

    Custom Partition Table: Safe memory layout featuring Factory, OTA_0, and OTA_1 partitions, allowing seamless rollbacks in case of a corrupted update.

Debugging & HMI (Human-Machine Interface)

    Smart Logging: Dynamic logging system that streams logs via UDP to a localhost for wireless debugging. If Wi-Fi is disconnected, it automatically falls back to the physical UART interface.

    Core Dump Configuration: In the event of a fatal crash or panic, register states and memory are dumped to the Flash. This allows post-mortem debugging to pinpoint the exact line of code that caused the fault.

    OLED Display: Local HMI via I2C. The screen provides real-time diagnostic data including IP address, Wi-Fi/MQTT connection status, current state machine mode, and live sensor telemetry.
