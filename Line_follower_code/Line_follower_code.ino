#include <QTRSensors.h>
QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
int MOTOR2_PIN1 = 3;
int MOTOR2_PIN2 = 5;
int MOTOR1_PIN1 = 9;
int MOTOR1_PIN2 = 6;
int vitezaMS;
int vitezaMD;

int viteza_croaziera=225;      // se creste pana la maxim 255;
int poz_ref=35;                //Poz ref a sens fata de linie and shit tm (Offset left right 35-center)
int Kp=5;                      //se poate modifica  (acceleratie) valoare prea mare duce la instabilitate
int Kd=20;                     //se poate modifica   (frana/redresare) valoare prea mare duce la instabilitate
int eroare = 0;
int eroare_ant = 0;
int derivativ = 0;
int comanda;

void setup()
{

  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  
  // configure the sensors
  
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);
  qtr.setEmitterPin(2);

  delay(500);

  Calibration(250);
}

void loop() {
    uint16_t position = qtr.readLineBlack(sensorValues)/100;
  
    eroare=poz_ref-position;
    derivativ=eroare-eroare_ant;
    comanda=Kp*eroare+Kd*derivativ;
  
    vitezaMS=viteza_croaziera-comanda;
    vitezaMD=viteza_croaziera+comanda;

    vitezaMS = (vitezaMS > 255) ? 255 : ((vitezaMS < -255) ? -255 : vitezaMS);
    vitezaMD = (vitezaMD > 255) ? 255 : ((vitezaMD < -255) ? -255 : vitezaMD);

    go(vitezaMS,vitezaMD);
    eroare_ant=eroare;
}

//Function go should be improved in later versions when I get to it tm.

void go(int speedLeft, int speedRight) {
  if (speedLeft > 0) {
    analogWrite(MOTOR1_PIN1, speedLeft);
    analogWrite(MOTOR1_PIN2, 0);
  } 
  else {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -speedLeft);
  }
 
  if (speedRight > 0) {
    analogWrite(MOTOR2_PIN1, speedRight);
    analogWrite(MOTOR2_PIN2, 0);
  }else {
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, -speedRight);
  }
}

void Calibration(int passes) {
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Cal. On LED

  for (uint16_t i = 0; i < passes; i++)
  { 
    qtr.calibrate();
  }

  digitalWrite(LED_BUILTIN, LOW); // Cal. Off LED

}

