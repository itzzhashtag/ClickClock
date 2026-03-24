 
#include <WiFi.h>
#include "time.h"
#include <Stepper.h>
#include <Adafruit_NeoPixel.h>
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
const unsigned long syncInterval = 1UL * 60 * 60 * 1000;
int hour,minute,h1,h2,m1,m2;
const int LED_Port=4;
int ledCount = 4;
int ledMode = 0; 
bool wifiSyncedFirstTime = false;  // tracks first successful WiFi connection
bool timeSynced = false;           // tracks first successful NTP sync

Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledCount, LED_Port, NEO_GRB + NEO_KHZ800);// led pin, type -> //LED Strip Controller
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
uint32_t getColor(int flag) 
{
  switch(flag) 
  {
    case 0: return strip.Color(255, 255, 255); // White
    case 1: return strip.Color(255, 0, 0);     // Red
    case 2: return strip.Color(0, 255, 0);     // Green
    case 3: return strip.Color(0, 0, 255);     // Blue
    case 4: return strip.Color(255, 165, 0);   // Orange
    case 5: return strip.Color(255, 255, 0);   // Yellow
    case 6: return strip.Color(0, 255, 255);   // Cyan
    case 7: return strip.Color(255, 20, 147);  // Pink
    
    // Extra colors
    case 8: return strip.Color(128, 0, 128);   // Purple
    case 9: return strip.Color(0, 128, 255);   // Aqua
    case 10: return strip.Color(255, 50, 50);  // Soft Red
    case 11: return strip.Color(255, 255, 50);  // Soft Yellow
    default: return strip.Color(255, 255, 255);
  }
}

void ledTask(int Mode_Select) 
{
  switch (Mode_Select)
  {

    case 1:
      for (int i = 0; i < 4; i++) {
        strip.setPixelColor(i, getColor(6)); // Cyan (change if needed)
      }
      break;

    case 2:
      for (int i = 0; i < 4; i++) {
        strip.setPixelColor(i, getColor(4)); // Orange (change if needed)
      }
      break;

    case 3:
      for (int i = 0; i < 4; i++) {
        strip.setPixelColor(i, getColor(1)); // Red (change if needed)
      }
      break;

    case 4:
      strip.setPixelColor(0, getColor(1)); // Red
      strip.setPixelColor(1, getColor(2)); // Green
      strip.setPixelColor(2, getColor(3)); // Blue
      strip.setPixelColor(3, getColor(5)); // Yellow
      break;
    
    case 5:
      strip.setPixelColor(0, getColor(4)); // Orange
      strip.setPixelColor(1, getColor(7)); // Pink
      strip.setPixelColor(2, getColor(9)); // Aqua
      strip.setPixelColor(3, getColor(10)); // Soft Red
      break;
    
    case 6:
      for (int i = 0; i < 4; i++) 
      {
        strip.setPixelColor(i, getColor(10)); // Soft Red (change if needed)
      }
      break;
      
    case 7:
      for (int i = 0; i < 4; i++) 
      {
        strip.setPixelColor(i, getColor(8)); // Purple (change if needed)
      }
      break;
       
    case 8:
      for (int i = 0; i < 4; i++) 
      {
        strip.setPixelColor(i, getColor(11)); //  Soft Yellow (change if needed)
      }
      break;


    case 0:
    default:
      for (int i = 0; i < 4; i++) {
        strip.setPixelColor(i, getColor(0)); // White
      }
      break;
  }
  strip.show();
}
void startup() 
{
  Serial.println("Startup animation...");
  ledTask(4);
  // Step 1: go to max values
  hour_ms.set_to_digit(2);
  hour_ls.set_to_digit(9);
  minute_ms.set_to_digit(5);
  minute_ls.set_to_digit(9);
  delay(1000);
  ledTask(5);
  // Step 2: return to zero
  hour_ms.force_zero();
  hour_ls.force_zero();
  minute_ms.force_zero();
  minute_ls.force_zero();
  delay(500);
  Serial.println("Startup done.");
  ledTask(0);
}

// ================= TIME FUNCTION =================
void timeSet() 
{
  struct tm timeinfo;
  // Try to read time from internal ESP clock
  if (!getLocalTime(&timeinfo)) 
  {
    Serial.println("❌ Failed to read internal time");
    ledTask(3); //  TURN ON ERROR LED HERE
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
  // ================= FIRST TIME WIFI =================
  if (!wifiSyncedFirstTime) 
  {
    Serial.println("⚠️ Searching WiFi for first time...");
    while (true) 
    {
      wifi_connect();  // try connecting WiFi

      if (WiFi.status() == WL_CONNECTED) 
      {
        Serial.println("✅ First time WiFi connected!");
        wifiSyncedFirstTime = true;  // will not enter this loop again
        ledTask(0); // LED OFF, WiFi OK
        break;      // exit first-time WiFi loop
      }

      Serial.println("❌ WiFi not found, retrying...");
      ledTask(3); // LED for WiFi error
      delay(1000); // retry every 1 sec
    }
  }

  // ================= FIRST TIME NTP =================
  if (!timeSynced) 
  {
    Serial.println("⚠️ First time NTP sync required...");
    while (true) 
    {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) 
      {
        Serial.println("✅ First time NTP sync successful!");
        timeSynced = true;   // will not enter this loop again
        ledTask(0);          // LED OFF, NTP OK
        break;               // exit first-time NTP loop
      }

      Serial.println("❌ NTP not reachable, retrying...");
      ledTask(8); // LED for NTP error
      delay(1000); // retry every 1 sec
    }
  }

  // ================= NORMAL OPERATION =================
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("❌ No WiFi → Skip Sync (already synced once)");
    ledTask(6); // LED warning but continue
    return;
  }

  // Request normal sync
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) 
  {
    Serial.println("✅ NTP Sync Success (normal operation)");
    ledTask(0); // LED OK
  } 
  else 
  {
    Serial.println("❌ NTP Sync Failed (continuing offline)");
    ledTask(2); // LED warning for NTP failure
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
    ledTask(0);  // white 🔌 TURN OFF ERROR LED HERE
  } 
  else 
  {
    Serial.println("\n❌ WiFi Failed (Continuing Offline)");
    ledTask(1);// 🔴 TURN ON ERROR LED HERE
    
  }
}
// ================= SETUP =================
void setup() 
{
  Serial.begin(115200);
  ledTask(7);
  delay(500);
  wifi_connect();
  syncTimeNTP();
  startup();
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