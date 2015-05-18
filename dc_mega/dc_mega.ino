#include <Servo.h>

#include <NewPing.h>


#define COLORSETUP 100


//Define Pinouts
#define PIN_TRIGGER_R  8
#define PIN_ECHO_R     9

#define PIN_TRIGGER_L  10
#define PIN_ECHO_L     11

#define PIN_COLORSENSE1  A2
#define PIN_COLORSENSE2  A3
#define PIN_COLORSENSE3  A0
#define PIN_COLORSENSE4  A1

#define PIN_LEFT_PWM     5
#define PIN_LEFT_DIR     22
#define PIN_RIGHT_PWM    6
#define PIN_LEFT_DIR     23
#define PIN_SERVO        3

#define PIN_BREAKBEAM   A5
#define PIN_FRONTLASER  A4


//Define States
#define BIN_INVERTED 0
#define BIN_DOWN 175

#define MAX_DISTANCE 200 

#define PATH 0
#define CUBE_SEARCH 1
#define CUBE_DUMP 2

#define FORWARD 0
#define LEFT 1
#define RIGHT 2
#define BACK 3
#define STOP 4


//Parameters
#define NUM_AVG 5
#define MAX_BLOCKS 2



int robotState = 0;


//Define Global Vars
int homeColor1 = 0; //Formerly BR
int homeColor2 = 0; //Formerly BL
int homeColor3 = 0; //Formerly TR
int homeColor4 = 0; //Formerly TL

int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;

int colorSetup1 = 0;
int colorSetup2 = 0;
int colorSetup3 = 0;
int colorSetup4 = 0;

int colorArr1[NUM_AVG];
int colorArr2[NUM_AVG];
int colorArr3[NUM_AVG];
int colorArr4[NUM_AVG];


int frontLaserArr[NUM_AVG];
int backBreakBeamArr[NUM_AVG];

int iter = 0;

int val1, val2, val3, val4;

int blockCount = 0;
int prevFrontVal = 0;
int prevBackVal = 0;





Servo s;

NewPing sonarR(TRIGGER_PIN_R, ECHO_PIN_R, MAX_DISTANCE); 
NewPing sonarL(TRIGGER_PIN_L, ECHO_PIN_L, MAX_DISTANCE); 

int distR, distL;
int DistanceCm;


void setup() {
  // put your setup code here, to run once:
  pinMode(leftPWM, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(rightDir, OUTPUT);
  
  s.attach(servoPin);
  s.write(BIN_DOWN);
  Serial.begin(115200);
  
  for(int i = 0; i < COLORSETUP; ++i)
  {
    colorSetup1 += analogRead(colorSense1);
    colorSetup2 += analogRead(colorSense2);
    colorSetup3 += analogRead(colorSense3);
    colorSetup4 += analogRead(colorSense4);
  }
  
  for(int j = 0; j < NUM_AVG; j++)
  {
    //Initialize Moving Average arrays to zero
    colorArr1[j] = 0;
    colorArr2[j] = 0;
    colorArr3[j] = 0;
    colorArr4[j] = 0;
    
    frontLaserArr[j] = 0;
    backBreakBeamArr[j] = 0;
  }
  
 
  homeColor1 = colorSetup1/COLORSETUP;
  homeColor2 = colorSetup2/COLORSETUP;
  homeColor3 = colorSetup3/COLORSETUP;
  homeColor4 = colorSetup4/COLORSETUP;
  
  plannedPath();
}

void loop() {
  querySensors();
  //delay(100);
  /*
  distR = sonarR.ping_in(); 
  distL = sonarL.ping_in();
  
  if(distR == 0)
  {
    distR = 50;
  }
  if(distL == 0)
  {
    distL = 50;
  }
  
  Serial.print(distR);
  Serial.print("\t");
  Serial.println(distL);
  
  if(distR < 8 && distL < 8)
  {
    driveBack(100);
  }
  else if(distR < 8)
  {
    driveLeft(100);
  }
  else if(distL < 8)
  {
    driveRight(100);
  }
  else
  {
    driveForward(100);
  }
  */
}
  
  
  
  
  
  
  
  
  
  /*
  Serial.print("\n");
  int frontLaserThresh = 500;
  int backBreakBeamThresh = 500;
  
  
  //frontLaserArr[iter%NUM_AVG] = analogRead(frontLaser);
  int frontLaserVal = analogRead(frontLaser);//getValue(frontLaserArr);
  int backBreakBeamVal = analogRead(backBreakBeam);
  
  
  Serial.print("LaserVal:\t");
  Serial.print(frontLaserVal);
  if(frontLaserVal + frontLaserThresh < prevFrontVal)
  {
    blockCount++;
    Serial.print(" NumBlocks:\t");
    Serial.print(blockCount);
  }
  
  
  
  if(blockCount >=MAX_BLOCKS)
  {
    delay(1000);
    s.write(BIN_INVERTED);
    blockCount = 0;
  }
  
  
  if(prevBackVal < (backBreakBeamVal - backBreakBeamThresh))
  {
    s.write(BIN_DOWN);
  }
  
  
  
  prevBackVal = backBreakBeamVal;
  prevFrontVal = frontLaserVal;
  
  iter++;
}

*/







  // put your main code here, to run repeatedly:
  //s.write(0);
  //delay(1000);
  //s.write(180);
  //delay(1000);
  //distR = sonarR.ping_in(); 
  //distL = sonarL.ping_in();   
  //Serial.print(distR);
  //Serial.print("\t");
  //Serial.println(distL);
  //driveForward(50);
  //driveLeft(100);
  //driveRight(100);
  //driveBack(100);
  /*
  int sum1 = 0;
  int sum2 = 0;
  int sum3 = 0;
  int sum4 = 0;
   
  sensor1 = analogRead(colorSense1);
  sensor2 = analogRead(colorSense2);
  sensor3 = analogRead(colorSense3);
  sensor4 = analogRead(colorSense4); 
  
  Serial.print(analogRead(frontLaser));
  Serial.print("\t");
  Serial.println(analogRead(backBreakBeam));

*/
/*
  colorArr1[iter%NUM_AVG] = sensor1;
  colorArr2[iter%NUM_AVG] = sensor2;
  colorArr3[iter%NUM_AVG] = sensor3;
  colorArr4[iter%NUM_AVG] = sensor4;
  iter++;
  
  
  for(int i = 0; i < NUM_AVG; i++)
  {
    sum1 = sum1 + colorArr1[i];
    sum2 = sum2 + colorArr2[i];
    sum3 = sum3 + colorArr3[i];
    sum4 = sum4 + colorArr4[i];
  }
  
  val1 = sum1 / min(iter, NUM_AVG);
  val2 = sum2 / min(iter, NUM_AVG);
  val3 = sum3 / min(iter, NUM_AVG);
  val4 = sum4 / min(iter, NUM_AVG);
  
  //printColors();
  
  int count = 0;
  int thresh = 75;
  /*
  if (abs((sensor1 - homeColor1)*100/homeColor1) < thresh)
  {
    count ++;
  }  
  if (abs((sensor2 - homeColor2)*100/homeColor2) < thresh)
  {
    count ++;
  }  
  if (abs((sensor3 - homeColor3)*100/homeColor3) < thresh)
  {
    count ++;
  }  
  if (abs((sensor4 - homeColor4)*100/homeColor4) < thresh)
  {
    count ++;
  }
  */
   /* 
  if (sensor1 > thresh)
  {
   count++; 
  }
  if (sensor2 > thresh)
  {
   count++; 
  }
  if (sensor3 > thresh)
  {
   count++; 
  }
  if (sensor4 > thresh)
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
  */

void plannedPath()
{
  int actions[10];
  int times[10];
  int actionCounter = 0;
  
  bool runPlannedPath = true;
  
  while(runPlannedPath)
  {
   
    readSensors();
    
    updateTimer();
    
    doAction(actions[actionCounter]);
    times[actionCounter]--;
    if(times[actionCounter] >= 0)
    {
      actionCounter++;
    }
  }
}

void readSensors()
{
  
}
  
  
void doAction(int action)
{
  switch(action)
  {
    case FORWARD:
      driveForward(100);
      break;
    case LEFT:
      driveLeft(100);
      break;
    case RIGHT:
          driveRight(100);
      break;
    case BACK:
          driveBack(100);
      break;
    case STOP:
          driveStop(100);
      break;
  }
  
  case action


}
  /*
  driveForward(100);
  delay(1500);
  delay(1500);
  driveStop();
  driveLeft(100);
  delay(800);
  driveStop();
  driveForward(100);
  delay(1500);
  delay(1500);
  delay(1500);
  delay(1500);
  driveStop();
  driveRight(100);
  delay(800);
  driveStop();
  driveForward(100);
  delay(1500);
  driveStop();
  driveRight(100);
  delay(800);
  driveStop(); 
  driveForward(100);
  delay(1500);
  delay(1500);
  delay(1500);
  delay(1500);
  driveStop();
  */


int getValue(int* valueArray)
{
  int sum = 0;
  for(int i = 0; i < NUM_AVG; i++)
  {
    sum = sum + valueArray[i];
  }
  
  return sum/min(iter, NUM_AVG);
}

void driveForward(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);
  analogWrite(rightPWM, vel);
  
  digitalWrite(leftDir, 1);
  digitalWrite(rightDir, 0);
}


void driveLeft(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);
  analogWrite(rightPWM, vel);
  
  digitalWrite(leftDir, 0);
  digitalWrite(rightDir, 0);
}

void driveRight(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);
  analogWrite(rightPWM, vel);
  
  digitalWrite(leftDir, 1);
  digitalWrite(rightDir, 1);
}

void driveBack(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);
  analogWrite(rightPWM, vel);
  
  digitalWrite(leftDir, 0);
  digitalWrite(rightDir, 1);
}

void driveStop()
{
  analogWrite(leftPWM, 0);
  analogWrite(rightPWM, 0);
  
  digitalWrite(leftDir, 0);
  digitalWrite(rightDir, 1);
}

void printColors()
{
  Serial.print("Color1: ");
  Serial.print(val1);
  Serial.print("\t");
  Serial.print("Color2: ");
  Serial.print(val2);
  Serial.print("\t");
  Serial.print("Color3: ");
  Serial.print(val3);
  Serial.print("\t");
  Serial.print("Color4: ");
  Serial.println(val4);
}

void querySensors()
{

}

