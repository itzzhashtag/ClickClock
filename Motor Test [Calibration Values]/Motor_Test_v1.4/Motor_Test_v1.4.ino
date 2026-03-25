// ============================================================
//  Click Clock — Simple Motor Test
//  Each motor: UP to max → back DOWN to 0
//  Drift on M1? Lower its spd value (560 → 556 etc.)
// ============================================================

#include <Stepper.h>

// ============================================================
// ----- ClockDigit Class -----
// ============================================================
class ClockDigit
{
  int8_t current_digit    = 0;
  int8_t invert_direction = 1;
  int    STEPS_PER_DIGIT;          // per-motor — tune individually

  static const int STEPS_PER_REVOLUTION = 2038;

  Stepper stepper;

public:
  // dir = +1 or -1 | spd = steps per digit | then IN1..IN4
  ClockDigit(int8_t dir, int spd, int blu, int yel, int pnk, int ora)
    : invert_direction(dir),
      STEPS_PER_DIGIT(spd),
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
// ----- Motor Objects -----
// ClockDigit(dir, spd, IN1, IN2, IN3, IN4)
//
// spd = steps per digit — adjust per motor to fix drift:
//   too many steps → motor overshoots → lower spd
//   too few steps  → motor undershoots → raise spd
// ============================================================
//               dir  spd   IN1  IN2  IN3  IN4
ClockDigit hour_ms  ( 1,  560,  16,  17,   5,  18);  // H1 — Hour tens   (0–2)
ClockDigit hour_ls  (-1,  560,  19,  21,  22,  23);  // H2 — Hour units  (0–9)
ClockDigit minute_ms( 1,  560,  25,  26,  27,  14);  // M1 — Min  tens   (0–5) ← tune if drifting
ClockDigit minute_ls(-1,  560,  12,  13,  32,  33);  // M2 — Min  units  (0–9)


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
// ----- Top to Bottom Test -----
// Each motor: UP to max → back DOWN to 0
// ============================================================
void top_to_bottom()
{
  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║  TEST 1: Full Range              ║");
  Serial.println("║  Each motor: UP to max → DOWN 0  ║");
  Serial.println("╚══════════════════════════════════╝\n");
  delay(1000);

  testMotor(hour_ms,   "H1 — Hour Tens   (0–2)", 2);
  testMotor(hour_ls,   "H2 — Hour Units  (0–9)", 9);
  testMotor(minute_ms, "M1 — Min  Tens   (0–5)", 5);
  testMotor(minute_ls, "M2 — Min  Units  (0–9)", 9);

  Serial.println("✅ Test 1 done\n");
}


// ============================================================
// ----- Two Digit Test -----
// All 4 motors together: 0 → 1 → 2 → 1 → 0
// ============================================================
void two_digit_test()
{
  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║  TEST 2: Two Digit (All Together)║");
  Serial.println("║  All motors: 0→1→2→1→0           ║");
  Serial.println("╚══════════════════════════════════╝\n");
  delay(1000);

  Serial.println("► All → digit 1");
  hour_ms.set_to_digit(1);   hour_ls.set_to_digit(1);
  minute_ms.set_to_digit(1); minute_ls.set_to_digit(1);
  delay(1000);

  Serial.println("► All → digit 2");
  hour_ms.set_to_digit(2);   hour_ls.set_to_digit(2);
  minute_ms.set_to_digit(2); minute_ls.set_to_digit(2);
  delay(1000);

  Serial.println("► All → digit 1");
  hour_ms.set_to_digit(1);   hour_ls.set_to_digit(1);
  minute_ms.set_to_digit(1); minute_ls.set_to_digit(1);
  delay(1000);

  Serial.println("► All → digit 0");
  hour_ms.force_zero(); hour_ls.force_zero();
  minute_ms.force_zero(); minute_ls.force_zero();
  delay(1000);

  Serial.println("✅ Test 2 done\n");
}


// ============================================================
// ----- Setup -----
// ============================================================
void setup()
{
  Serial.begin(115200);
  delay(1000);
  minute_ms.set_to_digit(0);  // already at 0, so just re-seats it

  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║   CLICK CLOCK — MOTOR TEST       ║");
  Serial.println("║  Wrong way?  Flip dir: 1 → -1    ║");
  Serial.println("║  Drifting?   Tune spd value       ║");
  Serial.println("╚══════════════════════════════════╝\n");
  delay(1500);

  top_to_bottom();
  two_digit_test();

  Serial.println("════════════════════════════════════");
  Serial.println("✅ ALL TESTS DONE");
  Serial.println("Drifting? Lower that motor's spd");
  Serial.println("  e.g. 560 → 556 and re-flash");
  Serial.println("════════════════════════════════════");
}

void loop() { /* nothing */ }
