// ============================================================
//  _   _    _    ____  _   _ _____   _     ____ 
// | | | |  / \  / ___|| | | |_   _| / \   / ___|
// | |_| | / _ \ \___ \| |_| | | |  / _ \ | |  _ 
// |  _  |/ ___ \ ___) |  _  | | | / ___ \| |_| |
// |_| |_/_/   \_\____/|_| |_| |_|/_/   \_\\____|
//
//  Click Clock — ESP32 Stepper Digit Clock
//  HH:MM — 24hr Format — NTP Synced — IST (UTC+5:30)
//  H1: 0-2 | H2: 0-9 | M1: 0-5 | M2: 0-9
// ============================================================


// ============================================================
// ----- Libraries -----
// ============================================================
#include <WiFi.h>               // ESP32 WiFi
#include "time.h"               // NTP time functions
#include <Stepper.h>            // Stepper motor control
#include <Adafruit_NeoPixel.h>  // WS2812 RGB LED strip


// ============================================================
// ----- WiFi Config -----
// ============================================================
const char* ssid = "your_ssid";           // Wifi SSID
const char* password = "ssid_password";   // Wifi Passcode

// Static IP config (optional — comment out if using DHCP)
IPAddress local_IP("Your_ESP32_IP");       // Static IP for ESP1 Set something nearby your IP
IPAddress gateway("Your_static_Ip");      // Routers IP - Wifi
IPAddress subnet(255, 255, 255, 0);     // Subnet mask
IPAddress primaryDNS(8, 8, 8, 8);       // Google primary DNS
IPAddress secondaryDNS(8, 8, 4, 4);     // Google secondary DNS


// ============================================================
// ----- NTP Time Config -----
// ============================================================
const char* ntpServer1 = "time.google.com";   // Primary NTP server
const char* ntpServer2 = "pool.ntp.org";      // Fallback NTP server
const char* ntpServer3 = "time.windows.com";  // Second fallback

const long gmtOffset_sec      = 19800;  // IST = UTC + 5:30 = 19800 seconds
const int  daylightOffset_sec = 0;      // India has no daylight saving


// ============================================================
// ----- Global Variables -----
// ============================================================
unsigned long lastSync = 0;                          // Timestamp of last NTP resync
const unsigned long syncInterval = 1UL * 59 * 1000; // Resync every 59 seconds

int hour, minute;       // Current hour and minute (assigned from NTP)
int h1, h2, m1, m2;    // Individual digits: HH:MM → h1 h2 : m1 m2

bool wifiSyncedFirstTime = false;  // True after first successful WiFi connect
bool timeSynced          = false;  // True after first successful NTP sync


// ============================================================
// ----- LED Strip Config -----
// ============================================================
const int LED_Port  = 4;   // GPIO pin connected to LED data line
const int ledCount  = 4;   // Number of LEDs in the strip
int       ledMode   = 0;   // Current LED mode (reserved for future use)

// Initialise NeoPixel strip object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledCount, LED_Port, NEO_GRB + NEO_KHZ800);


// ============================================================
// ----- ClockDigit Class -----
// Controls one stepper motor that drives one digit column
// Each digit position = STEPS_PER_DIGIT motor steps
// invert_direction: +1 or -1 — flip if motor spins wrong way
// ============================================================
class ClockDigit
{
  int8_t current_digit    = 0;  // Tracks where motor currently is (0–max)
  int8_t invert_direction = 1;  // +1 = normal, -1 = reversed wiring

  static const int STEPS_PER_REVOLUTION = 2038;  // 28BYJ-48 full revolution
  static const int STEPS_PER_DIGIT      = 560;   // Steps between digits — CALIBRATE THIS

  Stepper stepper;  // Arduino Stepper instance

public:
  // Constructor — set direction, wiring pins, and speed
  // blu/yel/pnk/ora = ULN2003 IN1 IN2 IN3 IN4 pins
  ClockDigit(int8_t dir, int blu, int yel, int pnk, int ora)
    : invert_direction(dir),
      stepper(STEPS_PER_REVOLUTION, blu, yel, pnk, ora)
  {
    stepper.setSpeed(10);  // RPM — lower = smoother, higher = faster
  }

  // Move to an absolute digit position (0 = bottom, max = top)
  // Positive delta → motor goes UP, Negative delta → motor goes DOWN
  void set_to_digit(uint8_t digit)
  {
    int delta = digit - current_digit;  // How many digits to move
    if (delta != 0)
    {
      // invert_direction * delta gives correct physical direction
      stepper.step(invert_direction * STEPS_PER_DIGIT * delta);
      current_digit = digit;  // Update tracked position
    }
  }

  // Force motor back to position 0 (home / bottom)
  // Always moves DOWN by however many digits we are above 0
  void force_zero()
  {
    stepper.step(invert_direction * STEPS_PER_DIGIT * (-current_digit));
    current_digit = 0;
  }
  void boot_correct(int steps)
  {
    stepper.step(steps);
  }
};


// ============================================================
// ----- Motor Objects -----
// ClockDigit(direction, IN1, IN2, IN3, IN4)
// direction: +1 = clockwise UP | -1 = anticlockwise UP
// ============================================================
ClockDigit hour_ms  ( 1, 16, 17,  5, 18);  // H1 — Hour   tens  (0–2)
ClockDigit hour_ls  (-1, 19, 21, 22, 23);  // H2 — Hour   units (0–9)
ClockDigit minute_ms( 1, 25, 26, 27, 14);  // M1 — Minute tens  (0–5)
ClockDigit minute_ls(-1, 12, 13, 32, 33);  // M2 — Minute units (0–9)


// ============================================================
// ----- Motor Work -----
// Sends current h1 h2 m1 m2 digit values to each motor
// Motors move one at a time (Stepper library is blocking)
// ============================================================
void motorWork()
{
  Serial.println("⚙️  Moving Motors...");
  hour_ms.set_to_digit(h1);    // Move H1 to hour tens digit
  hour_ls.set_to_digit(h2);    // Move H2 to hour units digit
  minute_ms.set_to_digit(m1);  // Move M1 to minute tens digit
  minute_ls.set_to_digit(m2);  // Move M2 to minute units digit
  Serial.println("✅ Movement Done\n");
}


// ============================================================
// ----- LED Color Palette -----
// Returns a 32-bit GRB color value by index number
// ============================================================
uint32_t getColor(int flag)
{
  switch (flag)
  {
    case  0: return strip.Color(255, 255, 255);  // White
    case  1: return strip.Color(255,   0,   0);  // Red
    case  2: return strip.Color(  0, 255,   0);  // Green
    case  3: return strip.Color(  0,   0, 255);  // Blue
    case  4: return strip.Color(255, 165,   0);  // Orange
    case  5: return strip.Color(255, 255,   0);  // Yellow
    case  6: return strip.Color(  0, 255, 255);  // Cyan
    case  7: return strip.Color(255,  20, 147);  // Pink
    case  8: return strip.Color(128,   0, 128);  // Purple
    case  9: return strip.Color(  0, 128, 255);  // Aqua
    case 10: return strip.Color(255,  50,  50);  // Soft Red
    case 11: return strip.Color(255, 255,  50);  // Soft Yellow
    default: return strip.Color(255, 255, 255);  // Default White
  }
}


// ============================================================
// ----- LED Mode Controller -----
// Sets all 4 LEDs to a preset color pattern by mode number
//
//  0 = White (normal / idle)
//  1 = Cyan
//  2 = Orange
//  3 = Red        ← WiFi error indicator
//  4 = RGBY       ← Startup phase 1
//  5 = Mixed      ← Startup phase 2
//  6 = Soft Red   ← WiFi warning (offline but running)
//  7 = Purple     ← Boot indicator
//  8 = Soft Yellow ← NTP error indicator
// ============================================================
void ledTask(int Mode_Select)
{
  switch (Mode_Select)
  {
    case 1:  // Cyan — reserved
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(6));
      break;

    case 2:  // Orange — NTP failed but continuing
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(4));
      break;

    case 3:  // Red — WiFi connection error
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(1));
      break;

    case 4:  // Red/Green/Blue/Yellow — startup phase 1 (going to max)
      strip.setPixelColor(0, getColor(1));  // Red
      strip.setPixelColor(1, getColor(2));  // Green
      strip.setPixelColor(2, getColor(3));  // Blue
      strip.setPixelColor(3, getColor(5));  // Yellow
      break;

    case 5:  // Orange/Pink/Aqua/SoftRed — startup phase 2 (returning to zero)
      strip.setPixelColor(0, getColor(4));   // Orange
      strip.setPixelColor(1, getColor(7));   // Pink
      strip.setPixelColor(2, getColor(9));   // Aqua
      strip.setPixelColor(3, getColor(10));  // Soft Red
      break;

    case 6:  // Soft Red — no WiFi warning (clock still running from RTC)
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(10));
      break;

    case 7:  // Purple — boot sequence running
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(8));
      break;

    case 8:  // Soft Yellow — NTP unreachable, retrying
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(11));
      break;

    case 0:  // White — normal idle state
    default:
      for (int i = 0; i < 4; i++) strip.setPixelColor(i, getColor(0));
      break;
  }
  strip.show();  // Push color data to physical LEDs
}


// ============================================================
// ----- Startup Animation -----
// Runs once on boot to verify all motors are working
// Phase 1: all digits rise to their maximum position
// Phase 2: all digits return back down to zero
// ============================================================
void startup()
{
  Serial.println("🎬 Startup animation starting...");

  // Phase 1 — rise to max (motors go UP one by one)
  ledTask(4);                   // LED: Red/Green/Blue/Yellow
  hour_ms.set_to_digit(2);      // H1 max = 2
  hour_ls.set_to_digit(9);      // H2 max = 9
  minute_ms.set_to_digit(5);    // M1 max = 5
  minute_ls.set_to_digit(9);    // M2 max = 9
  delay(1000);                  // Brief pause at top

  // Phase 2 — return to zero (motors go DOWN one by one)
  ledTask(5);                   // LED: Orange/Pink/Aqua/SoftRed
  hour_ms.force_zero();         // H1 → 0
  hour_ls.force_zero();         // H2 → 0
  minute_ms.force_zero();       // M1 → 0
  minute_ls.force_zero();       // M2 → 0
  delay(500);                   // Settle before clock starts

  Serial.println("✅ Startup done.");
  ledTask(0);                   // LED: back to white
}


// ============================================================
// ----- WiFi Connect -----
// Attempts WiFi connection with a 5 second timeout
// Sets LED red on failure, white on success
// ============================================================
void wifi_connect()
{
  Serial.println("🔄 Connecting to WiFi...");
  WiFi.mode(WIFI_STA);                     // Station mode (client)
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);  //Configuration of WIfi 
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout    = 5000;   // Give up after 5 seconds

  // Poll until connected or timed out
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout)
  {
    delay(200);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\n✅ WiFi Connected!");
    ledTask(0);  // White — all good
  }
  else
  {
    Serial.println("\n❌ WiFi Failed (Continuing Offline)");
    ledTask(1);  // Red — connection failed
  }
}


// ============================================================
// ----- Time → Digits -----
// Reads current time from ESP32 internal RTC (set by NTP)
// Splits HH:MM into four individual digits h1 h2 m1 m2
// Also triggers periodic NTP resync every syncInterval ms
// ============================================================
void timeSet()
{
  struct tm timeinfo;

  // Read time from ESP32 internal RTC
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("❌ Failed to read internal time");
    ledTask(3);  // Red — RTC read error
    return;
  }

  // Assign to globals (no `int` here — already declared globally)
  hour   = timeinfo.tm_hour;
  minute = timeinfo.tm_min;

  // Split into individual digits
  h1 = (hour   / 10) % 3;  // Hour   tens  — valid range 0–2
  h2 =  hour   % 10;       // Hour   units — valid range 0–9
  m1 = (minute / 10) % 6;  // Minute tens  — valid range 0–5
  m2 =  minute % 10;       // Minute units — valid range 0–9

  // Serial debug output
  Serial.println("=========================");
  Serial.printf( "🕒 Time: %02d:%02d\n", hour, minute);
  Serial.print(  "   H1 (hour tens)    : "); Serial.println(h1);
  Serial.print(  "   H2 (hour units)   : "); Serial.println(h2);
  Serial.print(  "   M1 (minute tens)  : "); Serial.println(m1);
  Serial.print(  "   M2 (minute units) : "); Serial.println(m2);
  Serial.printf( "   Digits → %d%d:%d%d\n", h1, h2, m1, m2);
  Serial.println("=========================");

  // Periodic NTP resync — triggered every syncInterval milliseconds
  if (millis() - lastSync > syncInterval)
  {
    Serial.println("🔄 Periodic NTP Resync...");
    syncTimeNTP();        // Attempt resync (LED handled inside)
    lastSync = millis();  // Reset resync timer
  }
}


// ============================================================
// ----- NTP Sync -----
// Handles three cases:
//   1. First boot — blocks until WiFi connects
//   2. First boot — blocks until NTP responds
//   3. Normal operation — resync if WiFi available, warn if not
// ============================================================
void syncTimeNTP()
{
  // ----- Case 1: First time WiFi -----
  // Keeps retrying forever until connected on first boot
  if (!wifiSyncedFirstTime)
  {
    Serial.println("⚠️  First boot — waiting for WiFi...");
    while (true)
    {
      wifi_connect();

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("✅ First WiFi connection successful!");
        wifiSyncedFirstTime = true;  // Never enter this block again
        ledTask(0);                  // White — WiFi OK
        break;
      }

      Serial.println("❌ WiFi not found — retrying in 1s...");
      ledTask(3);      // Red — WiFi error
      delay(1000);
    }
  }

  // ----- Case 2: First time NTP -----
  // Keeps retrying forever until time is received on first boot
  if (!timeSynced)
  {
    Serial.println("⚠️  First boot — waiting for NTP...");
    while (true)
    {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
      struct tm timeinfo;

      if (getLocalTime(&timeinfo))
      {
        Serial.println("✅ First NTP sync successful!");
        timeSynced = true;  // Never enter this block again
        ledTask(0);         // White — NTP OK
        break;
      }

      Serial.println("❌ NTP not reachable — retrying in 1s...");
      ledTask(8);    // Soft Yellow — NTP error
      delay(1000);
    }
  }

  // ----- Case 3: Normal periodic resync -----
  // If WiFi is gone, warn and try reconnecting — but don't block
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("❌ No WiFi — skipping resync (clock still running)");
    ledTask(6);      // Soft Red — offline warning
    wifi_connect();  // Try reconnecting (non-blocking, 5s timeout)
    return;
  }

  // WiFi is up — request fresh time from NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
  struct tm timeinfo;

  if (getLocalTime(&timeinfo))
  {
    Serial.println("✅ NTP resync successful");
    ledTask(0);  // White — all good
  }
  else
  {
    Serial.println("❌ NTP resync failed — continuing from internal RTC");
    ledTask(2);  // Orange — NTP failed but clock still running
  }
}


// ============================================================
// ----- Setup -----
// Runs once on power-on or reset
// ============================================================
int lastMinute = -1;  // Tracks last minute to detect changes

void setup()
{
  Serial.begin(115200);   // Start serial monitor at 115200 baud
  strip.begin();          // Initialise LED strip (required before any LED calls)
  strip.show();           // Push blank frame — ensures all LEDs start OFF
  ledTask(7);             // Purple — boot in progress
  minute_ms.boot_correct(4);  // Fix Drift in M1 -> Hrs 00:*0
  delay(500);             // Brief pause so purple is visible
  wifi_connect();         // Connect to WiFi (5s timeout, non-blocking)
  syncTimeNTP();          // Sync time from NTP (blocks until first sync)
  startup();              // Run motor + LED startup animation
}


// ============================================================
// ----- Loop -----
// Runs continuously — checks if minute has changed
// Only updates motors once per minute to save wear
// ============================================================
void loop()
{
  struct tm timeinfo;

  // If RTC is unavailable, skip this iteration
  if (!getLocalTime(&timeinfo)) return;

  // Check if the minute has changed since last update
  if (timeinfo.tm_min != lastMinute)
  {
    lastMinute = timeinfo.tm_min;              // Store new minute
    Serial.println("⏱  Minute changed → updating clock");
    timeSet();    // Read time, split digits, trigger resync if due
    motorWork();  // Move all 4 motors to new digit positions
  }
}

// ============================================================
// ----- The End -----
// ============================================================
