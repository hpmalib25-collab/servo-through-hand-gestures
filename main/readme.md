

# 🔧 ESP32 UART Servo Controller (ESP-IDF / FreeRTOS)

An ESP32 firmware project that receives servo angles over UART and drives a servo motor using the LEDC PWM peripheral. Built with **ESP-IDF** and **FreeRTOS**.

---

## 📋 Overview

The ESP32 listens on `UART0` for an ASCII integer angle value (e.g., `"90\n"`), converts it to a PWM duty cycle, and moves the servo to that angle. Designed to work with any host (PC, Raspberry Pi, etc.) sending angles over serial.

---

## 📁 File

```
└── main.c    # Full firmware: UART init, LEDC PWM init, servo control loop
```

---

## ⚙️ Hardware Requirements

| Component     | Detail                         |
|---------------|-------------------------------|
| ESP32         | Any variant                   |
| Servo Motor   | Standard 3-wire (e.g. SG90)  |
| USB Cable     | For flashing & UART comms     |

### Wiring

| Servo Wire | ESP32 Pin |
|------------|-----------|
| Signal     | GPIO 18   |
| VCC        | 5V        |
| GND        | GND       |

> ⚠️ Power the servo from an external 5V supply if it draws more current than the ESP32's 5V pin can supply.

---

## 🔩 Pin & Peripheral Configuration

| Parameter          | Value              | Description                        |
|--------------------|--------------------|------------------------------------|
| `SERVO_GPIO`       | `18`               | PWM signal output pin              |
| `UART_PORT`        | `UART_NUM_0`       | USB-Serial UART                    |
| Baud Rate          | `115200`           | Must match the sender              |
| PWM Frequency      | `50 Hz`            | Standard servo control frequency   |
| PWM Resolution     | `16-bit`           | Fine-grained duty cycle control    |
| `SERVO_MIN_DUTY`   | `1638`             | ~1ms pulse width → 0°              |
| `SERVO_MAX_DUTY`   | `8192`             | ~2ms pulse width → 180°            |

---

## 🧠 How It Works

```
UART RX  →  atoi()  →  set_servo_angle()  →  LEDC PWM  →  Servo
```

1. **`uart_init()`** — Configures `UART0` at 115200 baud, 8N1, no flow control.
2. **`servo_init()`** — Configures LEDC Timer 0 at 50 Hz (16-bit resolution) and binds it to GPIO 18.
3. **`app_main()` loop** — Reads bytes from UART, parses the integer angle with `atoi()`, and calls `set_servo_angle()`.
4. **`set_servo_angle()`** — Clamps the angle to `[0, 180]`, linearly maps it to a duty cycle, and applies it via `ledc_set_duty()` + `ledc_update_duty()`.

### Duty Cycle Formula

```
duty = SERVO_MIN_DUTY + ((SERVO_MAX_DUTY - SERVO_MIN_DUTY) * angle / 180)
```

| Angle | Duty Cycle |
|-------|------------|
| 0°    | 1638       |
| 90°   | 4915       |
| 180°  | 8192       |

---

## 🚀 Getting Started

### Prerequisites

- [ESP-IDF v4.x or v5.x](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- USB driver for your ESP32 board

### Build & Flash

```bash
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor    # Replace COMx with your port (e.g. COM3 or /dev/ttyUSB0)
```

### Expected Serial Output

```
Servo ready. Waiting for angle...
Angle: 0   -> Duty: 1638
Angle: 90  -> Duty: 4915
Angle: 180 -> Duty: 8192
```

---

## 📡 Communication Protocol

Send a plain ASCII integer followed by a newline (`\n`) over serial:

| Angle | Send       |
|-------|------------|
| 0°    | `"0\n"`    |
| 90°   | `"90\n"`   |
| 180°  | `"180\n"`  |

**Settings:** 115200 baud · 8 data bits · No parity · 1 stop bit

Example using Python:
```python
import serial
ser = serial.Serial('COMx', 115200)
ser.write(b"90\n")
```

---

## 🔧 Troubleshooting

| Problem                  | Solution                                                  |
|--------------------------|-----------------------------------------------------------|
| Servo not moving         | Check GPIO 18 wiring and ensure servo has 5V power        |
| Garbled UART data        | Verify baud rate is 115200 on both sides                  |
| Build fails              | Make sure ESP-IDF is correctly installed and sourced       |
| Port not found           | Check USB connection; install CP210x or CH340 USB drivers |
| Servo jittering          | Use an external 5V supply instead of ESP32's onboard 5V   |

---

## 📄 License

This project is open-source and free to use for personal or educational purposes.
