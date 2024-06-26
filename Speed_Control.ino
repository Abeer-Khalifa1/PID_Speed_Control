#include <PID_v1_bc.h>
int trig = 12;
int echo = 5;

int speedL = 11;
int IN1 = 10;
int IN2 = 9;
int IN3 = 8;
int IN4 = 7;
int speedR = 6;

long duration, distance;

unsigned long previousMillis;
unsigned long currentMillis;
unsigned long currentMicros;
unsigned long previousMicros = 0;
volatile int countR = 0;
volatile int countL = 0;
double slots = 20;
double rpmL = 1;
double rpmR = 1;
int pwmR = 150;
int pwmL = 150;
int pwml = 150;  //left
int pwm2 = 150;  //right
double MaxRPM_left = 360;
double MaxRPM_right = 850;

double setpoint = 0, Input1, Output1;
double kp = 0.0124, ki = 0.000121, kd = 0.277;
PID control(&Input1, &Output1, &setpoint, kp, ki, kd, REVERSE);

void counterR() {
  countR++;
}

void counterL() {
  countL++;
}

void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(speedL, pwml);
  analogWrite(speedR, pwm2);
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(speedL, pwml);
  analogWrite(speedR, pwm2);
}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(speedL, pwml);
  analogWrite(speedR, 0);
}


void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(speedL, 0);
  analogWrite(speedR, pwm2);
}

void stop() {
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
}

void setup() {
  Serial.begin(9600);
  previousMillis = millis();
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), counterR, RISING);
  attachInterrupt(digitalPinToInterrupt(3), counterL, RISING);
  control.SetMode(AUTOMATIC);
}

void loop() {
  currentMillis = millis();
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration / 2) * 0.0343;
  Serial.print("distance");
  Serial.println(distance);
  delay(5);

  if ((currentMillis - previousMillis) >= 1000) {
    previousMillis = currentMillis;
    Serial.print("countR=");
    Serial.println(countR);
    noInterrupts();
    int countR_copy = countR;
    int countL_copy = countL;
    interrupts();
    rpmR = (countR_copy / slots) * 60;
    rpmL = (countL_copy / slots) * 60;
    Input1 = rpmL - rpmR;
    control.Compute();

    if (Output1 >= 0 && distance > 30) {
      pwml = constrain(150 - Output1 / 2, 0, 255);
      pwm2 = constrain(150 + Output1 / 2, 0, 255);
      forward();
      Serial.print("pwml= ");
      Serial.println(pwml);
      Serial.print("pwm2= ");
      Serial.println(pwm2);
    }

    else if (Output1 >= 0 && distance <= 30) {
      pwml = constrain(150 - Output1 / 2, 0, 255);
      pwm2 = constrain(150 + Output1 / 2, 0, 255);
      left();
      Serial.print("pwm1= ");
      Serial.println(pwml);
      Serial.print("pwm2= ");
      Serial.println(pwm2);
    }

    else if (Output1 < 0 && distance > 30) {
      pwml = constrain(150 + Output1 / 2, 0, 255);
      pwm2 = constrain(150 - Output1 / 2, 0, 255);
      forward();
      Serial.print("pwm1= ");
      Serial.println(pwml);
      Serial.print("pwm2= ");
      Serial.println(pwm2);
    }

    else if (Output1 < 0 && distance <= 30) {
      pwml = constrain(150 + Output1 / 2, 0, 255);
      pwm2 = constrain(150 - Output1 / 2, 0, 255);
      left();
      Serial.print("pwml= ");
      Serial.println(pwml);
      Serial.print("pwm2= ");
      Serial.println(pwm2);
    }
    Serial.print("rpmR=");
    Serial.println(rpmR);
    Serial.print("rpmL=");
    Serial.println(rpmL);
    countL = 0;
    countR = 0;
  }

  else {
    analogWrite(speedL, pwml);
    analogWrite(speedR, pwm2);
  }
}