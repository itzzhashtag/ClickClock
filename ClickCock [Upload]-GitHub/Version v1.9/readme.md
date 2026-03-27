# 🕰️ Click Clock — ESP32 Mechanical Stepper Clock

> ⚙️ A kinetic, WiFi-synced clock that brings time to life using motors, memory, and precision engineering.

---

## 📌 Overview

**Click Clock** is a 4-digit mechanical clock powered by an ESP32 that displays time in **HH:MM (24-hour format)** using stepper motors.

Each digit is physically driven using a **28BYJ-48 stepper motor + ULN2003 driver**, creating a satisfying mechanical movement for every minute update.

Time is synced via **NTP (Internet Time)** and maintained using the ESP32’s internal RTC — so it keeps running even when WiFi drops.

This version introduces a **Flash Memory Recovery System**, allowing the clock to recover its position after sudden power loss and safely reset itself to a known state.

---

## ✨ Features

* 🌐 WiFi + NTP Time Sync
* ⏱ Real-time clock using ESP32 internal RTC
* ⚙️ 4 Stepper Motors (Mechanical Digits)
* 🎨 NeoPixel LED Status Indicators
* 🔁 Auto Recovery (WiFi / NTP failures)
* 💾 Flash Memory Position Save & Recovery
* 🚫 No external RTC required
* 🧠 Smart first-boot sync logic
* 🔋 Offline time continuity
* 🎬 Startup animation (full range test)

---

## 🆕 What's New (This Version)

### 🚀 Major Improvements

* 💾 Flash Memory Time Save using ESP32 NVS
* 🔁 Power-loss Recovery System
* 🔄 Reverse Homing to 00:00 on Boot
* 🧠 Smart skip if no recovery needed
* 🧹 Auto-clear memory after recovery
* ⚡ Stable restart behavior
* 🔧 Better motor state consistency

---

### ⚙️ New Functions

* `saveTimeToFlash()` → Saves HH:MM into flash memory  
* `loadTimeFromFlash()` → Reads last saved time  
* `recoverToZero()` → Moves clock back to 00:00 after power loss  
* `boot_correct()` → Raw motor movement for recovery  

---

## 🧠 Working Principle

Internet (NTP)
↓
ESP32 WiFi
↓
Internal RTC (keeps time)
↓
timeSet() → Extract HH:MM digits
↓
motorWork() → Move stepper motors
↓
saveTimeToFlash() → Store last position
↓
Power Loss Happens ⚡
↓
Boot → recoverToZero()
↓
Clock moves backward to 00:00
↓
Normal startup resumes


---

## 🔄 Power Loss Recovery Logic

### 🧩 How it Works

1. Every minute → time is saved as `HHMM` in flash  
2. If power is lost → last position is remembered  
3. On reboot:
   - If saved time ≠ `0000`
   - Clock moves backward to 00:00
   - Memory is cleared after recovery  
4. System starts fresh from zero  

---

### 📌 Example Behavior

| Power Loss At | Restart Action |
|--------------|--------------|
| 11:22        | Reverse → 00:00 |
| 03:05        | Reverse → 00:00 |
| 00:00        | No recovery |

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

Refer to `Wiring.md` for full details including:

* Power distribution
* Capacitor placement
* Stepper motor connections
* LED wiring safety

---

## 🎨 LED Status Indicators

| Mode | Color       | Meaning           |
| ---- | ----------- | ----------------- |
| 0    | White       | Normal operation  |
| 3    | Red         | WiFi error        |
| 6    | Soft Red    | Offline running   |
| 2    | Orange      | NTP failed        |
| 8    | Yellow      | NTP retry         |
| 7    | Purple      | Boot              |
| 4/5  | Mixed       | Startup animation |

---

## 🔄 Boot Sequence

1. LED shows boot indicator  
2. Motors apply drift correction  
3. Flash recovery runs (if needed)  
4. WiFi connection attempt  
5. First-time NTP sync (blocking)  
6. Startup animation (0 → max → 0)  
7. Clock begins operation  

---

## ⏱ Runtime Behavior

* Updates once per minute  
* Motors move only when time changes  
* Time is saved every minute  
* Periodic NTP resync  
* Continues running if WiFi drops  

---

## 🛠 Calibration

 
static const int STEPS_PER_DIGIT = 1120; // Adjust Accordingly


---

##  ⚠️ Important Notes

Do NOT power motors directly from ESP32
All grounds must be common
Use capacitors to prevent voltage drops
Stepper control is sequential (blocking)
Flash memory has limited write cycles → avoid excessive writes
Assumes motors do not slip during operation

---

## 📸 Demo / Showcase

Mechanical stepping with visible motion — adds physical feedback to timekeeping.

(Soon images/videos here)

---

## 🚀 Future Improvements

Parallel motor movement using FreeRTOS
Web dashboard control
Alarm system
Limit switch for true zero calibration
Step-based absolute positioning
Auto brightness / night mode

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
