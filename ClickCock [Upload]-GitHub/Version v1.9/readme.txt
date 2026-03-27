# 🕰️ Click Clock — ESP32 Mechanical Stepper Clock

> ⚙️ A kinetic, WiFi-synced clock that brings time to life using motors, memory, and precision engineering.

---

## 📌 Overview

**Click Clock** is a 4-digit mechanical clock powered by an ESP32 that displays time in **HH:MM (24-hour format)** using stepper motors.

Each digit is physically driven using a **28BYJ-48 stepper motor + ULN2003 driver**, creating a satisfying mechanical movement for every minute update.

Time is synced via **NTP (Internet Time)** and maintained using the ESP32’s internal RTC — so it keeps running even when WiFi drops.

Now enhanced with **Flash Memory Recovery**, the clock can intelligently recover its position after unexpected power loss.

---

## ✨ Features

* 🌐 **WiFi + NTP Time Sync**
* ⏱ **Real-time clock using ESP32 internal RTC**
* ⚙️ **4 Stepper Motors (Mechanical Digits)**
* 🎨 **NeoPixel LED Status Indicators**
* 🔁 **Auto Recovery (WiFi / NTP failures)**
* 💾 **Flash Memory Position Save & Recovery**
* 🚫 **No external RTC required**
* 🧠 **Smart first-boot sync logic**
* 🔋 **Offline time continuity**
* 🎬 **Startup animation (full range test)**

---

## 🆕 What's New (This Version)

### 🚀 Major Improvements

* 💾 **Flash Memory Time Save (NVS)**
* 🔁 **Power-loss Recovery System**
* 🔄 **Reverse Homing to 00:00 on Boot**
* 🧠 **State-aware startup (skips recovery if not needed)**
* 🧹 **Auto-clear memory after recovery**
* ⚡ **More reliable restart behavior**
* 🔧 **Improved motor position consistency**

---

### ⚙️ New Functions

* `saveTimeToFlash()` → Saves HH:MM into flash memory  
* `loadTimeFromFlash()` → Reads last saved time  
* `recoverToZero()` → Moves clock back to 00:00 after power loss  
* `boot_correct()` → Raw motor movement (used for recovery)

---

## 🧠 Working Principle
