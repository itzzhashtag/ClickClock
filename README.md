<div align="center">

# ⏰ Click Clock — The Kinetic Clock

**by [Aniket Chowdhury](mailto:micro.aniket@gmail.com) (aka `#Hashtag`)**

<img src="https://img.shields.io/badge/Status-Working-brightgreen?style=for-the-badge&logo=arduino" />
<img src="https://img.shields.io/badge/Built%20with-ESP32-blue?style=for-the-badge&logo=arduino" />
<img src="https://img.shields.io/badge/Type-Mechanical%20Clock-black?style=for-the-badge" />
<img src="https://img.shields.io/badge/Control-WiFi%20%7C%20NTP-purple?style=for-the-badge" />

</div>

---

## 🎬 Project Overview

Click Clock is a **kinetic mechanical clock** that physically moves time into place using stepper motors.

Instead of pixels, this clock uses **real motion** — each digit shifts into position with a satisfying mechanical click, making time something you can *see and feel*.

---

## ✨ Highlights

* ⚙️ Fully mechanical **HH:MM display**
* 🌐 **WiFi + NTP auto time sync**
* 🧠 Runs on **ESP32 internal RTC (offline capable)**
* 🎨 **NeoPixel status LEDs**
* 🔁 Smart recovery from WiFi / NTP failures
* 🎬 Startup animation (full motion sweep)
* 🔧 Drift correction for mechanical accuracy

---

## ⚙️ How It Works

```text
Internet Time (NTP)
        ↓
     ESP32 WiFi
        ↓
 Internal RTC (keeps time offline)
        ↓
   Time Processing (HH:MM)
        ↓
 Stepper Motor Control
        ↓
 Mechanical Digit Movement
```

---

## 🧠 Core Concept

> Time is not displayed — it is *constructed*.

Each minute:

* Digits are calculated
* Motors rotate to exact positions
* Physical numbers align into current time

---

## 🔧 Hardware

* ESP32
* 28BYJ-48 Stepper Motors (×4)
* ULN2003 Driver Boards (×4)
* WS2812 / NeoPixel LEDs
* External 5V Power Supply (2A–3A recommended)
* Capacitors for stability

---

## 🔌 Wiring & Power

📄 See full guide: **`Wiring.md`**

Includes:

* Safe power design ⚡
* Capacitors & protection 🔋
* Motor wiring ⚙️
* LED safety 💡

---

## 🎨 LED Status System

| State       | Meaning           |
| ----------- | ----------------- |
| ⚪ White     | Normal operation  |
| 🔴 Red      | WiFi error        |
| 🔥 Soft Red | Running offline   |
| 🟠 Orange   | NTP failed        |
| 🟡 Yellow   | NTP retry         |
| 🟣 Purple   | Booting           |
| 🌈 Mixed    | Startup animation |

---

## 🎬 Boot Behavior

* 🟣 System boot indicator
* 🔧 Mechanical drift correction
* 🌐 WiFi connection attempt
* ⏱ Time sync (NTP)
* ⚙️ Full motor sweep (test)
* 🕒 Clock starts

---

## ⏱ Runtime Behavior

* Updates **once per minute**
* Motors move **only when needed**
* Keeps time even without internet
* Automatically resyncs when possible

---

## 🛠 Calibration

```cpp
static const int STEPS_PER_DIGIT = 560;
```

Adjust based on your mechanical build.

---

## ⚠️ Important Notes

* ❗ Do NOT power motors from ESP32
* 🔗 Common ground is mandatory
* ⚡ Use capacitors to prevent resets
* ⚙️ Stepper control is sequential (blocking)

---

## 📸 Showcase

>  images / videos here Soon

---

## 🚀 Future Ideas

* Parallel motor control (FreeRTOS)
* Web dashboard control
* Smart brightness / night mode
* Sound or haptic feedback

---
 
## 👤 Author & Contact

👨 **Name:** Aniket Chowdhury (aka Hashtag)  
📧 **Email:** [micro.aniket@gmail.com](mailto:micro.aniket@gmail.com)  
💼 **LinkedIn:** [itzz-hashtag](https://www.linkedin.com/in/itzz-hashtag/)  
🐙 **GitHub:** [itzzhashtag](https://github.com/itzzhashtag)  
📸 **Instagram:** [@itzz_hashtag](https://instagram.com/itzz_hashtag)

---

## 📜 License

This project is released under a Modified MIT License.
It is intended for personal and non-commercial use only.

🚫 Commercial use or distribution for profit is not permitted without prior written permission.
🤝 For collaboration, reuse, or licensing inquiries, please contact the author.

📄 View Full License <br>
[![License: MIT–NC](https://img.shields.io/badge/license-MIT--NC-blue.svg)](./LICENSE)

---

## ⭐ Support

If you like this project:

* ⭐ Star the repo
* 🍴 Fork it
* 🛠 Build your own version

---

## ❤️ Acknowledgements

This is a solo passion project, built with countless nights of tinkering, testing, and debugging.  
If you find it useful or inspiring, feel free to ⭐ the repository or connect with me on social media!

---

## 🔥 Final Thought

> This isn’t just a clock.
> It’s a **mechanical visualization of time itself.**

</div>
