# 🔌 ESP32 + 4× 28BYJ-48 + ULN2003 — SAFE WIRING GUIDE

> ⚙️ Reliable • 🔥 Safe • 🧠 Noise-Protected Power Design

---

# 🔋 POWER SYSTEM (VERY IMPORTANT)

## ✅ Recommended Setup

```
12V Adapter (3A–5A)
        ↓
   Buck Converter (LM2596)
        ↓
      5V Output
```

---

## 🔌 Connections

```
5V (+)  → VCC of ALL ULN2003 boards
5V (+)  → ESP32 5V (VIN pin)

GND     → ULN2003 GND
GND     → ESP32 GND  (COMMON GROUND)
```

---

## ⚠️ NEVER DO THIS

❌ DO NOT power motors from ESP32
❌ DO NOT use USB power for motors
❌ DO NOT skip common ground

---

# ⚡ CURRENT REQUIREMENT

| Component | Current    |
| --------- | ---------- |
| 1 Motor   | ~200–250mA |
| 4 Motors  | ~1A peak   |
| ESP32     | ~300mA     |

👉 **Minimum PSU: 5V 2A (recommended 3A for safety)**

---

# 🧠 NOISE & PROTECTION (VERY IMPORTANT)

## 🔹 1. BULK CAPACITOR (MAIN FILTER)

👉 Place across **5V and GND (near motors)**

```
+5V ───||─── GND
       1000µF / 16V
```

✔ Absorbs motor spikes
✔ Prevents ESP32 resets

---

## 🔹 2. LOCAL CAPACITORS (PER DRIVER)

👉 Near each ULN2003 board:

```
+5V ───||─── GND
       100µF
```

✔ Smooths local noise
✔ Improves motor stability

---

## 🔹 3. ESP32 STABILITY CAP

👉 Near ESP32:

```
5V ───||─── GND
      470µF
```

✔ Prevents WiFi reset spikes

---

# 💡 LED (WS2812) PROTECTION

## 🔹 DATA LINE RESISTOR (MANDATORY)

```
ESP32 GPIO 4 ──[330Ω]──→ LED DIN
```

✔ Protects first LED
✔ Prevents signal ringing

---

## 🔹 LED POWER CAPACITOR

```
5V ───||─── GND
      1000µF
```

✔ Prevents LED flicker

---

# ⚙️ MOTOR CONNECTIONS

---

## 🟦 MOTOR 1 — HOUR TENS

```
ULN2003 IN1 → GPIO 16
ULN2003 IN2 → GPIO 17
ULN2003 IN3 → GPIO 5
ULN2003 IN4 → GPIO 18
```

---

## 🟦 MOTOR 2 — HOUR UNITS

```
ULN2003 IN1 → GPIO 19
ULN2003 IN2 → GPIO 21
ULN2003 IN3 → GPIO 22
ULN2003 IN4 → GPIO 23
```

---

## 🟦 MOTOR 3 — MINUTE TENS

```
ULN2003 IN1 → GPIO 25
ULN2003 IN2 → GPIO 26
ULN2003 IN3 → GPIO 27
ULN2003 IN4 → GPIO 14
```

---

## 🟦 MOTOR 4 — MINUTE UNITS

```
ULN2003 IN1 → GPIO 12
ULN2003 IN2 → GPIO 13
ULN2003 IN3 → GPIO 32
ULN2003 IN4 → GPIO 33
```

---

✔ Motors plug directly into ULN2003 boards
✔ ULN2003 already has flyback diodes (safe for coils)

---

# 🧠 SIGNAL + POWER LAYOUT (IMPORTANT)

## ✅ Best Practice Layout

```
[Power Supply]
      ↓
[Bulk Capacitor]
      ↓
 +5V BUS  ────────────────→ ULN2003 (x4)
      │
      ├────────→ ESP32
      │
      └─[330Ω]→ LED Strip
```

---

# ⚠️ CRITICAL SAFETY NOTES

✔ Always use **common ground**
✔ Keep motor wires **short**
✔ Keep LED data wire away from motor wires
✔ Use **thick wires for power (18–22 AWG)**
✔ Check polarity before powering ON

---

# 🔥 TROUBLESHOOTING

## ❌ Motors vibrating only

→ Wrong IN1–IN4 order

## ❌ ESP32 restarting

→ Add bigger capacitor (1000µF+)

## ❌ LEDs flickering

→ Add 330Ω resistor + capacitor

## ❌ WiFi disconnects randomly

→ Power noise → improve grounding + caps

---

# 🚀 FINAL CHECKLIST

* [ ] Common GND connected
* [ ] 5V stable supply (≥2A)
* [ ] 1000µF capacitor installed
* [ ] 330Ω resistor on LED data
* [ ] Motors NOT powered from ESP32

---

# 💀 ENGINEER TIP

> Motors create noise.
> Capacitors = stability.
> Good wiring = no debugging later.

---

# 🎯 RESULT

✔ Stable WiFi
✔ Smooth motor movement
✔ No resets
✔ Clean LED output

---
