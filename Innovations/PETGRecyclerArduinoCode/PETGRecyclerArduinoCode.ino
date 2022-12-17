//Librares
#include <PID_v1.h>
#include <AccelStepper.h>
//PINS
#define ThermistorPIN A0
#define RelayHotendSet 2

float Fahrenheit;
float FahrenheitX;
float celsius;
float celsiusX;
int Vout;
float R1 = 10000;
float logR2, R2, Temp;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

double Setpoint, Input, Output;

//STEPER DRIVER
#define dirPin1 6
#define stepPin1 5

#define dirPin2 11
#define stepPin2 10

#define motorInterfaceType1 1
#define motorInterfaceType2 1

PID myPID(&Input, &Output, &Setpoint, 50, 10, 1, REVERSE);
AccelStepper stepper1 = AccelStepper(motorInterfaceType1, stepPin1, dirPin1);
AccelStepper stepper2 = AccelStepper(motorInterfaceType2, stepPin2, dirPin2);

void ThermistorToCelsius() {
  for (int i = 0; i < 10; i++) {
    Vout = analogRead(ThermistorPIN);
    R2 = R1 * (1023.0 / (float)Vout - 1.0);
    logR2 = log(R2);
    Temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    celsiusX += Temp - 273.15;
  }
  celsius = celsiusX / 10;
  celsiusX = 0;
  Fahrenheit = ((celsius * 1.8) + 32.0);// 1.8 = 9/5
}

void setup() {
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 1);
  Setpoint = 260;

  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);

  stepper1.setSpeed(1000);
  stepper2.setSpeed(2.5);

  pinMode(ThermistorPIN, INPUT);
  pinMode(RelayHotendSet, OUTPUT);
}

void loop() {
  ThermistorToCelsius();
  Input = celsius;
  myPID.Compute();

  if (celsius > 250) {
    stepper1.runSpeed();
    stepper2.runSpeed();
  }
  digitalWrite(RelayHotendSet, Output);

  Serial.print("Celsius:");
  Serial.println(celsius);
  Serial.print("Fahrenheit:");
  Serial.println(Fahrenheit);
  Serial.print("Relay:");
  Serial.println(Output);
  Serial.println("");
}
