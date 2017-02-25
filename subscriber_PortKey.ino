//This code recieves input from user side and analyse it. And 
//then decides the actions to take 

//important modules (libraries) 
#include <Wire.h>
#include <Bridge.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Servo.h>
#include <JsonParser.h>
#include <HttpClient.h>

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//initialization for motors
Adafruit_BNO055 bno = Adafruit_BNO055(55);

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);

//variables for servo and 
Servo servo1; 
Servo servo2;
int xAng;
int yAng;

int currentZ=0;
int toHead=0;
int ourHead=0;
int error=0;

int mode;
int leftSpeed;
int rightSpeed;


//function runs at starting or reset time
void setup(void) 
{
  //start the bridge, and set pins for servo
  Bridge.begin();
  servo1.attach(9);
  servo2.attach(10);
  //Serial.begin(9600);
  Console.begin(); //starts the console connection
  //Console.println("Orientation Sensor Test"); Serial.println("");
  delay(1000);

  //to test if bno is connected properly
  if(!bno.begin())
  {
    Console.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  
    
  bno.setExtCrystalUse(true);

  AFMS.begin();
 
}

//runs in loop
void loop() {
 Console.print("Loop Start");
 
 
 int copyTT=0, skip=0;
 String TT ="";

  //initialize client, and connect it to the server. 
  //loop until conected, and reads data
  HttpClient client;
  client.get("http://pubsub.pubnub.com/subscribe/sub-c-f2c256ae-a20b-11e6-b095-02ee2ddab7fe/demoM/0/0");
  while (client.available()) {
    char c = client.read();
    
    if (c=='\"'){
      if (copyTT == 0){
        copyTT = 1;
        skip = 1;
      }
      else{
        copyTT = 0;
      }
    }
    
    if (copyTT==1 && skip==0){
        TT += c;
    }
    skip = 0;
    
    //Console.print(c);
  }
  Serial.println("");
  Serial.println(TT);
  Serial.println("");

  //read and parse the recived data
  String json="";
  int copyJSON = 0;
  client.get("http://pubsub.pubnub.com/subscribe/sub-c-f2c256ae-a20b-11e6-b095-02ee2ddab7fe/demoM/0/"+TT);
  while (client.available()) {
    char c = client.read();
    
    if (c=='{' || c=='}'){
      if (c=='{'){
        json = "";
      }
      else{}
      if (copyJSON==0){
        copyJSON = 1;
      }
      else{
        copyJSON = 0;
      }
    }

    if (copyJSON==1){
    
      json += c;
    }

    
    //Console.print(c);
  }

  json += "}";
  //Console.println("");
  //Console.println(json);
  //Console.println("");
  //Console.flush();

  JsonParser<32> parser;
  
  char jsonChar[json.length()+1];
  json.toCharArray(jsonChar, json.length()+1);
  JsonHashTable hashTable = parser.parseHashTable(jsonChar);
  
  if (!hashTable.success())
    {
        //Console.println("JsonParser.parseHashTable() failed");
        return;
    }

    String roll = hashTable.getString("roll");
    //Console.print("Roll=");
    //Console.println(roll);
    xAng = map(roll.toInt(), 90, -90, 0, 180);

    String pitch = hashTable.getString("pitch");
    //Console.print("Pitch=");
    //Console.println(pitch);
    yAng = map(pitch.toInt(), -90, 90, 0, 180);

    String yaw = hashTable.getString("yaw");
    //Console.print("Yaw=");
    //Console.println(yaw);

    String forward = hashTable.getString("forward");
    //Console.print("Forward=");
    //Console.println(forward);

    //char* backward = hashTable.getString("backward");
    //Console.print("Backward=");
    //Console.println(backward);

    //char* left = hashTable.getString("left");
    //Console.print("Left=");
    //Console.println(left);

    String rotation = hashTable.getString("right");
    //Console.print("Right=");
    //Console.println(rotation);

    servoWrite();

    delay(20);
    //control the up and rot 
    int up = forward.toInt();
    int rot = rotation.toInt(); 
    Console.println(up);
    if (up >530 || up<510 || rot<480 || rot>500){

      if (up>530) {
        //MODE 1 = BACKWARD
        mode = 1;
      }
      else if (up<510) {
        //MODE 0 = FORWARD
        mode = 0;
      }
      else{
        //MODE 3 = PURE ROTATION
        mode = 3;
      }
      //////////////////////////////////////////////////
      if (mode==1){
        leftSpeed  = map(up,530,1024,0,120);
        rightSpeed = leftSpeed;
        if (rot>500){
          rightSpeed = rightSpeed - map(rot,500,1024,0,30);
        }
        else if (rot<480){
          leftSpeed = leftSpeed - map(rot,480,0,0,30);
        }
        Motor1->setSpeed(leftSpeed); 
        Motor2->setSpeed(rightSpeed);
        Motor1->run(BACKWARD);
        Motor2->run(BACKWARD);
      }
      else if (mode==0){
        leftSpeed  = map(up,510,0,0,120);
        rightSpeed = leftSpeed;
        if (rot>500){
          rightSpeed = rightSpeed - map(rot,500,1025,0,30);
        }
        else if (rot<480){
          leftSpeed = leftSpeed - map(rot,480,0,0,30);
        }
        Motor1->setSpeed(leftSpeed); 
        Motor2->setSpeed(rightSpeed);
        Motor2->run(FORWARD);
        Motor1->run(FORWARD);
      } 

    else if (mode==3){

      leftSpeed = 70;
      
      if (rot>500){
          
          Motor1->setSpeed(leftSpeed); 
          Motor2->setSpeed(leftSpeed);
          Motor2->run(BACKWARD);
          Motor1->run(FORWARD);
        }
        else if (rot<480){
          
          Motor1->setSpeed(leftSpeed); 
          Motor2->setSpeed(leftSpeed);
          Motor2->run(FORWARD);
          Motor1->run(BACKWARD);
        }

    }

    }
    else{
      Motor1->setSpeed(0); 
      Motor2->setSpeed(0);
      Motor2->run(FORWARD);
      Motor1->run(FORWARD);

      ///TURN BASED ON IMU
     sensors_event_t event; 
     bno.getEvent(&event);
      //mapping head movements to base motors speed and direction. 
     currentZ = (int) event.orientation.x;
     Console.print("CurrentZ :");
     Console.print(currentZ);
     Console.print("  ");
     Console.print("YAW :");
     Console.print(yaw.toInt());
     Console.print("  ");

     toHead = map(yaw.toInt(),0,360,-180,180);
     ourHead = map(currentZ,0,360,-180,180);
     error = sqrt((toHead-ourHead)*(toHead-ourHead));
     int curr_speed = map(error,0,360,30,150);

     if (toHead >= ourHead){
      //TURN RIGHT
      Motor1->setSpeed(curr_speed);
      Motor1->run(FORWARD);
      Motor2->setSpeed(curr_speed);
      Motor2->run(BACKWARD);
     }else{
      //TURN LEFT
      Motor1->setSpeed(curr_speed);
      Motor1->run(BACKWARD);
      Motor2->setSpeed(curr_speed);
      Motor2->run(FORWARD);
      
     }


     Console.print("Error :");
     Console.print(error);
     Console.print("  ");
     //int curr_speed = 50;
     //int curr_speed = map(abs(diff),0,360,0,160);
     Console.print("Speed:  ");
     Console.print(curr_speed);
     Console.print("  ");
    }
    
}

//to set the angle for servo
void servoWrite(){
  servo1.write(xAng);
  servo2.write(yAng);
}




