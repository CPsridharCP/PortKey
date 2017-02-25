//user side code to read the data from sensor, pack in a string
//and send it to server
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Bridge.h>
#include <Console.h>
#include <HttpClient.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

int forward=0;
int rotation =0;

//runs initially or when reset
void setup(void) 
{
  Bridge.begin();
  Console.begin();
  if(!bno.begin())
  {
    Console.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
 
  bno.setExtCrystalUse(true);
}

//keep sensing data and send it to server
void loop(void) 
{
  sensors_event_t event; 
  bno.getEvent(&event);
  //Console.println("INLOOP");
  //read from server
  forward = analogRead(A1);
  rotation  = analogRead(A0);
  //Console.print("Forward: ");
  //Console.print(forward);
  //Console.print("Rotatino: ");
  //Console.println(rotation);

  //Console.print("X: ");
  //Console.print(String(event.orientation.x,0));
  //Console.print("\tY: ");
  //Console.print(String(event.orientation.y,0));
  //Console.print("\tZ: ");
  ///Console.print(String(event.orientation.z,0));
  //Console.println("");
  //delay(100);

  //if(digitalRead(7) == 1){
  //  Console.println("Delay 5 second");
  //  delay(5000);
  // }

  //setup a client server which will send data to server
  HttpClient client;
  client.get("http://pubsub.pubnub.com/publish/pub-c-4bd7749f-0d30-4a7f-8179-bb367b2d757a/sub-c-f2c256ae-a20b-11e6-b095-02ee2ddab7fe/0/demoM/0/%7B\"roll\":\""+String(event.orientation.z,0)+"\",\"pitch\":\""+String(event.orientation.y,0)+"\",\"yaw\":\""+String(event.orientation.x,0)+"\",\"forward\":\""+String(forward)+"\",\"right\":\""+String(rotation)+"\"%7D");
  //client.get("http://pubsub.pubnub.com/publish/pub-c-4bd7749f-0d30-4a7f-8179-bb367b2d757a/sub-c-f2c256ae-a20b-11e6-b095-02ee2ddab7fe/0/demoM/0/%7B\"roll\":\"000\",\"pitch\":\"000\",\"yaw\":\"000\",\"forward\":\"000\",\"backward\":\"000\",\"right\":\"000\",\"left\":\"000\"%7D");
  //while (client.available()) {
  //  char c = client.read();
  //  Console.print(c);
  // }
  //Console.println("");
  //Console.flush();
  delay(30);
  
}

