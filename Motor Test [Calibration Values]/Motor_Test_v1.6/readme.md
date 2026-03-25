==================== ESP32 + 4x 28BYJ-48 + ULN2003 WIRING ====================

-------------------- 🔋 POWER --------------------

External 5V Supply (+)  →  VCC of ALL ULN2003 boards  
External 5V Supply (GND)→  GND of ALL ULN2003 boards  

ESP32 GND               →  SAME GND (COMMON GROUND)  

⚠️ DO NOT power motors from ESP32 5V pin

---

-------------------- ⚙️ MOTOR 1 (HOUR TENS - H1) --------------------

ULN2003 IN1 → ESP32 GPIO 33  
ULN2003 IN2 → ESP32 GPIO 32  
ULN2003 IN3 → ESP32 GPIO 13  
ULN2003 IN4 → ESP32 GPIO 12  

Motor plugs directly into ULN2003 board

---

-------------------- ⚙️ MOTOR 2 (HOUR UNITS - H2) --------------------

ULN2003 IN1 → ESP32 GPIO 14  
ULN2003 IN2 → ESP32 GPIO 27  
ULN2003 IN3 → ESP32 GPIO 26  
ULN2003 IN4 → ESP32 GPIO 25  

Motor plugs directly into ULN2003 board

---

-------------------- ⚙️ MOTOR 3 (MINUTE TENS - M1) --------------------

ULN2003 IN1 → ESP32 GPIO 23  
ULN2003 IN2 → ESP32 GPIO 22  
ULN2003 IN3 → ESP32 GPIO 21  
ULN2003 IN4 → ESP32 GPIO 19  

Motor plugs directly into ULN2003 board

---

-------------------- ⚙️ MOTOR 4 (MINUTE UNITS - M2) --------------------

ULN2003 IN1 → ESP32 GPIO 18  
ULN2003 IN2 → ESP32 GPIO 5  
ULN2003 IN3 → ESP32 GPIO 17  
ULN2003 IN4 → ESP32 GPIO 16  

Motor plugs directly into ULN2003 board

---

-------------------- ⚠️ IMPORTANT NOTES --------------------

✔ All grounds MUST be common (ESP + Power + ULN2003)  
✔ Use 5V 2A–3A power supply minimum  
✔ Add 100µF–470µF capacitor across each ULN2003 VCC-GND (recommended)  
✔ Keep wiring short & clean to avoid noise  
✔ If motor vibrates only → swap IN1–IN4 order  

==========================================================================
