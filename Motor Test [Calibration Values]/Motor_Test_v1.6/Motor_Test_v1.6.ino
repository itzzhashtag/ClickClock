
#include <Stepper.h>

// ============================================================
// ----- ClockDigit Class -----
// ============================================================
class ClockDigit
{
  int8_t  current_digit    = 0;
  int8_t  invert_direction = 1;
  uint8_t max_digit;
  int     STEPS_PER_DIGIT;

  int pins[4];
  int stepIndex = 0;

  const int halfStep[8][4] = {
    {1,0,0,0}, {1,1,0,0},
    {0,1,0,0}, {0,1,1,0},
    {0,0,1,0}, {0,0,1,1},
    {0,0,0,1}, {1,0,0,1}
  };

  void _step(int dir)
  {
    stepIndex = (stepIndex + (dir > 0 ? 1 : 7)) % 8;
    for (int i = 0; i < 4; i++)
      digitalWrite(pins[i], halfStep[stepIndex][i]);
    delayMicroseconds(1200);
  }

  void _move(int steps)
  {
    int dir   = (steps > 0) ? invert_direction : -invert_direction;
    int count = abs(steps);
    for (int i = 0; i < count; i++) _step(dir);
    for (int i = 0; i < 4; i++) digitalWrite(pins[i], LOW);
  }

public:
  ClockDigit(int8_t dir, uint8_t maxDig, int spd,
             int p1, int p2, int p3, int p4)
    : invert_direction(dir), max_digit(maxDig), STEPS_PER_DIGIT(spd)
  {
    pins[0]=p1; pins[1]=p2; pins[2]=p3; pins[3]=p4;
    for (int i = 0; i < 4; i++) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], LOW);
    }
  }

  void set_to_digit(uint8_t digit)
  {
    int delta = digit - current_digit;
    if (delta != 0) {
      _move(STEPS_PER_DIGIT * delta);
      current_digit = digit;
    }
  }

  void force_zero()
  {
    _move(STEPS_PER_DIGIT * (-current_digit));
    current_digit = 0;
  }

  void up_one()
  {
    if (current_digit >= max_digit) { Serial.println("  ⚠️  Already at MAX"); return; }
    _move(STEPS_PER_DIGIT);
    current_digit++;
    Serial.printf("  ⬆️  UP → digit %d\n", current_digit);
  }

  void down_one()
  {
    if (current_digit <= 0) { Serial.println("  ⚠️  Already at 0"); return; }
    _move(-STEPS_PER_DIGIT);
    current_digit--;
    Serial.printf("  ⬇️  DOWN → digit %d\n", current_digit);
  }

  void go_to(uint8_t target)
  {
    if (target > max_digit) { Serial.printf("  ⚠️  Max is %d\n", max_digit); return; }
    int delta = (int)target - (int)current_digit;
    if (delta == 0) { Serial.printf("  ✅ Already at %d\n", current_digit); return; }
    _move(STEPS_PER_DIGIT * delta);
    current_digit = target;
    Serial.printf("  🎯 → digit %d\n", current_digit);
  }

  void go_home()
  {
    Serial.printf("  🏠 Homing from %d...\n", current_digit);
    _move(STEPS_PER_DIGIT * (-current_digit));
    current_digit = 0;
    Serial.println("  ✅ At 0");
  }

  void raw_steps(int steps)
  {
    Serial.printf("  🔧 Raw: %d steps\n", steps);
    _move(steps);
    Serial.println("  ✅ Done");
  }

  void flip_direction()
  {
    invert_direction *= -1;
    Serial.printf("  🔄 Direction → %+d\n", invert_direction);
  }

  void set_steps_per_digit(int spd)
  {
    STEPS_PER_DIGIT = spd;
    Serial.printf("  ⚙️  spd → %d\n", spd);
  }

  void status()
  {
    Serial.printf("  📍 pos=%d  max=%d  spd=%d  dir=%+d\n",
      current_digit, max_digit, STEPS_PER_DIGIT, invert_direction);
  }

  uint8_t getPos() { return current_digit; }
  uint8_t getMax() { return max_digit; }
  int     getSpd() { return STEPS_PER_DIGIT; }
  int8_t  getDir() { return invert_direction; }
};

// ============================================================
// ----- Motor Objects -----
// ClockDigit(dir, max, spd, IN1, IN2, IN3, IN4)
// ============================================================
//            dir  max  spd   IN1  IN2  IN3  IN4
ClockDigit H1( 1,   2,  560,  33, 32, 13, 12); // Hour tens   (0–2)
ClockDigit H2(-1,   9,  560,  14, 27, 26, 25);  // Hour units  (0–9)
ClockDigit M1( 1,   5,  560,  23, 22, 21, 19);  // Min  tens   (0–5)
ClockDigit M2(-1,   9,  560,  18,  5, 17, 16);  // Min  units  (0–9)
 


// ============================================================
// ----- Active Motor Selection -----
// ============================================================
ClockDigit* motors[4]      = { &H1, &H2, &M1, &M2 };
const char* motorNames[4]  = { "H1", "H2", "M1", "M2" };
int         selectedMotor  = 0;


// ============================================================
// ----- Help Menu -----
// ============================================================
void printHelp()
{
  Serial.println();
  Serial.println("╔══════════════════════════════════════════════╗");
  Serial.println("║         CLICK CLOCK — MOTOR TEST             ║");
  Serial.println("╠══════════════════════════════════════════════╣");
  Serial.println("║  SELECT MOTOR                                ║");
  Serial.println("║    1        → Select H1 (hour tens,   0–2)  ║");
  Serial.println("║    2        → Select H2 (hour units,  0–9)  ║");
  Serial.println("║    3        → Select M1 (min  tens,   0–5)  ║");
  Serial.println("║    4        → Select M2 (min  units,  0–9)  ║");
  Serial.println("╠══════════════════════════════════════════════╣");
  Serial.println("║  MOVE COMMANDS                               ║");
  Serial.println("║    u        → Move UP one digit              ║");
  Serial.println("║    d        → Move DOWN one digit            ║");
  Serial.println("║    g <n>    → Go to digit n  (e.g. g 3)      ║");
  Serial.println("║    h        → Home (return to 0)             ║");
  Serial.println("║    r <n>    → Raw steps  (e.g. r 100)        ║");
  Serial.println("╠══════════════════════════════════════════════╣");
  Serial.println("║  CALIBRATE                                   ║");
  Serial.println("║    f        → Flip direction (+1 / -1)       ║");
  Serial.println("║    s <n>    → Set steps/digit  (e.g. s 250)  ║");
  Serial.println("║    ?        → Show this help menu            ║");
  Serial.println("║    i        → Status of ALL motors           ║");
  Serial.println("╠══════════════════════════════════════════════╣");
  Serial.println("║  BATCH                                       ║");
  Serial.println("║    home     → Home ALL motors                ║");
  Serial.println("║    max      → Drive ALL to max digits        ║");
  Serial.println("║    set HHMM → Set display  (e.g. set 0930)   ║");
  Serial.println("╚══════════════════════════════════════════════╝");
  Serial.println();
}


// ============================================================
// ----- All Motor Status -----
// ============================================================
void printAllStatus()
{
  Serial.println("┌─────────────────────────────────────┐");
  Serial.println("│  ALL MOTOR STATUS                   │");
  for (int i = 0; i < 4; i++) {
    Serial.printf("│  %s  ", motorNames[i]);
    motors[i]->status();
  }
  Serial.println("└─────────────────────────────────────┘");
}


// ============================================================
// ----- Command Parser -----
// ============================================================
void handleCommand(String cmd)
{
  cmd.trim();
  if (cmd.length() == 0) return;

  Serial.printf("\n> %s\n", cmd.c_str());

  // Motor select
  if (cmd == "1") { selectedMotor = 0; Serial.printf("✅ Selected: %s\n", motorNames[0]); return; }
  if (cmd == "2") { selectedMotor = 1; Serial.printf("✅ Selected: %s\n", motorNames[1]); return; }
  if (cmd == "3") { selectedMotor = 2; Serial.printf("✅ Selected: %s\n", motorNames[2]); return; }
  if (cmd == "4") { selectedMotor = 3; Serial.printf("✅ Selected: %s\n", motorNames[3]); return; }

  // Help / status
  if (cmd == "?") { printHelp();      return; }
  if (cmd == "i") { printAllStatus(); return; }

  // Single motor commands
  if (cmd == "u") { Serial.printf("[%s] ", motorNames[selectedMotor]); motors[selectedMotor]->up_one();        return; }
  if (cmd == "d") { Serial.printf("[%s] ", motorNames[selectedMotor]); motors[selectedMotor]->down_one();      return; }
  if (cmd == "h") { Serial.printf("[%s] ", motorNames[selectedMotor]); motors[selectedMotor]->go_home();       return; }
  if (cmd == "f") { Serial.printf("[%s] ", motorNames[selectedMotor]); motors[selectedMotor]->flip_direction(); return; }

  // Go to digit: g <n>
  if (cmd.startsWith("g ")) {
    int n = cmd.substring(2).toInt();
    Serial.printf("[%s] ", motorNames[selectedMotor]);
    motors[selectedMotor]->go_to((uint8_t)n);
    return;
  }

  // Raw steps: r <n>
  if (cmd.startsWith("r ")) {
    int n = cmd.substring(2).toInt();
    Serial.printf("[%s] ", motorNames[selectedMotor]);
    motors[selectedMotor]->raw_steps(n);
    return;
  }

  // Set steps per digit: s <n>
  if (cmd.startsWith("s ")) {
    int n = cmd.substring(2).toInt();
    if (n <= 0) { Serial.println("  ⚠️  Invalid steps value"); return; }
    Serial.printf("[%s] ", motorNames[selectedMotor]);
    motors[selectedMotor]->set_steps_per_digit(n);
    return;
  }

  // Batch: home all
  if (cmd == "home") {
    Serial.println("🏠 Homing ALL motors...");
    for (int i = 0; i < 4; i++) {
      Serial.printf("  [%s] ", motorNames[i]);
      motors[i]->go_home();
    }
    Serial.println("✅ All motors at home.");
    return;
  }

  // Batch: drive all to max
  if (cmd == "max") {
    Serial.println("🔝 Driving ALL motors to max...");
    for (int i = 0; i < 4; i++) {
      Serial.printf("  [%s] ", motorNames[i]);
      motors[i]->go_to(motors[i]->getMax());
    }
    Serial.println("✅ All motors at max.");
    return;
  }

  // Batch: set HHMM
  if (cmd.startsWith("set ") && cmd.length() == 8) {
    String digits = cmd.substring(4);
    int d0 = digits[0] - '0';
    int d1 = digits[1] - '0';
    int d2 = digits[2] - '0';
    int d3 = digits[3] - '0';

    if (d0 < 0 || d0 > 2 || d1 < 0 || d1 > 9 ||
        d2 < 0 || d2 > 5 || d3 < 0 || d3 > 9) {
      Serial.println("  ⚠️  Invalid. Use set HHMM (e.g. set 0930)");
      return;
    }

    Serial.printf("🕒 Setting display → %d%d:%d%d\n", d0, d1, d2, d3);
    H1.go_to(d0); H2.go_to(d1);
    M1.go_to(d2); M2.go_to(d3);
    Serial.println("✅ Display set.");
    return;
  }

  Serial.println("  ❓ Unknown command — type ? for help");
}


// ============================================================
// ----- Setup -----
// ============================================================
void setup()
{
  Serial.begin(115200);
  M2.raw_steps(2);  //  nudge back by boot drift amount
  M1.raw_steps(2);  //  nudge back by boot drift amount
  H2.raw_steps(2);  //  nudge back by boot drift amount
  H1.raw_steps(2);  //  nudge back by boot drift amount
  delay(500);
  printHelp();
  Serial.printf("🟢 Active motor: %s  (type 1–4 to switch)\n\n", motorNames[selectedMotor]);
}


// ============================================================
// ----- Loop -----
// ============================================================
void loop()
{
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');
    handleCommand(cmd);
    Serial.printf("\n[Active: %s | pos=%d]\n",
      motorNames[selectedMotor],
      motors[selectedMotor]->getPos()
    );
  }
}

// ============================================================
// ----- The End -----
// ============================================================
