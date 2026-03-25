// ============================================================
//  _   _    _    ____  _   _ _____   _     ____ 
// | | | |  / \  / ___|| | | |_   _| / \   / ___|
// | |_| | / _ \ \___ \| |_| | | |  / _ \ | |  _ 
// |  _  |/ ___ \ ___) |  _  | | | / ___ \| |_| |
// |_| |_/_/   \_\____/|_| |_| |_|/_/   \_\\____|
//
//  Click Clock — Interactive Motor Test
//  Serial Monitor → type commands → motors move live
// ============================================================

/*
how to calibrate and transfer the value:

Step 1 — find the right number in MotorTest_Interactive
Flash the interactive test, then for each motor:
Type g 1 — motor should land exactly on digit 1,
If it overshoots → type s 540 (lower value), h to home, g 1 again,
If it undershoots → type s 580 (higher value), h to home, g 1 again,
Repeat until it lands perfectly
Type i to see the final spd value.

Step 2 — copy that one value into ClickClock
In your main clock, find this line:
//cppstatic const int STEPS_PER_DIGIT = 560;  // ← CALIBRATE THIS
Replace 560 with whatever number you found. 
Since the main clock uses one shared value for all motors, 
use the average, or the most important motor 
(M2 — minute units — changes most often).
*/



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

  static const int STEPS_PER_REVOLUTION = 2038;

  Stepper stepper;

public:
  // dir = +1 or -1 | max = max digit | spd = steps per digit | then IN1..IN4
  ClockDigit(int8_t dir, uint8_t maxDig, int spd,
             int blu, int yel, int pnk, int ora)
    : invert_direction(dir),
      max_digit(maxDig),
      STEPS_PER_DIGIT(spd),
      stepper(STEPS_PER_REVOLUTION, blu, yel, pnk, ora)
  {
    stepper.setSpeed(10);
  }

  // ---- Move commands ----

  void up_one()
  {
    if (current_digit >= max_digit) {
      Serial.println("  ⚠️  Already at MAX");
      return;
    }
    stepper.step(invert_direction * STEPS_PER_DIGIT);
    current_digit++;
    Serial.printf("  ⬆️  UP → digit %d\n", current_digit);
  }

  void down_one()
  {
    if (current_digit <= 0) {
      Serial.println("  ⚠️  Already at 0");
      return;
    }
    stepper.step(invert_direction * -STEPS_PER_DIGIT);
    current_digit--;
    Serial.printf("  ⬇️  DOWN → digit %d\n", current_digit);
  }

  void go_to(uint8_t target)
  {
    if (target > max_digit) {
      Serial.printf("  ⚠️  Target %d exceeds max %d\n", target, max_digit);
      return;
    }
    int delta = (int)target - (int)current_digit;
    if (delta == 0) {
      Serial.printf("  ✅ Already at digit %d\n", current_digit);
      return;
    }
    stepper.step(invert_direction * STEPS_PER_DIGIT * delta);
    current_digit = target;
    Serial.printf("  🎯 Moved → digit %d\n", current_digit);
  }

  void go_home()
  {
    Serial.printf("  🏠 Homing from digit %d...\n", current_digit);
    stepper.step(invert_direction * STEPS_PER_DIGIT * (-current_digit));
    current_digit = 0;
    Serial.println("  ✅ At home (0)");
  }

  void raw_steps(int steps)
  {
    Serial.printf("  🔧 Raw: %d steps\n", steps);
    stepper.step(steps);
    Serial.println("  ✅ Done (position NOT tracked for raw moves)");
  }

  void flip_direction()
  {
    invert_direction *= -1;
    Serial.printf("  🔄 Direction flipped → now %+d\n", invert_direction);
  }

  void set_steps_per_digit(int spd)
  {
    STEPS_PER_DIGIT = spd;
    Serial.printf("  ⚙️  Steps/digit updated → %d\n", spd);
  }

  void status()
  {
    Serial.printf(
      "  📍 pos=%d  max=%d  spd=%d  dir=%+d\n",
      current_digit, max_digit, STEPS_PER_DIGIT, invert_direction
    );
  }

  // Getters
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
ClockDigit H1( 1,   2,  560,  16,  17,   5,  18);  // Hour tens   (0–2)
ClockDigit H2(-1,   9,  560,  19,  21,  22,  23);  // Hour units  (0–9)
ClockDigit M1( 1,   5,  560,  25,  26,  27,  14);  // Min  tens   (0–5)
ClockDigit M2(-1,   9,  560,  12,  13,  32,  33);  // Min  units  (0–9)


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
  M1.raw_steps(4);  //  nudge back by boot drift amount
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
