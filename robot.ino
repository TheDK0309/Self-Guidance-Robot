#include <Servo.h>
Servo myservo; 
int pos = 0;   
 
const int trig = 6;   //trigger pin of ultrasonic sensor
const int echo = 5;   //echo pin of ultrasonic sensor

//pins for 2 DC motors
int forward1=10;         
int forward2=11;          
int backward1=12;          
int backward2=13;  
        
int dongcoservo=9;      //pin for servo motor
 
int maximum = 15;       //maximum distance allowed
int i;
int x=0;
unsigned long time_count; //time
int distance;    //distance      
int distance_left,distance_right;  //distance to the left/right
 
void distance_mes();  //distance measurement
void go_forward();//go forward
void go_left();//go to left
void go_right();//go to right
void go_backward();//go backward
void reset_motor(); //reset the DC motor
void turn_servo_right();//turn the ultrasonic sensor to the right
void turn_servo_left();//turn the ultrasonic sensor to the left

void setup() {  
    myservo.attach(9); 
    pinMode(trig,OUTPUT);  //trigger pin of ultrasonic sensor is output
    pinMode(echo,INPUT);   //echo pin of ultrasonic sensor is input 
    //motor pins are output
    pinMode(forward1,OUTPUT); 
    pinMode(forward2,OUTPUT); 
    pinMode(backward1,OUTPUT); 
    pinMode(backward2,OUTPUT); 
    //turn off all DC motors at start
    digitalWrite(forward1,LOW);
    digitalWrite(forward2,LOW);
    digitalWrite(backward1,LOW);
    digitalWrite(backward1,LOW);
   // Serial.begin(9600);
    myservo.write(90);    
    delay(1000);
   
    
    
}
 
void loop() 
{   
    distance=0;
    distance_mes();
    if(distance>maximum||distance==0) //reset the distance if it exceeds the limit,or at the start 
    {
      distance_mes(); //measure the distance
       if(distance>maximum||distance==0) //if distance > limit or at the start, go forward
        {
          go_forward();   
        }   
    }
    else //if distance<=limit(detected obstacle)
    {
      reset_motor();
      turn_servo_left(); //turn the sensor to the left
      
      distance_left=distance; //measure left distance
     
      turn_servo_right(); //turn the sensor to the right
      distance_right=distance; //measure right distance
      if(distance_right<10&&distance_left<10){ //if both distance to the left and right <backward10, go backward
        go_backward();
      }
      else
      {
        if(distance_right>distance_left) //if left distance<right distance, go to left
        {
          go_left();
          delay(1000); //1s
        }
        if(distance_right<distance_left) //if right distance<left distance, go to right
        {
          go_right();
          delay(1000); //1s
        }
      }
    }
  //Serial.println(distance);
}

void go_forward() //go foward
{ 
        //turn both forward pins of motors on,both DC motor turn in same direction, counter-clock wise
        digitalWrite(forward1,HIGH);
        digitalWrite(forward2,HIGH);
        //delay(2);
   
}
 
void go_left()
{
  //
  reset_motor();
  digitalWrite(backward1,HIGH); //backward pin of left motor on, makes left motor turns clockwise, right motor stop
  delay(1000);
  digitalWrite(backward1,LOW); //turn off motor
  
    
}

void go_right()
{
  reset_motor();
  digitalWrite(backward2,HIGH);//backward pin of right motor on, makes right motor turns clockwise, left motor stop
  delay(1000);
  digitalWrite(backward2,LOW);//turn off motor
  
}
 
void go_backward()
{
  reset_motor();
  for(i=0;i<20;i++)
      //turn on both backward pins
        digitalWrite(backward1,HIGH);
        digitalWrite(backward2,HIGH);
        delay(1000);
    //after going backward for 20,turn both pins down 
    digitalWrite(backward1,LOW);
    digitalWrite(backward2,LOW);
}
 
void reset_motor()  //set all motor pins to low
{
  digitalWrite(forward1,LOW);
  digitalWrite(forward2,LOW);
  digitalWrite(backward1,LOW);
  digitalWrite(backward2,LOW);
}

void distance_mes() //distance measurement
{
    digitalWrite(trig,0);  //turn off trigger at start
    delayMicroseconds(2);  //wait 2ms
    digitalWrite(trig,1);  //turn the trigger on
    delayMicroseconds(10);  //wait 10ms
    digitalWrite(trig,0);   //turn trigger off
    
    time_count = pulseIn(echo,HIGH); //time when echo pin in HIGH state  
    distance = int(time_count/2/29.412); //distance calculation
    
}

void turn_servo_left()
{
    myservo.write(180);    //turn servo motor to 180 degree position(ultrasonic sensor turn to left)         
    delay(1000);          //wait 1 sec
    distance_mes();       //measure the distance
    myservo.write(90);    //turn the motor back to 900 degree position(ultrasonic sensor point forward)    
}

void turn_servo_right()
{
    myservo.write(0);        //turn servo motor to 0 degree position(ultrasonic sensor turn to right)         
    delay(1000);              //wait 1 sec
    distance_mes();           //measure the distance
    myservo.write(90);       //turn the motor back to 900 degree position(ultrasonic sensor point forward)       
}


