#include <WiFi.h>
#include "time.h"
#include <Stepper.h>

// ================= WIFI CONFIG =================
const char* ssid = "your_ssid";           // Wifi SSID
const char* password = "ssid_password";   // Wifi Passcode
IPAddress local_IP("Your_ESP32_IP");       // Static IP for ESP1 Set something nearby your IP
IPAddress gateway("Your_static_Ip");      // Routers IP - Wifi
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);        // Google DNS
IPAddress secondaryDNS(8, 8, 4, 4);      // Backup DNS
// NTP (India IST)
const char* ntpServer1 = "time.google.com";
const char* ntpServer2 = "pool.ntp.org";
const char* ntpServer3 = "time.windows.com";
const long gmtOffset_sec = 19800; // +5:30
const int daylightOffset_sec = 0;
unsigned long lastSync = 0;
const unsigned long syncInterval = 15UL * 1 * 60 * 1000;
int hour=0,minute=0,h1=0,h2=0,m1=0,m2=0;
// ================= CLOCK DIGIT CLASS =================
class ClockDigit 
{
  int8_t current_digit = 0;   // stores current position
  int8_t invert_direction = 1;
  static const int STEPS_PER_REVOLUTION = 2038;
  static const int STEPS_PER_DIGIT = 560; // adjust after calibration
  Stepper stepper;

public:
  ClockDigit(int8_t dir, int blu, int yel, int pnk, int ora)
    : invert_direction(dir),
      stepper(STEPS_PER_REVOLUTION, blu, yel, pnk, ora) {
    stepper.setSpeed(10); // RPM (slow = smooth)
  }
  // Move to target digit
  void set_to_digit(uint8_t digit) 
  {
    int delta = digit - current_digit;
    if (delta != 0) { // avoid unnecessary movement
      stepper.step(invert_direction * STEPS_PER_DIGIT * delta);
      current_digit = digit;
    }
  }
  // Force reset to 0
  void force_zero() {
    stepper.step(invert_direction * STEPS_PER_DIGIT * (-current_digit));
    current_digit = 0;
  }
};

// ================= MOTOR OBJECTS =================
// Change pins if needed

ClockDigit hour_ms(1, 16, 17, 5, 18);
ClockDigit hour_ls(-1, 19, 21, 22, 23);
ClockDigit minute_ms(1, 25, 26, 27, 14);
ClockDigit minute_ls(-1, 12, 13, 32, 33);

// ================= STARTUP ANIMATION =================
void startup() {
  Serial.println("Startup animation...");

  // Step 1: go to max values
  hour_ms.set_to_digit(2);
  hour_ls.set_to_digit(9);
  minute_ms.set_to_digit(5);
  minute_ls.set_to_digit(9);

  delay(1000);

  // Step 2: return to zero
  hour_ms.force_zero();
  hour_ls.force_zero();
  minute_ms.force_zero();
  minute_ls.force_zero();

  delay(500);

  Serial.println("Startup done.");
}

// ================= TIME FUNCTION =================
void timeSet() 
{
  struct tm timeinfo;

  // Try to read time from internal ESP clock
  if (!getLocalTime(&timeinfo)) {
    Serial.println("❌ Failed to read internal time");

    // 🔴 TURN ON ERROR LED HERE
    // digitalWrite(ERROR_LED, HIGH);

    return;
  }

  int hour = timeinfo.tm_hour;
  int minute = timeinfo.tm_min;

  // Split digits
  h1 = (hour / 10) % 3;
  h2 = hour % 10;
  m1 = (minute / 10) % 6;
  m2 = minute % 10;

  // ================= SERIAL DEBUG =================
  Serial.println("=================================");
  Serial.printf("🕒 Time: %02d:%02d\n", hour, minute);
  Serial.print("Hour Tens     (H1): "); Serial.println(h1);
  Serial.print("Hour Units    (H2): "); Serial.println(h2);
  Serial.print("Minute Tens   (M1): "); Serial.println(m1);
  Serial.print("Minute Units  (M2): "); Serial.println(m2);
  Serial.printf("🕒  Digits → %d%d:%d%d\n", h1, h2, m1, m2);
  Serial.println("=================================");
  // ================= PERIODIC NTP RESYNC =================
  if (millis() - lastSync > syncInterval) {
    Serial.println("🔄 Hourly NTP Resync...");

    syncTimeNTP();   // attempt resync
    lastSync = millis();

    // If success → LED OFF handled inside sync
    // If fail → LED ON handled inside sync
  }
}
void syncTimeNTP() 
{
  Serial.println("🔄 NTP Sync Attempt...");

  // If no WiFi → skip
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ No WiFi → Skip Sync");
    // 🔴 TURN ON ERROR LED HERE
    return; // Exit early (important)
  }

  // Request sync
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
  struct tm timeinfo;
  // Try ONLY ONCE
  if (getLocalTime(&timeinfo)) 
  {
    Serial.println("✅ NTP Sync Success");
    // 🔌 TURN OFF ERROR LED HERE
  } else 
  {
    Serial.println("❌ NTP Sync Failed (Continuing Offline)");
    // 🔴 TURN ON ERROR LED HERE
  }
}
void motorWork() 
{
  Serial.println("⚙️ Moving Motors...");
  hour_ms.set_to_digit(h1);
  hour_ls.set_to_digit(h2);
  minute_ms.set_to_digit(m1);
  minute_ls.set_to_digit(m2);
  Serial.println("✅ Movement Done\n");
}
void wifi_connect() 
{
  Serial.println("🔄 Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 5000; // 5 seconds max
  // Try for limited time only (NON-BLOCKING APPROACH)
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) 
  {
    delay(200);   // small delay, not too long
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("\n✅ WiFi Connected!");
    // 🔌 TURN OFF ERROR LED HERE
    // digitalWrite(ERROR_LED, LOW);
  } 
  else 
  {
    Serial.println("\n❌ WiFi Failed (Continuing Offline)");
    // 🔴 TURN ON ERROR LED HERE
    // digitalWrite(ERROR_LED, HIGH);
  }
}
// ================= SETUP =================
void setup() 
{
  Serial.begin(115200);
  wifi_connect();
  syncTimeNTP();
  //startup();
}

// ================= LOOP =================
int lastMinute = -1;
void loop() 
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  if (timeinfo.tm_min != lastMinute) 
  {
    lastMinute = timeinfo.tm_min;
    Serial.println("⏱ Minute changed → updating");
    timeSet();
    motorWork();
  }
}