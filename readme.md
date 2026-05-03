# 🤖 Hand Gesture Servo Control

Control a servo motor in real-time using hand gestures detected via a webcam. Built with Python + MediaPipe on the PC side and ESP32 (FreeRTOS) on the hardware side, communicating over UART/Serial.

---

## 📁 Project Structure

```
├── gesture_servo.py   # PC-side: hand detection & serial communication
└── main.c             # ESP32-side: UART receive & servo PWM control
```

---

## 🧠 How It Works

```
Webcam → MediaPipe (finger count) → Serial/UART → ESP32 → Servo Motor
```

1. The Python script captures webcam frames and uses **MediaPipe** to detect hand landmarks.
2. It counts the number of extended fingers and maps that to a servo angle:
   - **0 fingers** → `0°`
   - **1–2 fingers** → `90°`
   - **3–5 fingers** → `180°`
3. The angle is sent as a string over **serial (UART)** only when it changes.
4. The ESP32 reads the angle via UART and drives the servo using **LEDC PWM**.

---

## 🛠️ Hardware Requirements

| Component        | Details                        |
|------------------|-------------------------------|
| ESP32            | Any variant                   |
| Servo Motor      | Standard 3-wire (e.g. SG90)  |
| USB Cable        | ESP32 ↔ PC                   |
| Webcam           | USB or built-in               |

**Wiring:**

| Servo Wire | ESP32 Pin |
|------------|-----------|
| Signal     | GPIO 18   |
| VCC        | 5V        |
| GND        | GND       |

---

## 💻 Software Requirements

### PC Side (Python)

- Python 3.x
- OpenCV
- MediaPipe
- PySerial

Install dependencies:
```bash
pip install opencv-python mediapipe pyserial
```

### ESP32 Side (C / ESP-IDF)

- ESP-IDF (v4.x or v5.x)
- FreeRTOS (included with ESP-IDF)
- Drivers used: `driver/uart.h`, `driver/ledc.h`

---

## ⚙️ Configuration

### Python — `gesture_servo.py`

Update the COM port to match your system:

```python
ser = serial.Serial('COM12', 115200)  # Windows
# ser = serial.Serial('/dev/ttyUSB0', 115200)  # Linux/Mac
```

### ESP32 — `main.c`

| Parameter        | Value     | Description                  |
|-----------------|-----------|------------------------------|
| `UART_PORT`     | `UART_NUM_0` | USB-Serial UART port      |
| `SERVO_GPIO`    | `18`      | GPIO pin for servo signal    |
| `Baud Rate`     | `115200`  | Must match Python script     |
| `SERVO_MIN_DUTY`| `1638`    | ~1ms pulse (~0°)             |
| `SERVO_MAX_DUTY`| `8192`    | ~2ms pulse (~180°)           |
| PWM Frequency   | `50 Hz`   | Standard servo frequency     |
| PWM Resolution  | `16-bit`  | For fine duty cycle control  |

---

## 🚀 Getting Started

### 1. Flash the ESP32

```bash
idf.py build
idf.py -p COMx flash monitor   # Replace COMx with your port
```

### 2. Run the Python Script

```bash
python gesture_servo.py
```

### 3. Control the Servo

Show your hand in front of the webcam:

| Gesture         | Fingers | Servo Angle |
|-----------------|---------|-------------|
| Fist            | 0       | 0°          |
| 1 or 2 fingers  | 1–2     | 90°         |
| 3+ fingers      | 3–5     | 180°        |

Press **`ESC`** to quit the application.

---

## 📡 Communication Protocol

- **Interface:** UART (USB-Serial)
- **Baud Rate:** 115200
- **Format:** ASCII integer followed by newline (`"90\n"`)
- **Direction:** PC → ESP32 (one-way)
- **Optimization:** Angle is only transmitted when it changes (no redundant sends)

---

## 🔧 Troubleshooting

| Problem | Solution |
|---|---|
| Serial port not found | Check Device Manager (Windows) or `ls /dev/tty*` (Linux/Mac) and update the COM port in the script |
| Servo not moving | Verify GPIO 18 wiring and that the servo has adequate power (5V) |
| Hand not detected | Ensure good lighting; keep your hand fully visible to the webcam |
| `ModuleNotFoundError` | Run `pip install opencv-python mediapipe pyserial` |
| ESP32 not responding | Re-flash firmware; check baud rate matches on both sides |

---

## 📄 License

This project is open-source. Feel free to modify and use it for personal or educational purposes.
