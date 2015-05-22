#include <Servo.h>

#include <NewPing.h>


#define COLORSETUP 100


//Define Pinouts
#define PIN_TRIGGER_R  8
#define PIN_ECHO_R     9

#define PIN_TRIGGER_L  10
#define PIN_ECHO_L     11

#define PIN_TRIGGER_SL 12
#define PIN_ECHO_SL    13

#define PIN_TRIGGER_SR 6
#define PIN_ECHO_SR    7

#define PIN_COLORSENSE1  A2
#define PIN_COLORSENSE2  A3
#define PIN_COLORSENSE3  A0
#define PIN_COLORSENSE4  A1

#define PIN_LEFT_PWM     4


#define PIN_LEFT_DIR1     22
#define PIN_LEFT_DIR2     24
#define PIN_RIGHT_PWM    5
#define PIN_RIGHT_DIR1     23
#define PIN_RIGHT_DIR2     25
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
#define FORWARD_WALL 6
#define LEFT 1
#define RIGHT 2
#define BACK 3
#define STOP 4
#define DUMP 5
#define DUMP_UP 7

//Parameters
#define NUM_AVG 7
#define MAX_BLOCKS 2

#define NUM_ACTIONS 44

#define PURPLE 1
#define WHITE 0

#define SONAR_NUM 4
#define SONAR_R 0
#define SONAR_L 1
#define SONAR_SR 2
#define SONAR_SL 3

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

int turnTime = 0;
int thresh = 50;

Servo s;

NewPing sonar[SONAR_NUM] = {
  NewPing(PIN_TRIGGER_R, PIN_ECHO_R, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_L, PIN_ECHO_L, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_SR, PIN_ECHO_SR, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_SL, PIN_ECHO_SL, MAX_DISTANCE)
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

bool different_color(int a, int b)
{
  return (abs((a-b)) > thresh);
}


void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LEFT_PWM, OUTPUT);
  pinMode(PIN_RIGHT_PWM, OUTPUT);
  pinMode(PIN_LEFT_DIR1, OUTPUT);
  pinMode(PIN_RIGHT_DIR1, OUTPUT);
    pinMode(PIN_LEFT_DIR2, OUTPUT);
  pinMode(PIN_RIGHT_DIR2, OUTPUT);

  s.attach(PIN_SERVO);
  s.write(BIN_DOWN);
  Serial.begin(9600);
  Serial.print("begin");
  
  delay(2000);


  for (int i = 0; i < COLORSETUP; ++i)
  {
    sensor1 += analogRead(PIN_COLORSENSE1);
    sensor2 += analogRead(PIN_COLORSENSE2);
    sensor3 += analogRead(PIN_COLORSENSE3);
    sensor4 += analogRead(PIN_COLORSENSE4);
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


  homeColor1 = sensor1 / COLORSETUP;
  homeColor2 = sensor2 / COLORSETUP;
  homeColor3 = sensor3 / COLORSETUP;
  homeColor4 = sensor4 / COLORSETUP;
  /**
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
  
    
**/

  Serial.print("Sensor 1: ");
  Serial.print(homeColor1);
  Serial.print(" Sensor 2: ");
  Serial.print(homeColor2);
  Serial.print(" Sensor 3: ");
  Serial.print(homeColor3);
  Serial.print(" Sensor 4: ");
  Serial.println(homeColor4);
// driveForward(100);

  
  turnTime = calibrateTurn();
  delay(100);

  driveLeft(75);

  delay(turnTime);
    driveStop();
    delay(100);
  driveLeft(75);
  delay(turnTime);
  driveStop();
  delay(100);

   plannedPath();
  //Serial.print("Driving");
  //s.write(BIN_INVERTED);
}


int calibrateTurn()
{
   int time = millis();
   driveRight(75);
   int count =0;
   while(count < 6)
   {
     delay(1);
    int leftDist = sonar[SONAR_L].ping_cm(); 
        int rightDist = sonar[SONAR_R].ping_cm(); 
        Serial.print("Left\t");
        Serial.print(leftDist);
                Serial.print("\tRight\t");
        Serial.println(rightDist);
    if(leftDist > 0 && leftDist< 30 && rightDist < 30 && rightDist > 0)
    {
      count++;
    }
    else
    {
     count =0; 
    }

   }
   time = millis() - time;
   Serial.println(time);
   driveStop();
   return time;
   
}
void loop() {
  //delay(PING_INTERVAL);
  //echoCheck(SONAR_L);
  //echoCheck(SONAR_R);
  ////oneSensorCycle();
  //Serial.println("escaped");
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
/*
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
      */
}


int duration(int action)
{
    switch (action)
  {
    case FORWARD:
        return 1400;
      break;
    case FORWARD_WALL:
        return 3500;
    break;
    case LEFT:
      return turnTime*.92;
      break;
    case RIGHT:
      return turnTime*.92;
      break;
    case BACK:
      return 1400;
      break;
    case STOP:
      return 100;
      break;
    case DUMP:
    return 1;
  }
}

void plannedPath()
{
  Serial.println("Plan path");
  int actionCounter = 0;

  int forwardTime = 1400;
  int startTime;

  //turnTime *=.95;
  //int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, LEFT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP, FORWARD, STOP, RIGHT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP};
  int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, RIGHT, 
                                FORWARD, FORWARD, FORWARD, LEFT, 
                                FORWARD_WALL, STOP, LEFT, STOP, 
                                FORWARD, FORWARD, FORWARD, STOP, 
                                LEFT, STOP, DUMP, STOP, FORWARD, STOP, LEFT, FORWARD,
                                RIGHT, FORWARD, DUMP_UP, FORWARD,
                                //FORWARD, RIGHT, FORWARD, FORWARD, 
                                STOP, LEFT, FORWARD, FORWARD, FORWARD, 
                                RIGHT, FORWARD, STOP, RIGHT, STOP, 
                                FORWARD, FORWARD, STOP, DUMP};

  //int durations [NUM_ACTIONS] = {1300, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100};

//  int durations [NUM_ACTIONS] = {forwardTime, forwardTime, 100, turnTime, forwardTime, forwardTime, forwardTime, turnTime, forwardTime, 100, turnTime, 100, forwardTime, forwardTime, forwardTime, forwardTime, forwardTime, forwardTime, 100};

 
    for (int i =0; i< NUM_ACTIONS; i++)
    {
      bool change_once = false;
      startTime = millis();
      if(actions[i] != (FORWARD || FORWARD_WALL)){
        while(millis() - startTime < duration(actions[i])){
          doAction(actions[i]);
          delay(5);
        }
      } /*else if (actions[i] == FORWARD_WALL)
      {
        while(millis() - startTime < duration(actions[i])){
          doAction(actions[i]);
          
        }
      }*/
      
      else {
        
      Serial.print("Action: ");
      Serial.println(i);
       int startVals[4]= {0,0,0,0}; 
       int startColors[4]= {0,0,0,0}; 
       for (int j = 0; j < NUM_AVG; j++)
       {
        startVals[0] += analogRead(PIN_COLORSENSE1);
        startVals[1] += analogRead(PIN_COLORSENSE2);
        startVals[2] += analogRead(PIN_COLORSENSE3);
        startVals[3] += analogRead(PIN_COLORSENSE4);
       }
       for(int j=0 ; j<4; j++)
       {
         //startColors[j] = color(startVals[j]/NUM_AVG); 
         startColors[j] = startVals[j]/NUM_AVG;
       }
       int fix = 0;
       int modifier = 0;
       int dur =  (duration(actions[i]) + modifier);
       startTime = millis();
      while(millis() - startTime < dur)
        {
          Serial.print("Action: ");
          Serial.println(i);
          Serial.println(millis());
          Serial.println(startTime);
          doAction(actions[i]);
          sensor1 = analogRead(PIN_COLORSENSE1);
          //colors[0] = color(sensor1);
          sensor2 = analogRead(PIN_COLORSENSE2);
          //colors[1]  = color(sensor2);
          sensor3 = analogRead(PIN_COLORSENSE3);
          //colors[2]  = color(sensor3);
          sensor4 = analogRead(PIN_COLORSENSE4);
          //colors[3]  = color(sensor4);
          int count = 0;
          Serial.print("Start 1: ");
          Serial.print(startColors[0]);      
                Serial.print("\tStart 2: ");
          Serial.print(startColors[1]);
                Serial.print("\tStart 3: ");
          Serial.print(startColors[2]);
                Serial.print("\tStart 4: ");
          Serial.println(startColors[3]);
          Serial.print("Sensor 1: ");
        Serial.print(sensor1);      
              Serial.print("\tSensor 2: ");
        Serial.print(sensor2);
              Serial.print("\tSensor 3: ");
        Serial.print(sensor3);
              Serial.print("\tSensor 4: ");
        Serial.println(sensor4);
          Serial.println(different_color(startColors[1], sensor2));
          Serial.println(different_color(startColors[3], sensor4));
          if (different_color(startColors[0], sensor1))
          {
           count ++; 
          }
          
           //if ( (startColors[3] >= sensor4 && (100*(startColors[3]-sensor4))/startColors[3] < thresh)  || (startColors[3] < sensor4 && (100*(sensor4 - startColors[3]))/sensor4 < thresh))
          if(different_color(startColors[2], sensor3))
          {
           count ++; 
          }
          
          if(count == 2 && !change_once && actions[i] == FORWARD){
              change_once = true;
//            startTime = millis() - duration(actions[i]) + 50;
              dur = millis()-startTime+50;
              startTime = millis();

            //startTime -= 500;
            //modifier += 500;
          }
          
          delay(100);
        }
       driveStop();
      delay(1000);
      }
  }
  driveStop();
  s.write(BIN_INVERTED);     

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
    case FORWARD_WALL:
      driveForwardWall(100);
      break;
    case LEFT:
      driveLeft(75);
      break;
    case RIGHT:
      driveRight(75);
      break;
    case BACK:
      driveBack(100);
      break;
    case STOP:
      driveStop();
      break;
    case DUMP:
      s.write(BIN_INVERTED);
      delay(1500);
      break;
    case DUMP_UP:
      s.write(BIN_DOWN);
      delay(1000);
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

  digitalWrite(PIN_LEFT_DIR1, 1);
  digitalWrite(PIN_RIGHT_DIR1, 0);
   digitalWrite(PIN_LEFT_DIR2, 0);
  digitalWrite(PIN_RIGHT_DIR2, 1);
}

void driveForwardWall(int speed)
{
  int vel = speed * 2.55;

  delay(1);
  int leftDist = sonar[SONAR_L].ping_cm(); 
  delay(1);
  int rightDist = sonar[SONAR_R].ping_cm();
  
  Serial.print("\nSensor 1 l: \t");
  Serial.println(leftDist);      
  Serial.print("\nSensor 2 r: \t");
  Serial.println(rightDist);
  
  if (leftDist > 30)
  {
    analogWrite(PIN_LEFT_PWM, vel);
  }
  else
  {
    analogWrite(PIN_LEFT_PWM, 0);
  }
  
  if (rightDist > 30)
  {
    analogWrite(PIN_RIGHT_PWM, vel);
  }
  else
  {
    analogWrite(PIN_RIGHT_PWM, 0);
  }
  

  digitalWrite(PIN_LEFT_DIR1, 1);
  digitalWrite(PIN_RIGHT_DIR1, 0);
   digitalWrite(PIN_LEFT_DIR2, 0);
  digitalWrite(PIN_RIGHT_DIR2, 1);
}

void driveLeft(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

    digitalWrite(PIN_LEFT_DIR1, 1);
  digitalWrite(PIN_RIGHT_DIR1, 1);
   digitalWrite(PIN_LEFT_DIR2, 0);
  digitalWrite(PIN_RIGHT_DIR2, 0);
}

void driveRight(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

    digitalWrite(PIN_LEFT_DIR1, 0);
  digitalWrite(PIN_RIGHT_DIR1, 0);
   digitalWrite(PIN_LEFT_DIR2, 1);
  digitalWrite(PIN_RIGHT_DIR2, 1);
}

void driveBack(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel);

    digitalWrite(PIN_LEFT_DIR1, 0);
  digitalWrite(PIN_RIGHT_DIR1, 1);
   digitalWrite(PIN_LEFT_DIR2, 1);
  digitalWrite(PIN_RIGHT_DIR2, 0);
}

void driveStop()
{
  analogWrite(PIN_LEFT_PWM, 0);
  analogWrite(PIN_RIGHT_PWM, 0);

    digitalWrite(PIN_LEFT_DIR1, 0);
  digitalWrite(PIN_RIGHT_DIR1, 1);
   digitalWrite(PIN_LEFT_DIR2, 1);
  digitalWrite(PIN_RIGHT_DIR2, 0);
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

