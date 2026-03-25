// ============================================================
//  Click Clock — Simple Motor Direction Test
//  Runs once: each motor goes UP 2 digits then back DOWN to 0
//  Watch each motor — if wrong direction, flip its dir value
// ============================================================

#include <Arduino.h>


// ============================================================
// ----- ClockDigit Class -----
// ============================================================
class ClockDigit
{
  int8_t current_digit    = 0;
  int8_t invert_direction = 1;
  int    STEPS_PER_DIGIT;

  int pins[4];
  int stepIndex = 0;

  const int halfStep[8][4] = {
    {1,0,0,0}, {1,1,0,0},
    {0,1,0,0}, {0,1,1,0},
    {0,0,1,0}, {0,0,1,1},
    {0,0,0,1}, {1,0,0,1}
  };

  // Move one half-step in given direction (+1 or -1)
  void _step(int dir)
  {
    stepIndex = (stepIndex + (dir > 0 ? 1 : 7)) % 8;
    for (int i = 0; i < 4; i++)
      digitalWrite(pins[i], halfStep[stepIndex][i]);
    delayMicroseconds(1200);
  }

  // Move N raw steps — positive = UP direction, negative = DOWN direction
  void _move(int steps)
  {
    // physical direction = sign of steps × invert_direction
    int physDir = (steps > 0) ? invert_direction : -invert_direction;
    int count   = abs(steps);
    for (int i = 0; i < count; i++)
      _step(physDir);
    // Release coils after move
    for (int i = 0; i < 4; i++)
      digitalWrite(pins[i], LOW);
  }

public:
  ClockDigit(int8_t dir, int spd, int blu, int yel, int pnk, int ora)
    : invert_direction(dir), STEPS_PER_DIGIT(spd)
  {
    pins[0]=blu; pins[1]=yel; pins[2]=pnk; pins[3]=ora;
    for (int i = 0; i < 4; i++) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], LOW);
    }
  }

  void up_one()
  {
    _move(+STEPS_PER_DIGIT);   // positive = UP
    current_digit++;
    Serial.printf("  ⬆️  UP   → digit %d\n", current_digit);
  }

  void down_one()
  {
    _move(-STEPS_PER_DIGIT);   // negative = DOWN
    current_digit--;
    Serial.printf("  ⬇️  DOWN → digit %d\n", current_digit);
  }
};


// ============================================================
// ----- Motor Objects -----
// ClockDigit(dir, steps_per_digit, IN1, IN2, IN3, IN4)
// dir: +1 = clockwise is UP | -1 = anticlockwise is UP
// ============================================================
//            dir   spd   blu  yel  pnk  ora
ClockDigit H1(  1,  679,   16,  17,   5,  18);  // Hour tens   (0–2)
ClockDigit H2( -1,  203,   19,  21,  22,  23);  // Hour units  (0–9)
ClockDigit M1(  1,  339,   25,  26,  27,  14);  // Min  tens   (0–5)
ClockDigit M2( -1,  203,   12,  13,  32,  33);  // Min  units  (0–9)


// ============================================================
// ----- Test One Motor -----
// Goes UP twice, then DOWN twice back to 0
// ============================================================
void testMotor(ClockDigit &motor, const char* name)
{
  Serial.println("─────────────────────────────────");
  Serial.printf( "🧪 Testing: %s\n", name);
  Serial.println("─────────────────────────────────");

  Serial.println("► UP one...");   motor.up_one();   delay(600);
  Serial.println("► UP one...");   motor.up_one();   delay(600);
  Serial.println("► DOWN one..."); motor.down_one(); delay(600);
  Serial.println("► DOWN one..."); motor.down_one(); delay(600);

  Serial.printf("✅ %s done\n\n", name);
  delay(1000);
}


// ============================================================
// ----- Setup — runs test once -----
// ============================================================
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║   MOTOR DIRECTION TEST           ║");
  Serial.println("║  Each motor: UP UP DOWN DOWN     ║");
  Serial.println("║  Wrong way? Flip dir: 1 → -1     ║");
  Serial.println("╚══════════════════════════════════╝\n");
  delay(1500);

  testMotor(H1, "H1 — Hour Tens   (0–2)");
  testMotor(H2, "H2 — Hour Units  (0–9)");
  testMotor(M1, "M1 — Min  Tens   (0–5)");
  testMotor(M2, "M2 — Min  Units  (0–9)");

  Serial.println("════════════════════════════════════");
  Serial.println("✅ ALL DONE");
  Serial.println("Wrong direction? Flip its dir value");
  Serial.println("  1 → -1   or   -1 → 1");
  Serial.println("════════════════════════════════════");
}

void loop() { /* nothing */ }
