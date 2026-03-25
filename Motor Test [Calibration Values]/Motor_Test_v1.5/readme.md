
==================== ESP32 + 4x 28BYJ-48 + ULN2003 WIRING ====================

-------------------- 🔋 POWER --------------------
  
External 5V Supply (+)  →  VCC of ALL ULN2003 boards  
External 5V Supply (GND)→  GND of ALL ULN2003 boards  

ESP32 GND               →  SAME GND (COMMON GROUND)  

⚠️ DO NOT power motors from ESP32 5V pin

---

-------------------- ⚙️ MOTOR 1 (HOUR TENS) --------------------

ULN2003 IN1 → ESP32 GPIO 16  
ULN2003 IN2 → ESP32 GPIO 17  
ULN2003 IN3 → ESP32 GPIO 5  
ULN2003 IN4 → ESP32 GPIO 18  

Motor plugs directly into ULN2003 board

---

-------------------- ⚙️ MOTOR 2 (HOUR UNITS) --------------------

ULN2003 IN1 → ESP32 GPIO 19  
ULN2003 IN2 → ESP32 GPIO 21  
ULN2003 IN3 → ESP32 GPIO 22  
ULN2003 IN4 → ESP32 GPIO 23  

Motor plugs directly into ULN2003 board

---

-------------------- ⚙️ MOTOR 3 (MINUTE TENS) --------------------

ULN2003 IN1 → ESP32 GPIO 25  
ULN2003 IN2 → ESP32 GPIO 26  
ULN2003 IN3 → ESP32 GPIO 27  
ULN2003 IN4 → ESP32 GPIO 14  
  
Motor plugs directly into ULN2003 board  

---

-------------------- ⚙️ MOTOR 4 (MINUTE UNITS) --------------------

ULN2003 IN1 → ESP32 GPIO 12  
ULN2003 IN2 → ESP32 GPIO 13  
ULN2003 IN3 → ESP32 GPIO 32  
ULN2003 IN4 → ESP32 GPIO 33  

Motor plugs directly into ULN2003 board  

---

-------------------- ⚠️ IMPORTANT NOTES --------------------

✔ All grounds MUST be common (ESP + Power + ULN2003)  
✔ Use 5V 2A–3A power supply minimum  
✔ Keep wiring clean to avoid signal noise  
✔ If motor vibrates only → check IN1–IN4 order  

==========================================================================
