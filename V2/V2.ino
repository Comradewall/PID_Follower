//Cod pentru Hackathonul FIMM(V0.1)
//Programator: https://github.com/Comradewall
//Beta subject to changes

//Define statements for constants(e.g. pin numbers, speed of sound, etc.)

#include <QTRSensors.h>
QTRSensors qtr;

const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];


#define Trig_1  2
#define Trig_2  3
#define Echo_1  4
#define Echo_2  5
#define MOTOR_L1  6
#define MOTOR_L2  7
#define MOTOR_R1  8
#define MOTOR_R2  9

const int Kd = 3;
const int Kp = 8;
const int PozRef = 10;
const int viteza_croaziera = 70;

float Position;
int eroare;
int eroare_ant;
int derivativ;
int commanda;

int vitezaMS;
int vitezaMD;

float LowerLimit = 0;
float HigherLimit = 20;

#define SpeedOfSound 0.034

void setup() {
  
  Serial.begin(9600);

  //Initialize Pins for use:

  pinMode(Trig_1, OUTPUT);
  pinMode(Trig_2, OUTPUT);
  pinMode(Echo_1, INPUT);
  pinMode(Echo_2, INPUT);
  pinMode(MOTOR_L1, OUTPUT);
  pinMode(MOTOR_L2, OUTPUT);
  pinMode(MOTOR_R1, OUTPUT);
  pinMode(MOTOR_R2, OUTPUT);

  //Init Sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){12,11,10}, SensorCount);

  delay(500);

  Calibration(250);

}

void loop() {

  float UltrasonicSensorOutput = DistanceRelativeToCenter();
  float ReflactanceArray = QTRSensorRead();
  
  // If else loop that refuses to work goes here

  Position = ReflactanceArray;
  
  eroare = PozRef - Position;
  derivativ = eroare - eroare_ant;
  commanda = Kp * eroare + Kd * derivativ;
  
  vitezaMS = viteza_croaziera + commanda;
  vitezaMD = viteza_croaziera - commanda;

  vitezaMS = (vitezaMS > 255) ? 255 : ((vitezaMS < -255) ? -255 : vitezaMS);
  vitezaMD = (vitezaMD > 255) ? 255 : ((vitezaMD < -255) ? -255 : vitezaMD);

  go(vitezaMS,vitezaMD);
  eroare_ant = eroare;

  //Serial.println(Position);

}
float DistanceRelativeToCenter() {

  int duration;
  int Distance_L;
  float RelativePosition;
  int Distance_R;

  //Reset Trig Pin
  digitalWrite(Trig_1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(Trig_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_1, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(Echo_1, HIGH);
  // Calculating the distance (cm)
  Distance_L = duration * SpeedOfSound / 2;

  digitalWrite(Trig_2, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_2, LOW);
  duration = pulseIn(Echo_2, HIGH);
  Distance_R = duration * SpeedOfSound / 2;

  RelativePosition = float(Distance_L) / float(Distance_R);

  //Serial.println(RelativePosition);
  RelativePosition = RelativePosition * 10.00;
  return RelativePosition;

}

float QTRSensorRead() {

  uint16_t position = qtr.readLineBlack(sensorValues)/100;

  return position;

}

void go(int SpeedLeft, int SpeedRight) {
  if (SpeedLeft > 0) {
    analogWrite(MOTOR_R2, SpeedLeft);
    analogWrite(MOTOR_R1, 0);
  } 
  else {
    analogWrite(MOTOR_R2, 0);
    analogWrite(MOTOR_R1, -SpeedLeft);
  }
 
  if (SpeedRight > 0) {
    analogWrite(MOTOR_L1, SpeedRight);
    analogWrite(MOTOR_L2, 0);
  }
  else {
    analogWrite(MOTOR_L1, 0);
    analogWrite(MOTOR_L2, -SpeedRight);
  }
}

void Calibration(int Passes) {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);     // Calibration LED

  for (uint16_t i = 0; i < Passes; i++) {
      qtr.calibrate();
  }

  digitalWrite(13, LOW); //Finn Cal.
}
