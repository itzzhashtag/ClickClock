# ⏰ Click Clock — ESP32 Stepper Digit Clock

> ⚙️ A fully mechanical **HH:MM clock** powered by **ESP32 + Stepper Motors + WiFi (NTP)**
> 🌐 Self-syncing, visually satisfying, and engineered for precision

---

## 🎬 Demo Preview

✨ Mechanical digits move every minute
🌈 Status LEDs indicate system state
🌐 Time synced automatically via internet

---

## 🧠 What This Project Does

This project is a **4-digit mechanical clock** that displays time in:

```
HH : MM  (24-hour format)
```

Each digit is driven by a **28BYJ-48 stepper motor**, creating a satisfying **click-based mechanical movement**.

---

## ⚙️ Features

* ⏱ **Real-time clock using NTP (Internet time)**
* 🌐 **WiFi auto-connect with retry logic**
* 🔁 **Auto-resync every ~1 minute**
* ⚡ **Works offline after first sync**
* 🎨 **RGB LED status indicators**
* 🔧 **Startup self-test animation**
* 🧠 **State-based error handling (WiFi vs NTP)**

---

## 🧩 Hardware Required

| Component                                | Quantity |
| ---------------------------------------- | -------- |
| ESP32                                    | 1        |
| 28BYJ-48 Stepper Motor (12V recommended) | 4        |
| ULN2003 Driver Board                     | 4        |
| WS2812 / NeoPixel LEDs                   | 4        |
| 12V Power Supply                         | 1        |
| Buck Converter (12V → 5V for ESP)        | 1        |
| Wires + Frame/3D Printed Parts           | —        |

---

## 🔌 Wiring Overview

### 🧠 Motors → ESP32 (via ULN2003)

| Digit | GPIO Pins      |
| ----- | -------------- |
| H1    | 16, 17, 5, 18  |
| H2    | 19, 21, 22, 23 |
| M1    | 25, 26, 27, 14 |
| M2    | 12, 13, 32, 33 |

---

### 💡 LED Strip

| Component   | GPIO   |
| ----------- | ------ |
| WS2812 Data | GPIO 4 |

---

⚠️ **Important:**

* Use **external 12V supply for motors**
* Common GND between ESP32 & drivers
* DO NOT power motors from ESP32

---

## 🌐 Working Principle

### 1️⃣ First Boot Logic

* 🔄 Keeps searching for **WiFi** until connected
* 🔄 Keeps syncing **NTP time** until success
* ✅ Only then starts clock

---

### 2️⃣ Normal Operation

* 🕒 Time stored in ESP32 internal RTC
* 🔁 Periodic NTP resync
* ❌ If WiFi lost → continues running (no freeze)

---

### 3️⃣ Motor Control

Each digit moves based on:

```
Digit → Steps → Motor Rotation
```

* 1 digit ≈ 560 steps
* Full rotation ≈ 2038 steps

---

### 4️⃣ LED Status System

| Mode           | Meaning                 |
| -------------- | ----------------------- |
| ⚪ White        | Normal                  |
| 🔴 Red         | WiFi error              |
| 🟡 Yellow      | NTP error               |
| 🟠 Orange      | Sync failed but running |
| 🟣 Purple      | Booting                 |
| 🔵 Cyan        | Reserved                |
| 🌈 Multi-color | Startup animation       |

---

## 🎬 Startup Sequence

1. 🟣 Boot LED
2. 🌐 WiFi connect
3. ⏱ NTP sync
4. 🎨 Motor animation:

   * Move to max
   * Return to zero
5. ⚪ Ready

---

## 🧠 Software Architecture

```
setup()
 ├── wifi_connect()
 ├── syncTimeNTP()
 ├── startup()

loop()
 ├── timeSet()
 ├── motorWork()
```

---

## 🔄 Time Flow

```
NTP → ESP32 RTC → Digit Split → Stepper Motors
```

---

## ⚡ Performance Notes

* Motors update **once per minute** (low wear)
* System runs even without internet after sync
* Blocking logic used intentionally for reliability at boot

---

## 🛠 Calibration

```cpp
STEPS_PER_DIGIT = 560;
```

👉 Adjust this value based on your gear ratio / build

---

## 💀 Known Limitations

* Uses blocking stepper control (sequential movement)
* Requires stable power supply for motors
* No manual time fallback (yet)

---

## 🚀 Future Improvements

* 🔄 Non-blocking motor control (AccelStepper)
* 📱 Web interface for control
* 🧭 Auto-calibration using sensors
* 🌈 Smooth LED animations
* 🔋 Battery backup

---

## 👨‍🔧 Built By

**Aniket Chowdhury**
⚙️ Automatic Engineer × Artist

---

## ⭐ If you like this project

Give it a ⭐ on GitHub — it helps a lot!

---

## 🔥 Final Thought

> This isn’t just a clock.
> It’s a **mechanical visualization of time itself.**
