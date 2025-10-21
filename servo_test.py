import RPi.GPIO as GPIO
import time

# === SERVO SETUP ===
GPIO.setmode(GPIO.BCM)

# Define your servo pins
servo_pins = [17, 13, 22, 23]  # FL, FR, BL, BR (same as before)

# PWM parameters
PWM_FREQ = 50  # 50Hz for standard servos
NEUTRAL_DUTY_CYCLE = 7.5  # 7.5% for approximately 0 degrees (you may need slight tuning)

# Initialize servos
servos = []
for pin in servo_pins:
    GPIO.setup(pin, GPIO.OUT)
    pwm = GPIO.PWM(pin, PWM_FREQ)
    pwm.start(NEUTRAL_DUTY_CYCLE)
    servos.append(pwm)

print("Setting all servos to 0 degrees...")
time.sleep(2)  # Hold position for 2 seconds

# Cleanup
for pwm in servos:
    pwm.stop()

GPIO.cleanup()
print("Done!")
