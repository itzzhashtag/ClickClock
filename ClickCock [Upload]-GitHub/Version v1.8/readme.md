# 🕰️ Click Clock — ESP32 Mechanical Stepper Clock

> ⚙️ A kinetic, WiFi-synced clock that brings time to life using motors, light, and precision engineering.

---

## 📌 Overview

**Click Clock** is a 4-digit mechanical clock powered by an ESP32 that displays time in **HH:MM (24-hour format)** using stepper motors.

Each digit is physically driven using a **28BYJ-48 stepper motor + ULN2003 driver**, creating a satisfying mechanical movement for every minute update.

Time is synced via **NTP (Internet Time)** and maintained using the ESP32’s internal RTC — so it keeps running even when WiFi drops.

---

## ✨ Features

* 🌐 **WiFi + NTP Time Sync**
* ⏱ **Real-time clock using ESP32 internal RTC**
* ⚙️ **4 Stepper Motors (Mechanical Digits)**
* 🎨 **NeoPixel LED Status Indicators**
* 🔁 **Auto Recovery (WiFi / NTP failures)**
* 🚫 **No external RTC required**
* 🧠 **Smart first-boot sync logic**
* 🔋 **Offline time continuity**
* 🎬 **Startup animation (full range test)**

---

## 🆕 What's New (This Version)

### 🚀 Major Improvements

* ✅ **Removed external RTC (DS3231 no longer needed)**
* ✅ **Full NTP-based time system**
* ✅ **Offline fallback using internal RTC**
* ✅ **First-time blocking sync (WiFi + NTP)**
* ✅ **Non-blocking resync during runtime**
* ✅ **Improved power stability handling**
* ✅ **Boot drift correction added**
* ✅ **Cleaner modular architecture**

---

### ⚙️ New Functions

* `syncTimeNTP()` → Smart sync logic (boot + runtime)
* `timeSet()` → Handles time parsing & periodic sync
* `motorWork()` → Dedicated motor control
* `ledTask()` → Central LED status manager
* `boot_correct()` → Fix mechanical drift at startup

---

## 🧠 Working Principle

```
Internet (NTP)
      ↓
ESP32 WiFi
      ↓
Internal RTC (keeps time even offline)
      ↓
timeSet() → Extract HH:MM digits
      ↓
motorWork() → Move stepper motors
      ↓
Physical digit display
```

---

## ⚙️ Hardware Required

* ESP32
* 4 × 28BYJ-48 Stepper Motors
* 4 × ULN2003 Driver Boards
* WS2812 / NeoPixel LEDs (x4)
* 5V 2A–3A Power Supply
* Capacitors (1000µF recommended)
* Jumper wires

---

## 🔌 Wiring

👉 Full wiring guide available in:
📄 **Wiring.md**

Includes:

* Power design ⚡
* Capacitors & protection 🔋
* Motor connections ⚙️
* LED safety 💡

---

## 🎨 LED Status Indicators

| Mode | Color       | Meaning           |
| ---- | ----------- | ----------------- |
| 0    | ⚪ White     | Normal operation  |
| 3    | 🔴 Red      | WiFi error        |
| 6    | 🔥 Soft Red | Offline (running) |
| 2    | 🟠 Orange   | NTP failed        |
| 8    | 🟡 Yellow   | NTP retry         |
| 7    | 🟣 Purple   | Boot              |
| 4/5  | 🌈 Mixed    | Startup animation |

---

## 🔄 Boot Sequence

1. 🟣 LED Boot Indicator
2. 🔧 Drift correction (motors calibrated)
3. 📡 WiFi connection
4. 🌐 NTP sync (blocking first time)
5. 🎬 Full motor test (0 → max → 0)
6. 🕒 Clock starts

---

## ⏱ Runtime Behavior

* Updates **once per minute**
* Motors move **only when time changes**
* NTP resync happens **periodically**
* If WiFi fails → continues running (no freeze)

---

## 🛠 Calibration

```cpp
static const int STEPS_PER_DIGIT = 560;
```

🔧 Adjust this value based on your mechanical design.

---

## ⚠️ Important Notes

* ❗ Do NOT power motors from ESP32
* 🔗 All grounds must be common
* ⚡ Use proper capacitors to avoid resets
* 🔁 Stepper library is blocking (sequential movement)

---

## 📸 Demo / Showcase

> *(Add your images / videos here — highly recommended)*

---

## 🚀 Future Improvements

* 🔄 Parallel motor movement (FreeRTOS)
* 📱 Web dashboard control
* ⏰ Alarm / notifications
* 🎵 Sound feedback
* 🧠 Smart brightness / night mode

---

## 🤝 Credits

Built with passion by an engineer who got bored and decided to build time itself ⚙️🔥

---

## ⭐ Support

If you like this project:

* ⭐ Star the repo
* 🍴 Fork it
* 🛠 Modify & build your own

---

## 💀 Final Thought

> Time is invisible.
> Until you build it.

---
