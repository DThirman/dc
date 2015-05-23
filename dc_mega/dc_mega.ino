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

#define PIN_RIGHT_PWM     4


#define PIN_LEFT_DIR1     22
#define PIN_LEFT_DIR2     24
#define PIN_LEFT_PWM    5
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
#define FORWARD_SLIGHT 11
#define LEFT 1
#define RIGHT 2
#define BACK 3
#define STOP 4
#define DUMP 5
#define DUMP_UP 7
#define CALIBRATE_OPP_COLOR 8
#define OLEFT 9
#define ORIGHT 10
#define DEDREKON 12

//Parameters
#define NUM_AVG 7
#define MAX_BLOCKS 2

#define NUM_ACTIONS 80

#define PURPLE 0
#define WHITE 1

#define SONAR_NUM 4
#define SONAR_R 0
#define SONAR_L 1
#define SONAR_SR 2
#define SONAR_SL 3

#define PURPLESTART 0//the purple corner (white start zone)
#define WHITESTART 1 //the white corner (purple start zone) 

int robotState = 0;


//Define Global Vars
int homeColor[4] = {0,0,0,0};

int oppositeColor[4] = {0,0,0,0};

int colorThresh[4] = {0,0,0,0};

int startCorner;

int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;


int frontLaserArr[NUM_AVG];
int backBreakBeamArr[NUM_AVG];

int iter = 0;

int val1, val2, val3, val4;

int blockCount = 0;
int prevFrontVal = 0;
int prevBackVal = 0;

int turnTime = 0;
int thresh = 50;

Servo s;

NewPing sonar[SONAR_NUM] = {
  NewPing(PIN_TRIGGER_R, PIN_ECHO_R, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_L, PIN_ECHO_L, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_SR, PIN_ECHO_SR, MAX_DISTANCE),
  NewPing(PIN_TRIGGER_SL, PIN_ECHO_SL, MAX_DISTANCE)
};

int cm[SONAR_NUM][NUM_AVG];

bool leftStopped = false;
bool rightStopped = false;
int stopDist = 0;

bool oppositeCalibrated = false;

int color(int val, int sensor)
{
  /*
   if(val>100)
   {
     return 1;
   }
   else
   {
     return 0;
   }
   */
   if(same_color(val, homeColor[sensor], sensor)){
     return startCorner;
   } else if(same_color(val, oppositeColor[sensor], sensor)){
     if(startCorner == PURPLESTART){
       return WHITESTART;
     } else {
       return PURPLESTART;
     }
   } else if(startCorner == WHITESTART && val > homeColor[sensor]){
       return WHITESTART;
   } else if(startCorner == PURPLESTART && val > oppositeColor[sensor]){
     return WHITESTART;
   } else {
     return -1;
   }
   
   
}

bool different_color(int a, int b)
{
  return (abs((a-b)) > thresh);
}

bool same_color(int a, int b, int sensor)
{
  return (abs((a-b)) < colorThresh[sensor]);
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

    frontLaserArr[j] = 0;
    backBreakBeamArr[j] = 0;
  }

  
  homeColor[0] = sensor1 / COLORSETUP;
  homeColor[1] = sensor2 / COLORSETUP;
  homeColor[2] = sensor3 / COLORSETUP;
  homeColor[3] = sensor4 / COLORSETUP;
  /**
  pingTimer[0] = millis() + 75;
  pingTimer[1] = pingTimer[0] + PING_INTERVAL;
  */
  Serial.println("Starting sonar initialization");
  
  for(uint8_t i = 0; i < NUM_AVG; i++){
    cm[SONAR_L][i] = 0;
    cm[SONAR_R][i] = 0;
    cm[SONAR_SL][i] = 0;
    cm[SONAR_SR][i] = 0;
  }
  
  for(uint8_t i = 0; i < NUM_AVG; i++){
  
    delay(PING_INTERVAL);
    unsigned int uS = sonar[SONAR_L].ping();
    cm[SONAR_L][i] = uS / US_ROUNDTRIP_CM;
    uS = sonar[SONAR_R].ping();
    cm[SONAR_R][i] = uS / US_ROUNDTRIP_CM;
    uS = sonar[SONAR_SL].ping();
    cm[SONAR_SL][i] = uS / US_ROUNDTRIP_CM;
    uS = sonar[SONAR_SR].ping();
    cm[SONAR_SR][i] = uS / US_ROUNDTRIP_CM;
  }
  /**
  Serial.print("Left initial dist: ");
  Serial.print(initialDistL);
  Serial.print("Right initial dist: ");
  Serial.println(initialDistR);
  
    
**/

  Serial.print("Sensor 1: ");
  Serial.print(homeColor[0]);
  Serial.print(" Sensor 2: ");
  Serial.print(homeColor[1]);
  Serial.print(" Sensor 3: ");
  Serial.print(homeColor[2]);
  Serial.print(" Sensor 4: ");
  Serial.println(homeColor[3]);
  
  
  
// driveForward(100);

  startCorner = findStartCorner();
  Serial.print("Start corner: ");
  Serial.println(startCorner);
  
  turnTime = calibrateTurn();
  delay(100);

  if(startCorner == PURPLESTART){
    driveLeft(75);
  } else {
    driveRight(75);
  }

  delay(turnTime);
    driveStop();
  delay(100);

   plannedPath();

  Serial.print("Driving");
  //s.write(BIN_INVERTED);
  //zigZagPath();
}

int findStartCorner()
{
  int leftDist[NUM_AVG];
  int rightDist[NUM_AVG];
  
  for(uint8_t i = 0; i < NUM_AVG; i++){
    leftDist[i] = sonar[SONAR_SL].ping() / US_ROUNDTRIP_CM;
    rightDist[i] = sonar[SONAR_SR].ping() / US_ROUNDTRIP_CM;
  }
  
  int leftMedian = find_median(leftDist);
  int rightMedian = find_median(rightDist);
  
  if(leftMedian != 0 && leftMedian < 20){
    return WHITESTART;
  } else if(rightMedian != 0 && rightMedian < 20){
    return PURPLESTART;
  }
}

int calibrateTurn()
{
   int time = millis();
   if(startCorner == PURPLESTART){
     driveRight(75);
   } else {
     driveLeft(75);
   }
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
  /*
  int leftDist = sonar[SONAR_L].ping_cm(); 
  int rightDist = sonar[SONAR_R].ping_cm(); 
  Serial.print("Left\t");
  Serial.print(leftDist);
  Serial.print("\tRight\t");
  Serial.println(rightDist);
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
      //driveForwardWall(80);
      
      /*
      bool diff1 = different_color(homeColor1, sensor1);
      bool diff2 = different_color(homeColor2, sensor2);
      bool diff3 = different_color(homeColor3, sensor3);
      bool diff4 = different_color(homeColor4, sensor4);
      
      if(diff1 & diff2 & diff3 & diff4)
      {
          driveBack(100);
      }
      else
      {
        driveForward(100);
      }
*/
}


int duration(int action)
{
    switch (action)
  {
    case FORWARD:
        return 3000;
      break;
      case DEDREKON:
        return 100;
      break;
    case FORWARD_SLIGHT:
        return 50;
      break;
    case FORWARD_WALL:
        return 8000;
    break;
    case LEFT:
      return turnTime;
      break;
    case RIGHT:
      return turnTime;
      break;
    case OLEFT:
      return turnTime*1.33;
      break;
    case ORIGHT:
      return turnTime*1.33;
      break;
    case BACK:
      return 1000;
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
  unsigned long startTime;

  //turnTime *=.95;
  //int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, LEFT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP, FORWARD, STOP, RIGHT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP};
 /*int actions [NUM_ACTIONS] = {FORWARD, STOP, CALIBRATE_OPP_COLOR, FORWARD, FORWARD, STOP, STOP, OLEFT, STOP, BACK, STOP, FORWARD_SLIGHT, FORWARD, STOP, DUMP, FORWARD, STOP, DUMP_UP, FORWARD,
                                STOP, OLEFT, STOP, BACK, STOP, FORWARD, STOP, DUMP, STOP, LEFT, STOP, DUMP_UP, STOP, FORWARD, STOP, FORWARD, FORWARD, STOP};*/
       //old path                                                              //^stops here, stop.
//int actions [NUM_ACTIONS] = {FORWARD, STOP, CALIBRATE_OPP_COLOR, FORWARD, FORWARD, STOP, STOP, OLEFT, STOP, BACK, STOP, FORWARD_SLIGHT, FORWARD, FORWARD, FORWARD, STOP, LEFT, STOP, BACK, STOP,
//                                FORWARD_SLIGHT, FORWARD, STOP, DUMP, FORWARD, STOP, DUMP_UP, FORWARD, RIGHT, FORWARD, RIGHT, STOP, BACK, STOP, FORWARD, LEFT, FORWARD, FORWARD, FORWARD, LEFT, FORWARD, DUMP};
int actions [NUM_ACTIONS] = {FORWARD, STOP, CALIBRATE_OPP_COLOR, FORWARD, STOP, STOP, LEFT, STOP, BACK, STOP, DEDREKON, FORWARD, FORWARD, FORWARD, STOP, RIGHT, STOP, DUMP, STOP, FORWARD, STOP,
                                RIGHT, STOP, DUMP_UP, FORWARD, FORWARD, FORWARD, STOP, RIGHT, STOP, BACK, STOP, FORWARD_SLIGHT, FORWARD, FORWARD, FORWARD, STOP, ORIGHT, STOP, BACK, STOP, FORWARD_SLIGHT, 
                                FORWARD, FORWARD, FORWARD, FORWARD, FORWARD, FORWARD, FORWARD, STOP, ORIGHT, STOP, BACK, STOP, DUMP, FORWARD, STOP, DUMP_UP, STOP, RIGHT, STOP, FORWARD, FORWARD, FORWARD, DUMP};
  
//int durations [NUM_ACTIONS] = {1300, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100};

//  int durations [NUM_ACTIONS] = {forwardTime, forwardTime, 100, turnTime, forwardTime, forwardTime, forwardTime, turnTime, forwardTime, 100, turnTime, 100, forwardTime, forwardTime, forwardTime, forwardTime, forwardTime, forwardTime, 100};

 
    for (int i =0; i< NUM_ACTIONS; i++)
    {
      bool change_once = false;
      startTime = millis();
      if(actions[i] == BACK || actions[i] == DEDREKON || actions[i] == STOP || actions[i] == LEFT || actions[i] == RIGHT || actions[i] == FORWARD_WALL || actions[i] == OLEFT || actions[i] == ORIGHT || actions[i] == DUMP_UP){
        while(millis() - startTime < duration(actions[i])){
          doAction(actions[i]);
          delay(5);
        }
      } else if(actions[i] == DUMP){
        int dumpColors[4] = {0, 0, 0, 0};
        for(uint8_t j = 0; j < NUM_AVG; ++j){
          dumpColors[0] += analogRead(PIN_COLORSENSE1);
          dumpColors[1] += analogRead(PIN_COLORSENSE2);
          dumpColors[2] += analogRead(PIN_COLORSENSE3);
          dumpColors[3] += analogRead(PIN_COLORSENSE4);
        }
        for(uint8_t j = 0; j < 4; ++j){
          dumpColors[j] = dumpColors[j]/NUM_AVG;
          Serial.print("Dump color 1: ");
          Serial.println(dumpColors[j]);
        }
        int count = 0;
        for(uint8_t j = 0; j < 4; ++j){
          if(color(dumpColors[j], j) == color(oppositeColor[j], j)){
            Serial.print("Same color as opposite: ");
            Serial.println(j);
            count++;
          }
        }
        if(count == 4){
          Serial.println("Proceed with dump");
          doAction(DUMP);
        } else {
          Serial.println("Wow, don't proceed with dump, under no circumstances");
          emergencyDump();
          return;
        }
      }else if (actions[i] == CALIBRATE_OPP_COLOR)
      {
        int calibrate[4] = {0,0,0,0};
        for (uint8_t j = 0; j < COLORSETUP; ++j)
        {
          calibrate[0] += analogRead(PIN_COLORSENSE1);
          calibrate[1] += analogRead(PIN_COLORSENSE2);
          calibrate[2] += analogRead(PIN_COLORSENSE3);
          calibrate[3] += analogRead(PIN_COLORSENSE4);
        }
        oppositeColor[0] = calibrate[0]/COLORSETUP;
        oppositeColor[1] = calibrate[1]/COLORSETUP;
        oppositeColor[2] = calibrate[2]/COLORSETUP;
        oppositeColor[3] = calibrate[3]/COLORSETUP;
        colorThresh[0] = abs((homeColor[0]-oppositeColor[0]))/4;
        colorThresh[1] = abs((homeColor[1]-oppositeColor[1]))/4;
        colorThresh[2] = abs((homeColor[2]-oppositeColor[2]))/4;
        colorThresh[3] = abs((homeColor[3]-oppositeColor[3]))/4;
        oppositeCalibrated = true;
        Serial.print("Home 1: ");
        Serial.print(homeColor[0]);
        Serial.print(" Home 2: ");
        Serial.print(homeColor[1]);
        Serial.print(" Home 3: ");
        Serial.print(homeColor[2]);
        Serial.print(" Home 4: ");
        Serial.println(homeColor[3]);
        Serial.print("Opp 1: ");
        Serial.print(oppositeColor[0]);
        Serial.print(" Opp 2: ");
        Serial.print(oppositeColor[1]);
        Serial.print(" Opp 3: ");
        Serial.print(oppositeColor[2]);
        Serial.print(" Opp 4: ");
        Serial.println(oppositeColor[3]);
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
       int oppVals[4] = {0,0,0,0};
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
         startColors[j] = color(startVals[j]/NUM_AVG, j);
         startVals[j] = startVals[j]/NUM_AVG;
       }
       for(int j = 0; j < 4; j++){
         if(startColors[j] == WHITESTART){
           if(startCorner == WHITESTART){
             oppVals[j] = oppositeColor[j];
           } else {
             oppVals[j] = homeColor[j];
           }
         } else if(startColors[j] == PURPLESTART){
           if(startCorner == WHITESTART){
             oppVals[j] = homeColor[j];
           } else {
             oppVals[j] = oppositeColor[j];
           }
         }
       }
       int fix = 0;
       int modifier = 0;
       unsigned long dur =  (duration(actions[i]) + modifier);
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
          Serial.print("Start 1 val: ");
          Serial.print(startVals[0]);      
                Serial.print("\tStart 2 val: ");
          Serial.print(startVals[1]);
                Serial.print("\tStart 3 val: ");
          Serial.print(startVals[2]);
                Serial.print("\tStart 4 val: ");
          Serial.println(startVals[3]);
          Serial.print("Sensor 1: ");
        Serial.print(sensor1);      
              Serial.print("\tSensor 2: ");
        Serial.print(sensor2);
              Serial.print("\tSensor 3: ");
        Serial.print(sensor3);
              Serial.print("\tSensor 4: ");
        Serial.println(sensor4);
          Serial.println(color(oppVals[0], 0) == color(sensor1, 0));
          Serial.println(color(oppVals[2], 2) == color(sensor3, 2));
          Serial.print("Opp 1: ");
            Serial.print(oppVals[0]);      
                  Serial.print("\tOpp 2: ");
            Serial.print(oppVals[1]);
                  Serial.print("\tOpp 3: ");
            Serial.print(oppVals[2]);
                  Serial.print("\tOpp 4: ");
            Serial.println(oppVals[3]);
          if(!oppositeCalibrated){
            if (different_color(startVals[0], sensor1))
            {
             count ++; 
            }
            
             //if ( (startColors[3] >= sensor4 && (100*(startColors[3]-sensor4))/startColors[3] < thresh)  || (startColors[3] < sensor4 && (100*(sensor4 - startColors[3]))/sensor4 < thresh))
            if(different_color(startVals[2], sensor3))
            {
             count ++; 
            }
          } else {
            if(color(oppVals[0], 0) == color(sensor1, 0))
            {
             count ++; 
            }
            
             //if ( (startColors[3] >= sensor4 && (100*(startColors[3]-sensor4))/startColors[3] < thresh)  || (startColors[3] < sensor4 && (100*(sensor4 - startColors[3]))/sensor4 < thresh))
             if(color(oppVals[2], 2) == color(sensor3, 2))
            { //kevin told me to change this
             count ++; 
            }
          }
          
          if(count == 2 && !change_once && actions[i] == FORWARD){
            Serial.println("Change once");
              driveStop();
              change_once = true;
//            startTime = millis() - duration(actions[i]) + ;
              dur = 0;
              startTime = millis();

            //startTime -= 500;
            //modifier += 500;
          }
          count = 0;
          
          delay(100);
        }
       driveStop();
      delay(1000);
      }
  }
  driveStop();
  s.write(BIN_INVERTED);     

}


void doAction(int action)
{

  switch (action)
  {
    case FORWARD:
      driveForward(80);
      break;
      case DEDREKON:
      driveForward(60);
      break;
    case FORWARD_WALL:
      driveForwardWall(80);
      break;
    case LEFT:
      if(startCorner == PURPLESTART){
        driveLeft(75);
      } else {
        driveRight(75);
      }
      break;
      
    case FORWARD_SLIGHT:
      if(startCorner == PURPLESTART){
        driveLeft(50);
      } else {
        driveRight(50);
      }
      break;
    case RIGHT:
      if(startCorner == PURPLESTART){
        driveRight(75);
      } else {
        driveLeft(75);
      }
      break;
      case OLEFT:
      if(startCorner == PURPLESTART){
        driveLeft(75);
      } else {
        driveRight(75);
      }
      break;
    case ORIGHT:
      if(startCorner == PURPLESTART){
        driveRight(75);
      } else {
        driveLeft(75);
      }
      break;
    case BACK:
      driveBack(80);
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

void driveForwardLeft(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel*.9);
  analogWrite(PIN_RIGHT_PWM, vel);

  digitalWrite(PIN_LEFT_DIR1, 1);
  digitalWrite(PIN_RIGHT_DIR1, 0);
   digitalWrite(PIN_LEFT_DIR2, 0);
  digitalWrite(PIN_RIGHT_DIR2, 1);
}

void driveForwardRight(int speed)
{
  int vel = speed * 2.55;

  analogWrite(PIN_LEFT_PWM, vel);
  analogWrite(PIN_RIGHT_PWM, vel*.9);

  digitalWrite(PIN_LEFT_DIR1, 1);
  digitalWrite(PIN_RIGHT_DIR1, 0);
   digitalWrite(PIN_LEFT_DIR2, 0);
  digitalWrite(PIN_RIGHT_DIR2, 1);
}

void driveForwardWall(int speed)
{
  int vel = speed * 2.55;
  int leftDist;
  int rightDist;
  
  delay(1);
  //int leftDist = sonar[SONAR_L].ping(); 
  echoCheck(SONAR_L);
  delay(33);
  //int rightDist = sonar[SONAR_R].ping();
  echoCheck(SONAR_R);
  
  leftDist = find_median(cm[SONAR_L]);
  rightDist = find_median(cm[SONAR_R]);
  
  Serial.print("\nSensor 1 l: \t");
  Serial.print(leftDist);      
  Serial.print("\tSensor 2 r: \t");
  Serial.println(rightDist);
  Serial.print("Left stopped: ");
  Serial.print(leftStopped);
  Serial.print("\tRight stopped: ");
  Serial.println(rightStopped);
  Serial.print("Stop Dist: ");
  Serial.println(stopDist);
  
    digitalWrite(PIN_LEFT_DIR1, 1);
  digitalWrite(PIN_RIGHT_DIR1, 0);
   digitalWrite(PIN_LEFT_DIR2, 0);
  digitalWrite(PIN_RIGHT_DIR2, 1);
  
  if(leftDist > 43 || rightDist > 43){
    Serial.println("Both forward, branch 1");
    analogWrite(PIN_LEFT_PWM, vel);
    analogWrite(PIN_RIGHT_PWM, vel);
  } else if(leftStopped && rightStopped){
    if(leftDist < rightDist - 3){
      digitalWrite(PIN_LEFT_DIR1, 0);
      digitalWrite(PIN_LEFT_DIR2, 1);
      analogWrite(PIN_LEFT_PWM, 35);
    } else if(rightDist < leftDist - 3){
      digitalWrite(PIN_RIGHT_DIR1, 0); //kevin
      digitalWrite(PIN_RIGHT_DIR2, 1);
      analogWrite(PIN_RIGHT_PWM, 35);
    }
  
  }else {
    if(leftDist > 34){
      Serial.println("Left forward, greater than 34");
      analogWrite(PIN_LEFT_PWM, vel);
    } else if(leftStopped){
      Serial.println("Left stopped");
      analogWrite(PIN_LEFT_PWM, 0);
    } else if(rightStopped){
      if(abs((leftDist-rightDist)) < 4){
        Serial.println("Right stopped, left equal, stop now");
        leftStopped = true;
        analogWrite(PIN_LEFT_PWM, 0);
      }
      else if(leftDist > rightDist){
        Serial.println("Right stopped, left go forward");
        analogWrite(PIN_LEFT_PWM, 35);
      } else if(leftDist < rightDist){
        Serial.println("Right stopped ,left go backward");
        digitalWrite(PIN_LEFT_DIR1, 0);
        digitalWrite(PIN_LEFT_DIR2, 1);
        analogWrite(PIN_LEFT_PWM, 35);
      }
    } else if(leftDist < 27){
      Serial.println("Left backward, less than 27");
      digitalWrite(PIN_LEFT_DIR1, 0);
      digitalWrite(PIN_LEFT_DIR2, 1);
      analogWrite(PIN_LEFT_PWM, 35);
    } else {
      Serial.println("Left stop now, between 27 and 34");
      analogWrite(PIN_LEFT_PWM, 0);
      leftStopped = true;
      stopDist = leftDist;
    }
    if(rightDist > 34){
      Serial.println("Right forward, greater than 34");
      analogWrite(PIN_RIGHT_PWM, vel);
    } else if(rightStopped){
      Serial.println("Right stopped");
      analogWrite(PIN_RIGHT_PWM, 0);
    } else if(leftStopped){
      if(abs((rightDist-leftDist)) < 4){
        Serial.println("Left stopped, right equal, stop now");
        rightStopped = true;
        analogWrite(PIN_RIGHT_PWM, 0);
      }
      else if(rightDist > leftDist){
        Serial.println("Left stopped, right go forward");
        analogWrite(PIN_RIGHT_PWM, 35);
      } else if(rightDist < leftDist){
        Serial.println("Left stopped ,right go backward");
        digitalWrite(PIN_RIGHT_DIR1, 0); //kevin
        digitalWrite(PIN_RIGHT_DIR2, 1);
        analogWrite(PIN_RIGHT_PWM, 35);
      }
    } else if(rightDist < 27){
      Serial.println("Right backward, less than 27");
      digitalWrite(PIN_RIGHT_DIR1, 0); //kevin
      digitalWrite(PIN_RIGHT_DIR2, 1);
      analogWrite(PIN_RIGHT_PWM, 35);
    } else {
      Serial.println("Right stop now, between 27 and 34");
      analogWrite(PIN_RIGHT_PWM, 0);
      rightStopped = true;
      stopDist = rightDist;
    }
  }

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

void zigZagPath()
{
  Serial.println("Plan path");
  int actionCounter = 0;

  int forwardTime = 1400;
  int startTime;

  //turnTime *=.95;
  //int actions [NUM_ACTIONS] = {FORWARD, FORWARD, STOP, LEFT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP, FORWARD, STOP, RIGHT, STOP, FORWARD, FORWARD, FORWARD, FORWARD, STOP};
  
  //-non-dedrekoned
  //int actions [NUM_ACTIONS] = {FORWARD_WALL, CALIBRATE_OPP_COLOR, STOP, LEFT, STOP, BACK, STOP, FORWARD, STOP, LEFT, STOP, BACK,
  //                              STOP, FORWARD_WALL, STOP, RIGHT, STOP, RIGHT, STOP, BACK, STOP, FORWARD_WALL};
  int actions [NUM_ACTIONS] = {FORWARD, FORWARD, FORWARD, CALIBRATE_OPP_COLOR, STOP, LEFT, STOP, BACK, STOP, FORWARD, STOP, LEFT, STOP, BACK,
                                STOP, FORWARD, FORWARD, FORWARD, STOP, RIGHT, STOP, RIGHT, STOP, BACK, STOP, FORWARD, FORWARD, FORWARD};
                                 
 

  //int durations [NUM_ACTIONS] = {1300, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100, 1300, 100, 400, 100, 1300, 1300, 1300, 1300, 100};

  //int durations [NUM_ACTIONS] = {forwardTime, forwardTime, 100, turnTime, forwardTime, forwardTime, forwardTime, turnTime, forwardTime, 100, turnTime, 100, forwardTime, forwardTime, forwardTime, forwardTime, forwardTime, forwardTime, 100};

 
    for (int i =0; i< NUM_ACTIONS; i++)
    {
      leftStopped = false;
      rightStopped = false;
      bool change_once = false;
      startTime = millis();
      if(actions[i] == STOP || actions[i] == LEFT || actions[i] == RIGHT || actions[i] == FORWARD_WALL){
        while(millis() - startTime < duration(actions[i])){
          doAction(actions[i]);
          delay(5);
        }
      } else if (actions[i] == CALIBRATE_OPP_COLOR)
      {
        int calibrate[4] = {0,0,0,0};
        for (uint8_t j = 0; j < COLORSETUP; ++j)
        {
          calibrate[0] += analogRead(PIN_COLORSENSE1);
          calibrate[1] += analogRead(PIN_COLORSENSE2);
          calibrate[2] += analogRead(PIN_COLORSENSE3);
          calibrate[3] += analogRead(PIN_COLORSENSE4);
        }
        oppositeColor[0] = calibrate[0]/COLORSETUP;
        oppositeColor[1] = calibrate[1]/COLORSETUP;
        oppositeColor[2] = calibrate[2]/COLORSETUP;
        oppositeColor[3] = calibrate[3]/COLORSETUP;
      }
      else {
        
      Serial.print("Action: ");
      Serial.println(i);
       int startVals[4]= {0,0,0,0}; 
       int startColors[4]= {0,0,0,0}; 
       int oppVals[4] = {0,0,0,0};
       for (int j = 0; j < NUM_AVG; j++)
       {
        startVals[0] += analogRead(PIN_COLORSENSE1);
        startVals[1] += analogRead(PIN_COLORSENSE2);
        startVals[2] += analogRead(PIN_COLORSENSE3);
        startVals[3] += analogRead(PIN_COLORSENSE4);
       }
       for(int j=0 ; j<4; j++)
       {
         startColors[j] = color(startVals[j]/NUM_AVG, j); 
         //startColors[j] = startVals[j]/NUM_AVG;
       }
       for(int j = 0; j < 4; j++){
         if(startColors[j] == 1){
           oppVals[j] = oppositeColor[j];
         } else if(startColors[j] == 0){
           oppVals[j] = homeColor[j];
         }
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
          if (same_color(oppVals[0], sensor1, 0))
          {
           count ++; 
          }
          
           //if ( (startColors[3] >= sensor4 && (100*(startColors[3]-sensor4))/startColors[3] < thresh)  || (startColors[3] < sensor4 && (100*(sensor4 - startColors[3]))/sensor4 < thresh))
          if(same_color(oppVals[2], sensor3, 2))
          { //kevin told me to change this
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

void emergencyDump()
{
}

void gameOverMan()
{
  //check color sensors
  //compare to home color
  //if not same dump
  //else if cant go forward go back 1 sqaure and dump
  //else go forward 1 square and dump
  Serial.print("Game over, man! GAME OVER!");
}

