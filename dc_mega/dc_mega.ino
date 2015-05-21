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
#define BIN_INVERTED 1
#define BIN_DOWN 175

#define MAX_DISTANCE 200
#define PING_INTERVAL 33

#define PATH 0
#define CUBE_SEARCH 1
#define CUBE_DUMP 2

#define FORWARD 0
#define LEFT 1
#define RIGHT 2
#define BACK 3
#define STOP 4


//Parameters
#define NUM_AVG 7
#define MAX_BLOCKS 2

#define NUM_ACTIONS 19

#define PURPLE 1
#define WHITE 0

#define SONAR_NUM 2
#define SONAR_R 0
#define SONAR_L 1

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

unsigned long pingTimer[SONAR_NUM];
int cm[SONAR_NUM][NUM_AVG];
uint8_t currentSensor = 0;

Servo s;

NewPing sonar[SONAR_NUM] = {
  NewPing(PIN_TRIGGER_R, PIN_ECHO_R, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_L, PIN_ECHO_L, MAX_DISTANCE)
};

int initialDistL;
int initialDistR;

int color(int val)
{
   if(val>100)
   {
     return 1;
   }
   else
   {
     return 0;
   }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LEFT_PWM, OUTPUT);
  pinMode(PIN_RIGHT_PWM, OUTPUT);
  pinMode(PIN_LEFT_DIR, OUTPUT);
  pinMode(PIN_RIGHT_DIR, OUTPUT);

  s.attach(PIN_SERVO);
  s.write(BIN_DOWN);
  Serial.begin(9600);
  Serial.print("begin");
  
  delay(2000);


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
  
  pingTimer[0] = millis() + 75;
  pingTimer[1] = pingTimer[0] + PING_INTERVAL;
  
  Serial.println("Starting sonar initialization");
  
  for(uint8_t i = 0; i < NUM_AVG; i++){
    cm[SONAR_L][i] = 0;
    cm[SONAR_R][i] = 0;
  }
  
  for(uint8_t i = 0; i < NUM_AVG; i++){
  
    delay(PING_INTERVAL);
    unsigned int uS = sonar[SONAR_L].ping();
    cm[SONAR_L][i] = uS / US_ROUNDTRIP_CM;
    uS = sonar[SONAR_R].ping();
    cm[SONAR_R][i] = uS / US_ROUNDTRIP_CM;
  }
  
  initialDistL = find_median(cm[SONAR_L]);
  initialDistR = find_median(cm[SONAR_R]);
  
  Serial.print("Left initial dist: ");
  Serial.print(initialDistL);
  Serial.print("Right initial dist: ");
  Serial.println(initialDistR);
    
  //plannedPath();
  driveForward(100);
  //Serial.print("Driving");
  //s.write(BIN_INVERTED);
}



void loop() {
  delay(PING_INTERVAL);
  echoCheck(SONAR_L);
  echoCheck(SONAR_R);
  oneSensorCycle();
  /*
  for(uint8_t i = 0; i < SONAR_NUM; i++){
    if(millis() >= pingTimer[i]){
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      if(i == 0 && currentSensor == SONAR_NUM - 1){
        oneSensorCycle();
      }
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor][NUM_AVG-1] = MAX_DISTANCE;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }
*/
//  driveForward(100);
/**
    int sensor1 = analogRead(PIN_COLORSENSE1);
      int sensor2 = analogRead(PIN_COLORSENSE2);
      int sensor3 = analogRead(PIN_COLORSENSE3);
      int sensor4 = analogRead(PIN_COLORSENSE4);
      Serial.print("Sensor 1: \t");
      Serial.print(sensor1);      
            Serial.print("\tSensor 2: \t");
      Serial.print(sensor2);
            Serial.print("\tSensor 3: \t");
      Serial.print(sensor3);
            Serial.print("\tSensor 4: \t");
      Serial.println(sensor4);
      **/
}


void plannedPath()
{
  int actionCounter = 0;

//fflfffrfrfff
  //int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, LEFT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP, FORWARD, STOP, RIGHT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP};
  int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, RIGHT, FORWARD, FORWARD, FORWARD, LEFT, FORWARD, STOP, LEFT, STOP, FORWARD, FORWARD, FORWARD, STOP, STOP, STOP, STOP};

  //int durations [NUM_ACTIONS] = {1300, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100};

  int durations [NUM_ACTIONS] = {1400, 1400, 100, 700, 1400, 1400, 1400, 700, 1400, 100, 700, 100, 1200, 1200, 1200, 1200, 1200, 1200, 100};

 
    for (int i =0; i< NUM_ACTIONS; i++)
    {
       int startColors[4]= {0,0,0,0}; 
       for (int j = 0; j < COLORSETUP; j++)
       {
        startColors[0] += analogRead(PIN_COLORSENSE1);
        startColors[1] += analogRead(PIN_COLORSENSE2);
        startColors[2] += analogRead(PIN_COLORSENSE3);
        startColors[3] += analogRead(PIN_COLORSENSE4);
       }
       int change_once = 0;
       for(int j=0 ; j<4; j++)
       {
         startColors[j] = color(startColors[j]/COLORSETUP); 
       }
      for (int j = durations[i]; j > 0; j--)
      {
        doAction(actions[i]);
        delay(1);
        int colors[4];
        int sensor1 = analogRead(PIN_COLORSENSE1);
        colors[0] = color(sensor1);
        int sensor2 = analogRead(PIN_COLORSENSE2);
        colors[1]  = color(sensor2);
        int sensor3 = analogRead(PIN_COLORSENSE3);
        colors[2]  = color(sensor3);
        int sensor4 = analogRead(PIN_COLORSENSE4);
        colors[3]  = color(sensor4);
        int thresh = 50;
        int count = 0;
        /**
        Serial.print("Color 1: \t");
        Serial.print(colors[0]);      
        Serial.print("\tColor 2: \t");
        Serial.print(colors[1]);
        Serial.print("\tColor 3: \t");
        Serial.print(colors[2]);
        Serial.print("\tColor 4: \t");
        Serial.print(colors[3]);
        Serial.print("SColor 1: \t");
        Serial.print(startColors[0]);      
        Serial.print("\tSColor 2: \t");
        Serial.print(startColors[1]);
        Serial.print("\tSColor 3: \t");
        Serial.print(startColors[2]);
        Serial.print("\tSColor 4: \t");
        Serial.println(startColors[3]);
        **/
        for(int k =0; k<4; k++)
        {
          /**
          if(startColors[i]!=colors[i])
          {
             count++; 
          }
          **/

        }
                  if(startColors[1]!=colors[1])
          {
             count++; 
          }
                    if(startColors[3]!=colors[3])
          {
             count++; 
          }/*
          if(startColors[1]==colors[1]){
            count--;
          }
          if(startColors[3] == colors[3]){
            count--;
          }*/
        if (count == 2 && change_once == 0 && actions[i]==FORWARD)
        {
          change_once = 2;
          Serial.write("Wop");
          j = (durations[i]-j);
        }
        else if (count > 2 && change_once == 0 && actions[i] == FORWARD) 
        {
          change_once = 1;
          j = j/3;
        }
        //else if(change_once == 1 && count 
        else if (count < 1 && change_once == 2 && actions[i]==FORWARD)
        {
          change_once = 0;
          Serial.write("Woop");
          j = (durations[i])/2;
          //j = 0;
      }
    }
    driveStop();
    delay(1000);
  }
  driveStop();
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

void echoCheck(int currentSensor)
{
  
  for(uint8_t i = 1; i < NUM_AVG; i++){
    cm[currentSensor][i-1] = cm[currentSensor][i];
  }
  //if(sonar[currentSensor].check_timer()){
    cm[currentSensor][NUM_AVG-1] = sonar[currentSensor].ping() / US_ROUNDTRIP_CM;
  //}
}

void oneSensorCycle()
{
  
  int leftMedian = find_median(cm[SONAR_L]);
  int rightMedian = find_median(cm[SONAR_R]);
  
  Serial.print("Left: ");
  Serial.print(leftMedian);
  Serial.print("   Right: ");
  Serial.println(rightMedian);
  
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

int find_median(int array[]){
  int copy[NUM_AVG];
  for(uint8_t i = 0; i < NUM_AVG; i++){
    copy[i] = array[i];
  }
  insertion_sort(copy, NUM_AVG);
  return copy[NUM_AVG/2];
}

