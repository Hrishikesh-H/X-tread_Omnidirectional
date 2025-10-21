#include <AccelStepper.h>

// Stepper drivers (STEP pin, DIR pin)
AccelStepper stepperX(AccelStepper::DRIVER, 54, 55);
AccelStepper stepperY(AccelStepper::DRIVER, 60, 61);
AccelStepper stepperZ(AccelStepper::DRIVER, 46, 48);
AccelStepper stepperE0(AccelStepper::DRIVER, 26, 28);

// Driver enable pins (enable = LOW to activate by default)
#define X_ENABLE_PIN 38
#define Y_ENABLE_PIN 56
#define Z_ENABLE_PIN 62
#define E0_ENABLE_PIN 24

// Tuned for full-step mode – adjust as needed
#define SPEED        500     // full-steps per second
#define ACCELERATION 1000     // full-steps per second^2
#define DISTANCE     1000000L // a “far” target for continuous motion

String inputString = "";
String defaultCommand = "STOP";

unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 2000; // 2 seconds

void setup() {
  Serial.begin(9600);

  // Configure and activate drivers
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(Z_ENABLE_PIN, LOW);
  digitalWrite(E0_ENABLE_PIN, LOW);

  // Set max speed & acceleration on each axis
  stepperX.setMaxSpeed(SPEED);
  stepperX.setAcceleration(ACCELERATION);
  stepperY.setMaxSpeed(SPEED);
  stepperY.setAcceleration(ACCELERATION);
  stepperZ.setMaxSpeed(SPEED);
  stepperZ.setAcceleration(ACCELERATION);
  stepperE0.setMaxSpeed(SPEED);
  stepperE0.setAcceleration(ACCELERATION);

  // If Y still spins the wrong way, invert its DIR logic here:
  stepperY.setPinsInverted(false, true, false);

  // All axes stopped on startup
  applyCommand(defaultCommand);
}

void loop() {
  // Read incoming token
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      if (!applyCommand(inputString)) {
        Serial.println("Invalid command. Using default: STOP");
        applyCommand(defaultCommand);
      }
      printStepperStatus();
      inputString = "";
    } else {
      inputString += inChar;
    }
  }

  // Let AccelStepper handle acceleration & stepping
  stepperX.run();
  stepperY.run();
  stepperZ.run();
  stepperE0.run();

  // Periodic status output
  if (millis() - lastStatusTime >= statusInterval) {
    printStepperStatus();
    lastStatusTime = millis();
  }
}

bool applyCommand(String command) {
  command.trim();

  // STOP: decelerate to 0
  if (command == "STOP") {
    stepperX.stop();
    stepperY.stop();
    stepperZ.stop();
    stepperE0.stop();
    return true;
  }

  // For each direction we set a “far” target so run() will ramp and hold speed
  long pos;
  if (command == "N") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos +  DISTANCE);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos +  DISTANCE);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos -  DISTANCE);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos -  DISTANCE);
  }
  else if (command == "S") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos -  DISTANCE);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos -  DISTANCE);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos +  DISTANCE);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos +  DISTANCE);
  }
  else if (command == "E") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos -  DISTANCE);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos +  DISTANCE);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos -  DISTANCE);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos +  DISTANCE);
  }
  else if (command == "W") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos +  DISTANCE);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos -  DISTANCE);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos +  DISTANCE);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos -  DISTANCE);
  }
  // Diagonals
  else if (command == "NE") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos);            // X idle
    pos = stepperY.currentPosition();  stepperY.moveTo(pos +  DISTANCE);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos -  DISTANCE);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos);            // E idle
  }
  else if (command == "NW") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos +  DISTANCE);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos -  DISTANCE);
  }
  else if (command == "SE") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos -  DISTANCE);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos +  DISTANCE);
  }
  else if (command == "SW") {
    pos = stepperX.currentPosition();  stepperX.moveTo(pos);
    pos = stepperY.currentPosition();  stepperY.moveTo(pos -  DISTANCE);
    pos = stepperZ.currentPosition();  stepperZ.moveTo(pos +  DISTANCE);
    pos = stepperE0.currentPosition(); stepperE0.moveTo(pos);
  }
  else {
    return false;
  }

  return true;
}

void printStepperStatus() {
  Serial.print("Status - ");
  Serial.print("X: "); Serial.print(stepperX.speed());
  Serial.print(", Y: "); Serial.print(stepperY.speed());
  Serial.print(", Z: "); Serial.print(stepperZ.speed());
  Serial.print(", E: "); Serial.println(stepperE0.speed());
}
