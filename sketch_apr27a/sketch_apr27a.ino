// Core includes and definitions
#include <AccelStepper.h>

// Communication settings
#define BAUDRATE 9600  // Serial baud rate

// Stepper drivers (STEP pin, DIR pin)
AccelStepper stepperX(AccelStepper::DRIVER, 54, 55);
AccelStepper stepperY(AccelStepper::DRIVER, 60, 61);
AccelStepper stepperZ(AccelStepper::DRIVER, 46, 48);
AccelStepper stepperE0(AccelStepper::DRIVER, 26, 28);

// Driver enable pins (enable = LOW to activate)
#define X_ENABLE_PIN 38
#define Y_ENABLE_PIN 56
#define Z_ENABLE_PIN 62
#define E0_ENABLE_PIN 24

// Invert direction in software if axis wiring or logic differs
#define INV_X false
#define INV_Y false
#define INV_Z false
#define INV_E0 false

#define SPEED 6400   // Base movement speed (steps/sec)

String inputString = "";
unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 2000; // 2 seconds

// Enum for parsed commands
enum Command {
  CMD_STOP, CMD_N, CMD_S, CMD_E, CMD_W,
  CMD_NE, CMD_NW, CMD_SE, CMD_SW,
  CMD_COUNT,  // Number of valid commands
  CMD_INVALID = -1
};

// Structure to hold speeds for each axis
typedef struct {
  int16_t sx;
  int16_t sy;
  int16_t sz;
  int16_t se;
} AxisSpeeds;

// Lookup table: index by Command
const AxisSpeeds speedTable[CMD_COUNT] = {
  // sx,    sy,    sz,     se
  { 0,      0,      0,      0     }, // STOP
  { SPEED,  SPEED, -SPEED, -SPEED }, // N
  {-SPEED, -SPEED,  SPEED,  SPEED }, // S
  {-SPEED,  SPEED, -SPEED,  SPEED }, // E
  { SPEED, -SPEED,  SPEED, -SPEED }, // W
  { 0,      SPEED, -SPEED,  0     }, // NE
  { SPEED,  0,      0,     -SPEED }, // NW
  {-SPEED,  0,      0,      SPEED }, // SE
  { 0,     -SPEED,  SPEED,  0     }  // SW
};

void setup() {
  Serial.begin(BAUDRATE);

  // Configure and activate drivers
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(Z_ENABLE_PIN, LOW);
  digitalWrite(E0_ENABLE_PIN, LOW);

  // Set maximum speed per axis
  stepperX.setMaxSpeed(SPEED);
  stepperY.setMaxSpeed(SPEED);
  stepperZ.setMaxSpeed(SPEED);
  stepperE0.setMaxSpeed(SPEED);

  // Disable built-in inversion; we handle it in software
  stepperX.setPinsInverted(false, false, false);
  stepperY.setPinsInverted(false, false, false);
  stepperZ.setPinsInverted(false, false, false);
  stepperE0.setPinsInverted(false, false, false);

  // Initialize motors stopped
  executeCommand(CMD_STOP);
}

void loop() {
  // Read serial input until newline
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      String cmdStr = inputString;
      cmdStr.trim();
      Command cmd = parseCommand(cmdStr);
      if (cmd == CMD_INVALID) {
        Serial.print("Invalid command: "); Serial.println(cmdStr);
      } else {
        executeCommand(cmd);
      }
      inputString = "";
    } else {
      inputString += c;
    }
  }

  // Run steppers continuously
  stepperX.runSpeed();
  stepperY.runSpeed();
  stepperZ.runSpeed();
  stepperE0.runSpeed();

  // Periodic status print
  if (millis() - lastStatusTime >= statusInterval) {
    printStatus();
    lastStatusTime = millis();
  }
}

// Parse text to Command enum
Command parseCommand(const String& s) {
  if (s == "STOP") return CMD_STOP;
  if (s == "N")    return CMD_N;
  if (s == "S")    return CMD_S;
  if (s == "E")    return CMD_E;
  if (s == "W")    return CMD_W;
  if (s == "NE")   return CMD_NE;
  if (s == "NW")   return CMD_NW;
  if (s == "SE")   return CMD_SE;
  if (s == "SW")   return CMD_SW;
  return CMD_INVALID;
}

// Execute a parsed command by looking up speeds and applying inversion
void executeCommand(Command cmd) {
  if (cmd < 0 || cmd >= CMD_COUNT) return;
  AxisSpeeds sp = speedTable[cmd];

  // Apply software inversion
  sp.sx *= (INV_X ? -1 : 1);
  sp.sy *= (INV_Y ? -1 : 1);
  sp.sz *= (INV_Z ? -1 : 1);
  sp.se *= (INV_E0 ? -1 : 1);

  // Send speeds to motors
  stepperX.setSpeed(sp.sx);
  stepperY.setSpeed(sp.sy);
  stepperZ.setSpeed(sp.sz);
  stepperE0.setSpeed(sp.se);
}

// Print current speeds to Serial
void printStatus() {
  Serial.print("Status - X: "); Serial.print(stepperX.speed());
  Serial.print(", Y: "); Serial.print(stepperY.speed());
  Serial.print(", Z: "); Serial.print(stepperZ.speed());
  Serial.print(", E: "); Serial.println(stepperE0.speed());
}
