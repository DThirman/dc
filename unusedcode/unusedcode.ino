 
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
