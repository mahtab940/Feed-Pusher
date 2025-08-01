# Here is the code for line censor of two motors
// Shift register control pins
const int MOTORLATCH  = 12;
const int MOTORCLK    = 4;
const int MOTORENABLE = 7;
const int MOTORDATA   = 8;
 
// PWM pins for motors
const int M1_PWM = 11;
const int M2_PWM = 3;
const int M3_PWM = 6;
const int M4_PWM = 5;
 
// Bit positions in 74HC595 output (Q0 to Q7)
const uint8_t M1_IN1_BIT = 2;  // Q2
const uint8_t M1_IN2_BIT = 3;  // Q3
const uint8_t M2_IN1_BIT = 1;  // Q1
const uint8_t M2_IN2_BIT = 4;  // Q4
const uint8_t M3_IN1_BIT = 5;  // Q0
const uint8_t M3_IN2_BIT = 7;  // Q6
const uint8_t M4_IN1_BIT = 0;  // Q5
const uint8_t M4_IN2_BIT = 6;  // Q7
 
// Line sensors
const int SENSOR_LEFT  = A0;
const int SENSOR_RIGHT = A1;
 
uint8_t motorStates = 0;  // Holds the bits for the shift register output
 
// Update shift register with current motorStates
void updateShiftRegister() {
  digitalWrite(MOTORLATCH, LOW);
  shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, motorStates);
  digitalWrite(MOTORLATCH, HIGH);
}
 
// Set motor direction bits (does NOT update shift register immediately)
void setMotorDirection(uint8_t in1Bit, uint8_t in2Bit, char dir) {
  if (dir == 'F') {
    motorStates |=  (1 << in1Bit);
    motorStates &= ~(1 << in2Bit);
  } else if (dir == 'B') {
    motorStates &= ~(1 << in1Bit);
    motorStates |=  (1 << in2Bit);
  } else { // Stop
    motorStates &= ~(1 << in1Bit);
    motorStates &= ~(1 << in2Bit);
  }
}
 
// Control all 4 motors at once
void driveAll(char dir, int speed) {
  setMotorDirection(M1_IN1_BIT, M1_IN2_BIT, dir);
  setMotorDirection(M2_IN1_BIT, M2_IN2_BIT, dir);
  setMotorDirection(M3_IN1_BIT, M3_IN2_BIT, dir);
  setMotorDirection(M4_IN1_BIT, M4_IN2_BIT, dir);
  updateShiftRegister();
 
  if (dir == 'S') {
    analogWrite(M1_PWM, 0);
    analogWrite(M2_PWM, 0);
    analogWrite(M3_PWM, 0);
    analogWrite(M4_PWM, 0);
  } else {
    analogWrite(M1_PWM, speed);
    analogWrite(M2_PWM, speed);
    analogWrite(M3_PWM, speed);
    analogWrite(M4_PWM, speed);
  }
}
 
void setup() {
  // Setup pins
  pinMode(MOTORLATCH, OUTPUT);
  pinMode(MOTORCLK, OUTPUT);
  pinMode(MOTORDATA, OUTPUT);
  pinMode(MOTORENABLE, OUTPUT);
  digitalWrite(MOTORENABLE, LOW);  // Enable motor outputs
 
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M3_PWM, OUTPUT);
  pinMode(M4_PWM, OUTPUT);
 
  pinMode(SENSOR_LEFT, INPUT);
  pinMode(SENSOR_RIGHT, INPUT);
 
  Serial.begin(9600);
  Serial.println("System ready. Watching line sensors...");
}
 
void loop() {
  int leftSensor  = digitalRead(SENSOR_LEFT);   // LOW = line detected
  int rightSensor = digitalRead(SENSOR_RIGHT);  // LOW = line detected
 
  Serial.print("L: ");
  Serial.print(leftSensor);
  Serial.print(" | R: ");
  Serial.println(rightSensor);
 
  if (leftSensor == LOW && rightSensor == LOW) {
    // Both sensors see line → move forward
    Serial.println("Forward");
    driveAll('F', 180);
  } else if (leftSensor == LOW && rightSensor == HIGH) {
    // Left sees line → turn left
    Serial.println("Turning Left");
    setMotorDirection(M1_IN1_BIT, M1_IN2_BIT, 'F');  // Left front
    setMotorDirection(M2_IN1_BIT, M2_IN2_BIT, 'F');  // Left back
    setMotorDirection(M3_IN1_BIT, M3_IN2_BIT, 'S');  // Stop right front
    setMotorDirection(M4_IN1_BIT, M4_IN2_BIT, 'S');  // Stop right back
    updateShiftRegister();
    analogWrite(M1_PWM, 180);
    analogWrite(M2_PWM, 180);
    analogWrite(M3_PWM, 0);
    analogWrite(M4_PWM, 0);
  } else if (leftSensor == HIGH && rightSensor == LOW) {
    // Right sees line → turn right
    Serial.println("Turning Right");
    setMotorDirection(M1_IN1_BIT, M1_IN2_BIT, 'S');
    setMotorDirection(M2_IN1_BIT, M2_IN2_BIT, 'S');
    setMotorDirection(M3_IN1_BIT, M3_IN2_BIT, 'F');
    setMotorDirection(M4_IN1_BIT, M4_IN2_BIT, 'F');
    updateShiftRegister();
    analogWrite(M1_PWM, 0);
    analogWrite(M2_PWM, 0);
    analogWrite(M3_PWM, 180);
    analogWrite(M4_PWM, 180);
  } else {
    // No line detected → stop
    Serial.println("Stop");
    driveAll('S', 0);
  }
 
  delay(10);
}

## Here is the video of my work

[▶️ Watch the video](pic/A1.mp4)