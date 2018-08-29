/*
 * Rosserial Wireless car project
 * Voice control motor/LED over rosserial
 * IR sensor for track following
 */

#include <ros.h>
#include <std_msgs/String.h>
#include <Servo.h>
//#include <std_msgs/UInt16.h>
//#include <std_msgs/Empty.h>

#define RosSrvIP "192.168.2.53"   // Please double check Server IP address
#define SSID "P847"        // Modify the SSID here according to WiFi AP setting
#define PSWD "62118530"   // Modify the Password here according to WiFi AP setting
#define CARNUMBER "1"     // Modify the CARNUMBER parameter here according to the number of cars.

#define LED 31            // LED IO
#define EN1 3             // Left motor enable signal
#define M1 4              // Left motor IO-1
#define M2 5              // Left motor IO-2
#define M3 6              // Right motor IO-1
#define M4 7              // Right motor IO-2
#define EN2 9             // Right motor enable signal
#define Left_IR A0        // Left IR sensor 
#define Middle_IR A1      // Middle IR sensor 
#define Right_IR A2       // Right IR sensor 
#define TrigPin 11        // Ultrasound Trig pin
#define EchoPin 12        // Ultrasound Echo pin

ros::NodeHandle nh;


Servo servo1;
Servo servo2;

unsigned int servo1_angle = 0;
unsigned int servo2_angle = 0;

std_msgs::String msg;
std_msgs::String pmsg;
std_msgs::String chat_msg;
std_msgs::String link;

char error1[8] = "1:ERROR";
char error2[8] = "2:ERROR";
int LED_STATUS = LOW;    // LED status flag,
/* LOW -> LED is off, HIGH -> LED is on */
int MOTOR_STATUS = LOW;  // Motor control status flag,
/* LOW -> motor disabled, HIGH -> motor being controlled */
int normalSpeed = 50;  // Normal speed for motor pwm
int _currentSpeed = 0;
int fullSpeed = 75;    // Full speed for motor pwm
int halfSpeed = 40;    // Half speed for motor pwm
int speedDelta = 10;   // Speed delta for increment/derement
int Left_IR_Value;     // Left IR sensor value
int Middle_IR_Value;   // Middle IR sensor value
int Right_IR_Value;    // Right IR sensor value
int pos;
int message = 0;
int wifiRetryCount = 0;// wifiRetryCount flag to print current retry times
int count = 0;         // Ultrasonic counting flag in order to detect obstacles every 100 counts out of loop().
int inc = 0;           // inc: increment flag to ignore the first 4 sets of data on ultrasonic to prevent false detect
int distance = 0;
int UltrasonicFlag = 0;
int IRflag = 0;
int place = 0;
int hand = 0;

ros::Publisher chat("chatter", &chat_msg);
ros::Publisher chatter_pub("chatter", &msg);
ros::Publisher chatter_link("chatter", &link);
ros::Publisher position_pub("position", &pmsg);

// Servo callback function
void servoCb(const std_msgs::String& smsg) {
  int servo_angle_cmd = 0;
  if (strcmp(smsg.data, "1:30") == 0 )
  {
    Serial.println("Servo 1:30");    
    if ((CARNUMBER == "1") && (place != 1))
    {   
      //delay(100);      
      servo_angle_cmd = 30;
      servo1_angle = servo_angle_cmd;
      servo2_angle = 180 - servo_angle_cmd;   
      hand = 0;     
      //nh.spinOnce();
    }
  }
  else if (strcmp(smsg.data, "2:30") == 0)
  {
    Serial.println("Servo 2:30");     
    if ((CARNUMBER == "2") && (place != 1))
    { 
      //delay(100);       
      servo_angle_cmd = 30;
      servo1_angle = servo_angle_cmd;
      servo2_angle = 180 - servo_angle_cmd;
      hand = 0;      
      //nh.spinOnce();
    }
  }
  else if (strcmp(smsg.data, "1:60") == 0)
  {
    Serial.println("Servo 1:60");      
    if ((CARNUMBER == "1") && (place != 0))   // Only Car 1 will react to this while loop
    {
      //delay(100);        
      servo_angle_cmd = 55;  //reduce from 60 to avoid mechanical inteference.
      servo1_angle = servo_angle_cmd;
      servo2_angle = 180 - servo_angle_cmd;
      hand = 1;
      //nh.spinOnce();      
    }
  }
  else if (strcmp(smsg.data, "2:60") == 0)
  {
    Serial.println("Servo 2:60");      
    if ((CARNUMBER == "2") && (place != 0))    // Only Car 2 will react to this while loop
    {
      //delay(100);        
      servo_angle_cmd = 55; //reduce from 60 to avoid mechanical inteference
      servo1_angle = servo_angle_cmd;
      servo2_angle = 180 - servo_angle_cmd;
      hand = 1;     
      //nh.spinOnce();      
    }
  }    

  servo1.write(servo1_angle); //set servo1 angle, should be from 30 - 55
  servo2.write(servo2_angle); //set servo2 angle, should be from 30 - 55
}

// Message (string) callback function
void messageCb(const std_msgs::String& mmsg) {
  
  if ( (strcmp(mmsg.data, "work") == 0) && (CARNUMBER == "1"))
  {
    message = 2;
    count = 100;
    UltrasonicFlag = 0;
    // To resolve initial friction
    start();
    delay(50);
    
    Ultrasonic_with_IRtrack();
  }
  //debug test purpose only

  if (strcmp(mmsg.data, "1:work") == 0)
  {
    if (CARNUMBER == "1")
    {    
      delay(100);
      message = 2;
      count = 100;
      UltrasonicFlag = 0;
      // To resolve initial friction
      start();
      delay(50);          
      
      Ultrasonic_with_IRtrack();
    }
  }  
  
  if (strcmp(mmsg.data, "2:work") == 0)
  {
    if (CARNUMBER == "2")
    {    
      delay(100);
      message = 2;
      count = 100;
      UltrasonicFlag = 0;
      // To resolve initial friction
      start();
      delay(50);      
            
      Ultrasonic_with_IRtrack();
    }
  }    
  
  else if ( (strcmp(mmsg.data, "light on") == 0) && (LED_STATUS == LOW) )
  {
    message = 1;
    ledBlink(); // blink led for 3 times and stay on
    //delay(100);
    LED_STATUS = HIGH;
  }
  
  else if ( (strcmp(mmsg.data, "light close") == 0) && (LED_STATUS == HIGH) )
  {
    message = 1;
    digitalWrite(LED, LOW);  //toggle led off
    //delay(100);
    LED_STATUS = LOW;
  }

  // if receive slower command, speed + 10 (pwm value)
  else if ( strcmp(mmsg.data, "faster") == 0 )
  {
    if (_currentSpeed < 100)
    {
      message = 1;
      speedIncrease();
    }
  }

  // if receive slower command, speed - 10 (pwm value)
  else if ( strcmp(mmsg.data, "slower") == 0 )
  {
    if (_currentSpeed >= 15)
    {
      message = 1;
      speedDecrease();
    }
  }
  else if ( strcmp(mmsg.data, "go") == 0 )
  {
    message = 1;
    forward();
    MOTOR_STATUS = HIGH;
  }
  else if ( strcmp(mmsg.data, "turn left") == 0 )
  {
    message = 1;
    turnLeft();
    MOTOR_STATUS = HIGH;

  }
  else if ( strcmp(mmsg.data, "turn right") == 0 )
  {
    message = 1;
    turnRight();
    MOTOR_STATUS = HIGH;
  }

  else if ( strcmp(mmsg.data, "rotate left") == 0 )
  {
    message = 1;
    turnLeft_rotate();
    MOTOR_STATUS = HIGH;

  }
  else if ( strcmp(mmsg.data, "rotate right") == 0 )
  {
    message = 1;
    turnRight_rotate();
    MOTOR_STATUS = HIGH;
  }

  else if ( strcmp(mmsg.data, "back") == 0 )
  {
    message = 1;
    backward();
    MOTOR_STATUS = HIGH;
  }
  else if ( strcmp(mmsg.data, "stop") == 0 )
  {
    message = 1;
    halt();
    MOTOR_STATUS = LOW;
  }

  else
  {
    message = 0;
    motorDisable();
    MOTOR_STATUS = LOW;

    for (int i = 0; i < 20; i++)
    {
      digitalWrite(LED, HIGH - digitalRead(LED));
      delay(100);
    }
  }
  message = 0;
}

// chatter <std_msgs::String> callback function
void chatterCb(const std_msgs::String& cmsg) {
  //char carReply[22];

  if (strcmp(cmsg.data, "Car 1 error. Flag = 1") == 0)
  {
    if (CARNUMBER == "2")
    {
      chat_msg.data = "OK, Car 2 rolls out.";
      chat.publish( &chat_msg );
    }
  }
  else if (strcmp(cmsg.data, "Car 2 error. Flag = 2") == 0)
  {
    if (CARNUMBER == "1")
    {
      chat_msg.data = "OK, Car 1 rolls out.";
      chat.publish( &chat_msg );
    }
  }
}

//ros::Publisher chatter_pub("chatter", &msg);
//ros::Publisher position_pub("position", &pmsg);

ros::Subscriber<std_msgs::String> servo("servo", &servoCb);
ros::Subscriber<std_msgs::String> chatter_sub("chatter", &chatterCb);
ros::Subscriber<std_msgs::String> voice("/recognizer/output", &messageCb);


void setup()
{
  // LED pin setup
  pinMode(LED, OUTPUT);
  // DC Motor pin for wheel setup
  pinMode(M1,  OUTPUT);
  pinMode(M2,  OUTPUT);
  pinMode(M3,  OUTPUT);
  pinMode(M4,  OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  // IR track pin setup
  pinMode(Left_IR,   INPUT);
  pinMode(Middle_IR, INPUT);
  pinMode(Right_IR,  INPUT);
  // Ultrasonic pin setup
  pinMode(TrigPin,   OUTPUT);
  pinMode(EchoPin,   INPUT);
  // Servo Motor pin setup
  servo1.attach(10); // attach servo1 pwm to pin 10
  servo1.write(35);  // set servo1 to low position
  servo2.attach(13); // attach servo2 pwm to pin 13
  servo2.write(145); // set servo2 to low position (reverse to servo1)
  _currentSpeed = normalSpeed;

  nh.getHardware()->setBaud(115200);

  nh.getHardware()->setESP8266(Serial1, 115200);
  nh.getHardware()->setWiFi(SSID, PSWD); // Connect to WiFi AP SSID/PSWD
  //while(!Serial);  // Uncomment this line for debug only,
  // system will wait for Serial Monitor to be opened
  delay(100);
  Serial.print("Initializing Node Handler.");
  delay(100);
  Serial.print(" .");
  delay(100);
  Serial.print(" .\n");
  nh.initNode(RosSrvIP);
  delay(100);

  nh.advertise(chat);
  nh.advertise(chatter_pub);
  nh.advertise(chatter_link);  
  nh.advertise(position_pub);
  nh.subscribe(servo);
  nh.subscribe(chatter_sub);  
  nh.subscribe(voice);
}

void loop()
{
  while (!nh.connected())   // Node initialization retry for WiFi
  {
    if (!nh.connected()) {
    
      if (wifiRetryCount > 0)
      {
        Serial.print(wifiRetryCount); 
        Serial.println(" - Reinitializing Node Handler due to failure occurs");
      }
      nh.initNode(RosSrvIP);  // Retry to initialize Node Handler
      nh.spinOnce();
      delay(1000);
 
      wifiRetryCount++;      
    }
  }  
  wifiRetryCount = 0;  

  if (count >= 100 && UltrasonicFlag == 0 )
  {
    if (inc >= 4)
    {
      //Serial.print("ultrasonic increment = ");
      //Serial.println(inc);
      Ultrasonic_with_IRtrack();
    }
    inc++;

  }
  else
    count++;
  pos = IRvalue();
  //String link_msg = "";
  //link_msg += "Car ";
  //link_msg += CARNUMBER;
  //link_msg += " is linked.";
  //link.data = link_msg.c_str();
  
  switch(pos)
  {
    case 7:
      if (place == 1)
      {
        String temp = "";
        temp += "Car ";
        temp += CARNUMBER;
        temp += " is in position.";
        pmsg.data = temp.c_str();
        position_pub.publish( &pmsg );
        //nh.spinOnce();
      }
      else if (place == 0)
      {
        String temp = "";
        temp += "Car ";
        temp += CARNUMBER;
        temp += " is back to origin.";
        pmsg.data = temp.c_str();
        position_pub.publish( &pmsg );   
        //nh.spinOnce(); 
      }
      break;
    default:
      break;  
  }
  
  Serial.print("Place = ");
  Serial.println(place);  
  /*Serial.println(count);
  Serial.print("pos = ");  
  Serial.println(pos);
  Serial.print("hand = ");
  Serial.println(hand);  
  Serial.print("UltrasonicFlag = ");
  Serial.println(UltrasonicFlag);    
  */
  


  //if (nh.connected())
  nh.spinOnce();
  delay(1);  
  //Serial.println("Car 1 is linked.");
  goBackToOrigin();
}

void goBackToOrigin()
{

  if (place == 1 && UltrasonicFlag == 1 && hand == 1)
  {
    delay(300);
    ReverseRightTurn();
    delay(1);
    IRflag = 1;
    UltrasonicFlag = 0;   
    delay(1);
    Ultrasonic_with_IRtrack();
    delay(1);
    servo1.write(30);
    servo2.write(150);     
    delay(3000);     
    hand = 0;
    backward();
    delay(400);
    //Shake();    
    turnLeft_rotate();
    delay(950);
    halt();
    //ReverseLeftTurn();
  }
}

