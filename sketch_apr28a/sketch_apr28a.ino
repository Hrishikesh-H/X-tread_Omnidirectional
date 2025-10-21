#include <Servo.h>

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  servo0.attach(11); // RAMPS AUX-3 slot 1
  servo1.attach(6);  // RAMPS AUX-3 slot 2
  servo2.attach(5);  // RAMPS AUX-3 slot 3
  servo3.attach(4);  // RAMPS AUX-3 slot 4
  
  delay(1000); // wait for power stabilization
  
  servo0.write(0);
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  
  delay(1000);
}

void loop() {
  // sweep all servos 0 -> 180
  for (int pos = 0; pos <= 180; pos += 5) {
    servo0.write(pos);
    servo1.write(pos);
    servo2.write(pos);
    servo3.write(pos);
    delay(20);
  }
  
  // sweep back 180 -> 0
  for (int pos = 180; pos >= 0; pos -= 5) {
    servo0.write(pos);
    servo1.write(pos);
    servo2.write(pos);
    servo3.write(pos);
    delay(20);
  }
}
