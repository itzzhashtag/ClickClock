#include <Arduino.h>

// ================= DIRECTION TEST CODE =================
// Run this ONCE to verify each motor direction
// If digit goes DOWN when should go UP → flip dir (1 → -1 or -1 → 1)
// ======================================================

// ================= CLOCK DIGIT CLASS (BLOCKING - TEST ONLY) =================
class ClockDigit 
{
  int8_t current_digit = 0;
  int8_t invert_direction = 1;
  uint8_t max_digit;
  int STEPS_PER_DIGIT;

  int pins[4];
  int stepIndex = 0;

  const int halfStep[8][4] = {
    {1, 0, 0, 0}, {1, 1, 0, 0},
    {0, 1, 0, 0}, {0, 1, 1, 0},
    {0, 0, 1, 0}, {0, 0, 1, 1},
    {0, 0, 0, 1}, {1, 0, 0, 1}
  };

  void _step(int direction)
  {
    stepIndex = (stepIndex + (direction > 0 ? 1 : 7)) % 8;
    for (int i = 0; i < 4; i++)
      digitalWrite(pins[i], halfStep[stepIndex][i]);
    delayMicroseconds(1200);
  }

  void _move(int steps)
  {
    int dir = (steps > 0) ? invert_direction : -invert_direction;
    int count = abs(steps);
    for (int i = 0; i < count; i++)
      _step(dir);
    _release();
  }

  void _release()
  {
    for (int i = 0; i < 4; i++)
      digitalWrite(pins[i], LOW);
  }

public:
  ClockDigit(int8_t dir, uint8_t maxDig, int spd,
             int blu, int yel, int pnk, int ora)
    : invert_direction(dir),
      max_digit(maxDig),
      STEPS_PER_DIGIT(spd)
  {
    pins[0] = blu; pins[1] = yel;
    pins[2] = pnk; pins[3] = ora;
    for (int i = 0; i < 4; i++) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], LOW);
    }
  }

  // Move UP by one digit
  void up_one()
  {
    if (current_digit >= max_digit) {
      Serial.println("⚠️  Already at MAX — not moving UP");
      return;
    }
    _move(STEPS_PER_DIGIT);
    current_digit++;
    Serial.printf("  ⬆️  UP → now at digit %d\n", current_digit);
  }

  // Move DOWN by one digit
  void down_one()
  {
    if (current_digit <= 0) {
      Serial.println("⚠️  Already at 0 — not moving DOWN");
      return;
    }
    _move(-STEPS_PER_DIGIT);
    current_digit--;
    Serial.printf("  ⬇️  DOWN → now at digit %d\n", current_digit);
  }

  uint8_t getDigit() { return current_digit; }
  uint8_t getMax()   { return max_digit; }
};

// ================= MOTOR OBJECTS =================
//         dir  max  spd   blu  yel  pnk  ora
ClockDigit hour_ms  ( 1, 2, 679, 16, 17,  5, 18);
ClockDigit hour_ls  (-1, 9, 203, 19, 21, 22, 23);
ClockDigit minute_ms( 1, 5, 339, 25, 26, 27, 14);
ClockDigit minute_ls(-1, 9, 203, 12, 13, 32, 33);

// ================= TEST ONE MOTOR =================
void testMotor(ClockDigit &motor, const char* name)
{
  Serial.println("─────────────────────────────────");
  Serial.printf("🧪 Testing: %s (max digit = %d)\n", name, motor.getMax());
  Serial.println("─────────────────────────────────");

  // Step 1 — go UP one digit
  Serial.println("► Step 1: Moving UP one digit...");
  motor.up_one();
  delay(800);

  // Step 2 — go UP again
  Serial.println("► Step 2: Moving UP one more digit...");
  motor.up_one();
  delay(800);

  // Step 3 — come DOWN one digit
  Serial.println("► Step 3: Moving DOWN one digit...");
  motor.down_one();
  delay(800);

  // Step 4 — return to 0
  Serial.println("► Step 4: Returning DOWN to 0...");
  motor.down_one();
  delay(800);

  Serial.printf("✅ %s done — final position: %d\n\n", name, motor.getDigit());
  delay(1000); // pause before next motor
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║   MOTOR DIRECTION TEST MODE      ║");
  Serial.println("║  Watch each digit carefully:     ║");
  Serial.println("║  UP = digit number increases     ║");
  Serial.println("║  DOWN = digit number decreases   ║");
  Serial.println("║  Wrong way → flip dir in object  ║");
  Serial.println("╚══════════════════════════════════╝\n");
  delay(2000);

  testMotor(hour_ms,   "H1 - Hour Tens   (0–2)");
  testMotor(hour_ls,   "H2 - Hour Units  (0–9)");
  testMotor(minute_ms, "M1 - Min Tens    (0–5)");
  testMotor(minute_ls, "M2 - Min Units   (0–9)");

  Serial.println("════════════════════════════════════");
  Serial.println("✅ ALL MOTORS TESTED");
  Serial.println("If any moved wrong direction:");
  Serial.println("  → Flip its dir value: 1 → -1 or -1 → 1");
  Serial.println("  → Re-flash and test again");
  Serial.println("════════════════════════════════════");
}

// ================= LOOP =================
void loop()
{
  // Nothing — test runs once in setup()
}