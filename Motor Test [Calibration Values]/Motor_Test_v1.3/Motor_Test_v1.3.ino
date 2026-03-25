// ============================================================
//  Click Clock — Simple Motor Test
//  Uses same ClockDigit class as main clock (Stepper library)
//  Each motor: goes UP to max → comes back DOWN to 0
//  Wrong direction? Flip dir: 1 → -1 or -1 → 1
// ============================================================

#include <Stepper.h>

// ============================================================
// ----- ClockDigit Class (exact copy from main clock) -----
// ============================================================
class ClockDigit
{
  int8_t current_digit    = 0;
  int8_t invert_direction = 1;

  static const int STEPS_PER_REVOLUTION = 2038;
  static const int STEPS_PER_DIGIT      = 560;   // ← CALIBRATE THIS

  Stepper stepper;

public:
  ClockDigit(int8_t dir, int blu, int yel, int pnk, int ora)
    : invert_direction(dir),
      stepper(STEPS_PER_REVOLUTION, blu, yel, pnk, ora)
  {
    stepper.setSpeed(10);
  }

  void set_to_digit(uint8_t digit)
  {
    int delta = digit - current_digit;
    if (delta != 0)
    {
      stepper.step(invert_direction * STEPS_PER_DIGIT * delta);
      current_digit = digit;
    }
  }

  void force_zero()
  {
    stepper.step(invert_direction * STEPS_PER_DIGIT * (-current_digit));
    current_digit = 0;
  }
};


// ============================================================
// ----- Motor Objects (exact copy from main clock) -----
// ============================================================
ClockDigit hour_ms  ( 1, 16, 17,  5, 18);  // H1 — Hour   tens  (0–2)
ClockDigit hour_ls  (-1, 19, 21, 22, 23);  // H2 — Hour   units (0–9)
ClockDigit minute_ms( 1, 25, 26, 27, 14);  // M1 — Minute tens  (0–5)
ClockDigit minute_ls(-1, 12, 13, 32, 33);  // M2 — Minute units (0–9)


// ============================================================
// ----- Test One Motor -----
// ============================================================
void testMotor(ClockDigit &motor, const char* name, uint8_t maxDigit)
{
  Serial.println("─────────────────────────────────");
  Serial.printf( "🧪 %s\n", name);

  Serial.printf( "► Going UP to %d...\n", maxDigit);
  motor.set_to_digit(maxDigit);
  delay(1000);

  Serial.println("► Coming DOWN to 0...");
  motor.force_zero();
  delay(1000);

  Serial.printf("✅ %s done\n\n", name);
  delay(1000);
}


// ============================================================
// ----- Setup -----
// ============================================================
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║   MOTOR DIRECTION TEST           ║");
  Serial.println("║  Each motor: UP to max → DOWN  0 ║");
  Serial.println("║  Wrong way? Flip dir: 1 → -1     ║");
  Serial.println("╚══════════════════════════════════╝\n");
  delay(1500);

  testMotor(hour_ms,   "H1 — Hour Tens   (0–2)", 2);
  testMotor(hour_ls,   "H2 — Hour Units  (0–9)", 9);
  testMotor(minute_ms, "M1 — Min  Tens   (0–5)", 5);
  testMotor(minute_ls, "M2 — Min  Units  (0–9)", 9);

  Serial.println("════════════════════════════════════");
  Serial.println("✅ ALL DONE");
  Serial.println("Wrong direction? Flip dir: 1→-1 or -1→1");
  Serial.println("Steps off?  Adjust STEPS_PER_DIGIT");
  Serial.println("════════════════════════════════════");
}

void loop() { /* nothing */ }
