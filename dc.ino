
#include <Servo.h>
#include <NewPing.h>

#define COLORSETUP 100

#define  TRIGGER_PIN  6
#define  ECHO_PIN     5
#define MAX_DISTANCE 200 

int topRight = A0;
int topLeft = A1;
int bottomRight = A2;
int bottomLeft = A3;

int homeColorTR = 0;
int homeColorTL = 0;
int homeColorBR = 0;
int homeColorBL = 0;

int sensorTR = 0;
int sensorTL = 0;
int sensorBR = 0;
int sensorBL = 0;
int colorSetupTR = 0;
int colorSetupTL = 0;
int colorSetupBR = 0;
int colorSetupBL = 0;


Servo s;

int leftPWM = 11;
int leftDir = 12;
int rightPWM = 10;
int rightDir = 9;
int servoPin = 3;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

int dist;
int DistanceCm;


void setup() {
  // put your setup code here, to run once:
  pinMode(leftPWM, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(rightDir, OUTPUT);
  s.attach(servoPin);
  analogWrite(leftPWM, 255);
  analogWrite(rightPWM, 255);
  digitalWrite(leftDir, 0);
  digitalWrite(rightDir, 1);
  Serial.begin(115200);
  
  for(int i = 0; i < COLORSETUP; ++i)
  {
    colorSetupTR += analogRead(topRight);
    colorSetupTL += analogRead(topLeft);
    colorSetupBR += analogRead(bottomRight);
    colorSetupBL += analogRead(bottomLeft);  
  }
  
  homeColorTR = colorSetupTR/COLORSETUP;
  homeColorTL = colorSetupTL/COLORSETUP;
  homeColorBR = colorSetupBR/COLORSETUP;
  homeColorBL = colorSetupBL/COLORSETUP;
}

void loop() {
  // put your main code here, to run repeatedly:
//    s.write(0);
//    delay(20000);
//   s.write(180);
//    delay(20000);
  sensorTR = analogRead(topRight);
  sensorTL = analogRead(topLeft);
  sensorBR = analogRead(bottomRight);
  sensorBL = analogRead(bottomLeft);  
  dist = sonar.ping_in();  
  
  
 

  
  int count =0;
  int thresh = 100;
  /**
  if (abs((sensorTR- homeColorTR)*100/homeColorTR) < thresh)
  {
    count ++;
  }  
    if (abs((sensorTL- homeColorTL)*100/homeColorTL) < thresh)
  {
    count ++;
  }  
    if (abs((sensorBR- homeColorBR)*100/homeColorBR) < thresh)
  {
    count ++;
  }  
    if (abs((sensorBL- homeColorBL)*100/homeColorBL) < thresh)
  {
    count ++;
  }
  **/
  if (sensorTR > thresh)
  {
   count++; 
  }
    if (sensorTL > thresh)
  {
   count++; 
  }
    if (sensorBR > thresh)
  {
   count++; 
  }
    if (sensorBL > thresh)
  {
   count++; 
  }
  
  if (count >=2 )
  {
    s.write(180);
  } 
   else
  { 
    s.write(0);
  } 
   Serial.print("TR: ");
  Serial.print(sensorTR);
  Serial.print("  TL: ");
  Serial.print(sensorTL);
  Serial.print("  BR: ");
  Serial.print(sensorBR);
  Serial.print("  BL: ");
  Serial.print(sensorBL);
  Serial.print(" Dist: ");
  Serial.print(dist);
  Serial.print(" Count: ");
  Serial.println(count);
// delay(100);   
}
