#include <Servo.h>

#include <NewPing.h>


#define COLORSETUP 100

#define TRIGGER_PIN_R  8
#define ECHO_PIN_R     9

#define TRIGGER_PIN_L  10
#define ECHO_PIN_L     11


#define MAX_DISTANCE 200 
#define NUM_AVG 5

//Define Pinouts
int colorSense1 = A2;
int colorSense2 = A3;
int colorSense3 = A0;
int colorSense4 = A1;

int leftPWM = 5;
int leftDir = 22;
int rightPWM = 6;
int rightDir = 23;
int servoPin = 3;


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
int iter = 0;

int val1, val2, val3, val4;

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
    colorArr1[j] = 0;
    colorArr2[j] = 0;
    colorArr3[j] = 0;
    colorArr4[j] = 0;
  }
  
  homeColor1 = colorSetup1/COLORSETUP;
  homeColor2 = colorSetup2/COLORSETUP;
  homeColor3 = colorSetup3/COLORSETUP;
  homeColor4 = colorSetup4/COLORSETUP;
  
  Serial.println(homeColor1);
  Serial.println(homeColor2);
  Serial.println(homeColor3);
  Serial.println(homeColor4);
}

void loop() {
  // put your main code here, to run repeatedly:
  //s.write(0);
  //delay(1000);
  //s.write(180);
  //delay(1000);
  distR = sonarR.ping_in(); 
  distL = sonarL.ping_in();   
  Serial.print(distR);
  Serial.print("\t");
  Serial.println(distL);
  //driveForward(50);
  //driveLeft(100);
  //driveRight(100);
  //driveBack(100);
  int sum1 = 0;
  int sum2 = 0;
  int sum3 = 0;
  int sum4 = 0;
   
  sensor1 = analogRead(colorSense1);
  sensor2 = analogRead(colorSense2);
  sensor3 = analogRead(colorSense3);
  sensor4 = analogRead(colorSense4); 

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
  
}


  /*

  /**

  

  

  
  printColors();

  //Serial.print(" Dist: \t");
  //Serial.print(dist);
  //Serial.print(" Count: ");
  //Serial.println(count);
// delay(100);  
*/



void driveForward(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);//255);
  analogWrite(rightPWM, vel);//255);
  
  digitalWrite(leftDir, 1);
  digitalWrite(rightDir, 0);
}


void driveLeft(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);//255);
  analogWrite(rightPWM, vel);//255);
  
  digitalWrite(leftDir, 0);
  digitalWrite(rightDir, 0);
}

void driveRight(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);//255);
  analogWrite(rightPWM, vel);//255);
  
  digitalWrite(leftDir, 1);
  digitalWrite(rightDir, 1);
}

void driveBack(int speed)
{
  int vel = speed * 2.55;
  
  analogWrite(leftPWM, vel);//255);
  analogWrite(rightPWM, vel);//255);
  
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
