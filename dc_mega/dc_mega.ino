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
#define PIN_RIGHT_DIR     23
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

#define NUM_ACTIONS 19



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

NewPing sonarR(PIN_TRIGGER_R, PIN_ECHO_R, MAX_DISTANCE);
NewPing sonarL(PIN_TRIGGER_L, PIN_ECHO_L, MAX_DISTANCE);

int distR, distL;
int DistanceCm;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LEFT_PWM, OUTPUT);
  pinMode(PIN_RIGHT_PWM, OUTPUT);
  pinMode(PIN_LEFT_DIR, OUTPUT);
  pinMode(PIN_RIGHT_DIR, OUTPUT);

  s.attach(PIN_SERVO);
  s.write(BIN_DOWN);
  Serial.begin(115200);

  for (int i = 0; i < COLORSETUP; ++i)
  {
    colorSetup1 += analogRead(PIN_COLORSENSE1);
    colorSetup2 += analogRead(PIN_COLORSENSE2);
    colorSetup3 += analogRead(PIN_COLORSENSE3);
    colorSetup4 += analogRead(PIN_COLORSENSE4);
  }

  for (int j = 0; j < NUM_AVG; j++)
  {
    //Initialize Moving Average arrays to zero
    colorArr1[j] = 0;
    colorArr2[j] = 0;
    colorArr3[j] = 0;
    colorArr4[j] = 0;

    frontLaserArr[j] = 0;
    backBreakBeamArr[j] = 0;
  }


  homeColor1 = colorSetup1 / COLORSETUP;
  homeColor2 = colorSetup2 / COLORSETUP;
  homeColor3 = colorSetup3 / COLORSETUP;
  homeColor4 = colorSetup4 / COLORSETUP;

  plannedPath();
}

void loop() {

  Serial.write("hi");
  //driveForward(255);

}


void plannedPath()
{
  int actionCounter = 0;


  int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, LEFT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP, FORWARD, STOP, RIGHT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP};

  int durations [NUM_ACTIONS] = {1500, 1500, 100, 400, 100, 1500, 1500, 1500, 1500, 100, 1500, 100, 800, 100, 1500, 1500, 1500, 1500, 100};

  int startSensor1 = analogRead(PIN_COLORSENSE1);
  int startSensor2 = analogRead(PIN_COLORSENSE2);
  int startSensor3 = analogRead(PIN_COLORSENSE3);
  int startSensor4 = analogRead(PIN_COLORSENSE4);

  for (int i = 0; i < 1; i++)
  {

    for (int j = durations[i]; j > 0; j--)
    {
      doAction(actions[i]);
      delay(1);
      int sensor1 = analogRead(PIN_COLORSENSE1);
      int sensor2 = analogRead(PIN_COLORSENSE2);
      int sensor3 = analogRead(PIN_COLORSENSE3);
      int sensor4 = analogRead(PIN_COLORSENSE4);
      int thresh = 75;
      int count = 0;
      if (abs((sensor1 - homeColor1) * 100 / homeColor1) < thresh)
      {
        count ++;
      }
      if (abs((sensor2 - homeColor2) * 100 / homeColor2) < thresh)
      {
        count ++;
      }
      if (abs((sensor3 - homeColor3) * 100 / homeColor3) < thresh)
      {
        count ++;
      }
      if (abs((sensor4 - homeColor4) * 100 / homeColor4) < thresh)
      {
        count ++;
      }
      if (count > 2 && j > durations[i] / 2)
      {
        j = durations[i] / 2;
      }
    }
  }
}

void readSensors()
{

}


void doAction(int action)
{

  switch (action)
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
      driveStop();
      break;
  }


}

int getValue(int* valueArray)
{
  int sum = 0;
  for (int i = 0; i < NUM_AVG; i++)
  {
    sum = sum + valueArray[i];
  }

  return sum / min(iter, NUM_AVG);
}

void driveForward(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

  digitalWrite(PIN_LEFT_DIR, 1);
  digitalWrite(PIN_RIGHT_DIR, 0);
}


void driveLeft(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

  digitalWrite(PIN_LEFT_DIR, 0);
  digitalWrite(PIN_RIGHT_DIR, 0);
}

void driveRight(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

  digitalWrite(PIN_LEFT_DIR, 1);
  digitalWrite(PIN_RIGHT_DIR, 1);
}

void driveBack(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

  digitalWrite(PIN_LEFT_DIR, 0);
  digitalWrite(PIN_RIGHT_DIR, 1);
}

void driveStop()
{
  analogWrite(PIN_LEFT_PWM, 0);
  analogWrite(PIN_RIGHT_PWM, 0);

  digitalWrite(PIN_LEFT_DIR, 0);
  digitalWrite(PIN_RIGHT_DIR, 1);
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

void swap(int a, int b, int array[])
{
  int temp = array[b];
  array[b] = array[a];
  array[a] = temp;
}

void insertion_sort(int array[], int n)
{
  for(int i = 1; i < n; i++){
    int curr = array[i];
    int j = i;
    while(j > 0 && array[j-1] > curr){
      array[j] = array[j-1];
      j--;
    }
    array[j] = curr;
  }
}

