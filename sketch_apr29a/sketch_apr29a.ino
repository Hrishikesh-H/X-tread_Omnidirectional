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

#define SPEED 3200  // Constant movement speed

String inputString = "";
String defaultCommand = "STOP";

unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 2000; // 2 seconds

void setup() {
  Serial.begin(9600);

  // Configure enable pins
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);

  // Activate drivers (LOW = enabled; change to HIGH if yours invert)
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(Z_ENABLE_PIN, LOW);
  digitalWrite(E0_ENABLE_PIN, LOW);

  // Set maximum speed for each axis
  stepperX.setMaxSpeed(SPEED);
  stepperY.setMaxSpeed(SPEED);
  stepperZ.setMaxSpeed(SPEED);
  stepperE0.setMaxSpeed(SPEED);

  // If Y still spins in same direction, invert its DIR logic here:
  stepperY.setPinsInverted(false, true, false);  // (invertDirection, invertStep, invertEnable)

  // Ensure all motors are stopped on startup
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

  // Run steppers at current speeds
  stepperX.runSpeed();
  stepperY.runSpeed();
  stepperZ.runSpeed();
  stepperE0.runSpeed();

  // Periodic status output
  if (millis() - lastStatusTime >= statusInterval) {
    printStepperStatus();
    lastStatusTime = millis();
  }
}

bool applyCommand(String command) {
  command.trim();

  // STOP: zero all speeds
  if (command == "STOP") {
    stepperX.setSpeed(0);
    stepperY.setSpeed(0);
    stepperZ.setSpeed(0);
    stepperE0.setSpeed(0);
    return true;
  }

  // Cardinal directions
  if (command == "N") {
    stepperX.setSpeed(SPEED);
    stepperY.setSpeed(SPEED);
    stepperZ.setSpeed(-SPEED);
    stepperE0.setSpeed(-SPEED);
  } else if (command == "S") {
    stepperX.setSpeed(-SPEED);
    stepperY.setSpeed(-SPEED);
    stepperZ.setSpeed(SPEED);
    stepperE0.setSpeed(SPEED);
  } else if (command == "E") {
    stepperX.setSpeed(-SPEED);
    stepperY.setSpeed(SPEED);
    stepperZ.setSpeed(-SPEED);
    stepperE0.setSpeed(SPEED);
  } else if (command == "W") {
    stepperX.setSpeed(SPEED);
    stepperY.setSpeed(-SPEED);
    stepperZ.setSpeed(SPEED);
    stepperE0.setSpeed(-SPEED);
  }
  // Diagonals
  else if (command == "NE") {
    stepperX.setSpeed(0);
    stepperY.setSpeed(SPEED);
    stepperZ.setSpeed(-SPEED);
    stepperE0.setSpeed(0);
  } else if (command == "NW") {
    stepperX.setSpeed(SPEED);
    stepperY.setSpeed(0);
    stepperZ.setSpeed(0);
    stepperE0.setSpeed(-SPEED);
  } else if (command == "SE") {
    stepperX.setSpeed(-SPEED);
    stepperY.setSpeed(0);
    stepperZ.setSpeed(0);
    stepperE0.setSpeed(SPEED);
  } else if (command == "SW") {
    stepperX.setSpeed(0);
    stepperY.setSpeed(-SPEED);
    stepperZ.setSpeed(SPEED);
    stepperE0.setSpeed(0);
  } else {
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
