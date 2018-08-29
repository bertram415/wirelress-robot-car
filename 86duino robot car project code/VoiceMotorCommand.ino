
void start()  // initial move forward
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  digitalWrite(M3, HIGH);
  digitalWrite(M4, LOW);
  delay(1);
  analogWrite(EN1, normalSpeed+10);
  analogWrite(EN2, normalSpeed+10);
}


void forward()  // move forward
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  digitalWrite(M3, HIGH);
  digitalWrite(M4, LOW);
  delay(1);
  analogWrite(EN1, normalSpeed-4);
  analogWrite(EN2, normalSpeed-4);
}

void speedIncrease()  // speed increase
{
  _currentSpeed += speedDelta;
  // Keep _currentSpeed under maximum value
  if (_currentSpeed > 100)
    _currentSpeed = 100;
}

void speedDecrease()  // speed decrease
{
  _currentSpeed -=  speedDelta;
  // Keep _currentSpeed positive
  if (_currentSpeed < 5)
    _currentSpeed = 5;
}

void turnLeft_rotate()  // turn left rotate
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  digitalWrite(M3, HIGH);
  digitalWrite(M4, LOW);
  delay(1);
  analogWrite(EN1, normalSpeed-3);
  analogWrite(EN2, normalSpeed-3);
}

void turnLeft()         // turn left
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  digitalWrite(M3, HIGH);
  digitalWrite(M4, LOW);
  delay(1);
  analogWrite(EN1, normalSpeed);
  analogWrite(EN2, fullSpeed-3);
}

void turnRight_rotate()  // turn right rotate
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  digitalWrite(M3, LOW);
  digitalWrite(M4, HIGH);
  delay(1);
  analogWrite(EN1, normalSpeed-3);
  analogWrite(EN2, normalSpeed-3);
}

void turnRight()         // turn right
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  digitalWrite(M3, HIGH);
  digitalWrite(M4, LOW);
  delay(1);
  analogWrite(EN1, fullSpeed-3);
  analogWrite(EN2, normalSpeed);
}

void backward()  // move backward
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, HIGH);
  delay(1);
  analogWrite(EN1, normalSpeed);
  analogWrite(EN2, normalSpeed);
}

void halt()  // stop motor
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, HIGH);
  delay(1);
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
}

void motorDisable()
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, LOW);
  delay(1);
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
}

void ReverseRightTurn()
{
  while( pos != 2)
  {
    Serial.println("Reverse Right");
    pos = IRvalue();
    if (pos != 2)
    {
      turnRight_rotate();
    }
    else
      break;
  }
  nh.spinOnce();  
  halt();
}

void ReverseLeftTurn()
{
  while( pos != 4)
  {
    Serial.println("Reverse Left");
    pos = IRvalue();    
    if (pos != 4)
    {
      turnLeft_rotate();
    }
    else
      break;
  }
  nh.spinOnce();   
  halt();
}

void Shake()
{
  turnRight_rotate();
  delay(50);
  turnLeft_rotate();
  delay(50);
  turnRight_rotate();
  delay(50);
  turnLeft_rotate();
  delay(50);
}

void ledBlink()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(LED, HIGH - digitalRead(LED)); //toggle led blink & blink 3 times
    delay(100);
  }
  digitalWrite(LED, HIGH);  //toggle led stay on
}

