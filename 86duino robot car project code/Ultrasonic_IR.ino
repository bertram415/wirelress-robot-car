
void Ultrasonic_with_IRtrack()
{
  //Ultrasonic();
  distance = Ultrasonic();

  if (distance > 25 )
  {
    Serial.println(message);
    // IR tracking with vocal message "work"
    while ( message == 2 )
    {
      //Ultrasonic();
      distance = Ultrasonic();
      if (distance > 25 && UltrasonicFlag == 0 )
      {
        IRtrack();
      }

      else if (distance <= 25 && UltrasonicFlag == 0)
      {
        halt();
        //回報遇有障礙！
        if (CARNUMBER == "1")
        {
          msg.data = error1;
          delay(200);
        }
        else if (CARNUMBER == "2")
        {
          msg.data = error2;
          delay(200);
        }
        chatter_pub.publish( &msg );
        Serial.println("Less than 25cm");
        nh.spinOnce();
      }

      else if (distance > 25 && UltrasonicFlag == 1 )
      {
        halt();
        //不執行Ultrasonic
        //等待 Servo command
      }

      else if (distance <= 25 && UltrasonicFlag == 1)
      {
        halt();
        //不執行Ultrasonic
        //等待 Servo command
      }
      delay(1);
      nh.spinOnce();
    }


    // Auto IR tracking without vocal message
    while (IRflag == 1)
    {
      //Ultrasonic();
      distance = Ultrasonic();
      if (distance > 25 && UltrasonicFlag == 0 )
      {
        IRtrack();
      }

      else if (distance <= 25 && UltrasonicFlag == 0)
      {
        halt();
        //回報遇有障礙！
        if (CARNUMBER == "1")
        {
          msg.data = error1;
          delay(200);
        }
        else if (CARNUMBER == "2")
        {
          msg.data = error2;
          delay(200);
        }
        chatter_pub.publish( &msg );
        nh.spinOnce();
      }
      /*else if (distance > 25 && UltrasonicFlag == 1 ) // 基本上 UltrasonicFlag = 1 不會發生，進來迴圈前為 0，歸為1後已跳出迴圈而進不來。但以防萬一，所以放著。
      {
        halt();
        // UltrasonicFlag = 1時，不執行Ultrasonic
        // 等待 Servo command
      }

      else if (distance <= 25 && UltrasonicFlag == 1) // 基本上 UltrasonicFlag = 1 不會發生，進來迴圈前為 0，歸為1後已跳出迴圈而進不來。但以防萬一，所以放著。
      {
        halt();
        // UltrasonicFlag = 1時，不執行Ultrasonic
        // 等待 Servo command
      }*/
      delay(1);
    }

    count = 0;
  }

  else if (distance <= 25 )
  {
    //Serial.println(distance);
    //Serial.println("cm <= 30");
    halt();
    //delay(500);
    /*****回報遇有障礙！！！*****/
    if (CARNUMBER == "1")
      msg.data = error1;
    else if (CARNUMBER == "2")
      msg.data = error2;
    chatter_pub.publish( &msg );
    nh.spinOnce();
    //    backward();
    //    delay(500);
    //    halt();
    //    count = 100;
  }
  delay(1);
}


void IRtrack() {
  //Black is 1 ,White is 0
  int IR_pos;
  IR_pos = IRvalue();

  switch (IR_pos)
  {
    case 0:  //000
      Serial.println("IR000");
      forward();
      break;
    case 1:  //001
      Serial.println("IR001");
      turnRight_rotate();
      delay(5);
      break;
    case 2:  //010
      Serial.println("IR010");
      forward();
      break;
    case 3:  //011
      Serial.println("IR011");
      turnRight();
      break;
    case 4:  //100
      Serial.println("IR100");
      turnLeft_rotate();
      delay(5);
      break;
    case 5:  //101
      Serial.println("IR101");
      break;
    case 6:  //110
      Serial.println("IR110");
      turnLeft();
      break;
    case 7:  //111
      backward();
      delay(10);
      halt();    
      Serial.println("IR111");
      if (message == 2)
        message = 0;
      if (IRflag == 1)
        IRflag = 0;
      if (place >= 1)
        place = 0;       
      else
        place++;     
        UltrasonicFlag = 1;  
      break;
  }
}

int Ultrasonic() {

  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  int time = pulseIn(EchoPin, HIGH);
  int cm = time / 58;
  String result = "Distance:";
  result += cm;
  result += "(cm).";

  //Serial.println(result);
  //Serial.write(cm);
  return cm;
}


int IRvalue()
{
  if (analogRead(Left_IR) > 900)
    Left_IR_Value = 1;
  else
    Left_IR_Value = 0;
  if (analogRead(Middle_IR) > 900)
    Middle_IR_Value = 1;
  else
    Middle_IR_Value = 0;
  if (analogRead(Right_IR) > 900)
    Right_IR_Value = 1;
  else
    Right_IR_Value = 0;  
    
  pos = Left_IR_Value * 4 + Middle_IR_Value * 2 + Right_IR_Value;
  return pos;
}
