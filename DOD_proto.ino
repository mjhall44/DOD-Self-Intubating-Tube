//Prototype demo for Senior Design Group 5
//v1 - 2/13 last edit
//notes made core of program, motors won't connect on wire mechanism
//v2 - 2/21 last edit
// better tracking algo for vscel sensors

//program takes input from vscel sensors and allows for new direction
#include <SparkFun_VL6180X.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#define STEPS 200 //steps/rev
#define VL6180X_ADDRESS 0x29
//add address here when the second motorshield v2 comes in, individually address each
//powered by 2 12 v dc connections, one to board and one to top motor shield (check led lights cycle)?

double ir_dist, ir_amb, prev;
int dirl, diru, count;
int thresh = 15;
int baudSensor = 115200;
int pdLength = 3; // how many previous values to test against
double pastValRL[3]; // right left
double pastValUD[3]; //up down 

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
VL6180x sensor(VL6180X_ADDRESS);

Adafruit_StepperMotor *stepper17A = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *stepper17B= AFMS.getStepper(200, 2);
//Stepper stepper23(STEPS, 4, 5, 6, 7);

void setup() {

  Serial.begin(baudSensor); //Start Serial at 115200bps
    //vscel sensor setup
  Serial.println("Start, please make sure the device is initially aimed!");
  Wire.begin(); //Start I2C library
  delay(100); // delay
  Serial.println("Wire init!");
  sensor.VL6180xDefautSettings(); //Load default settings to get started.
  Serial.println("VL6180x default");
  if (sensor.VL6180xInit() != 0) {
    Serial.println("FAILED TO INITALIZE"); //Initialize device and check for errors
  }else{
    Serial.println("Range finder found.");
    };
   //delay(1000);
   // delay 1s
  Serial.println("Stepper test!");
  // set the speed of the motor to 30 RPMs
  //AFMS.begin();
  Serial.println("Motor Board Complete!");
  stepper17A->setSpeed(20);
  stepper17A->setSpeed(20);Serial.println("Speeds Set!");
//  stepper23.setSpeed(25);


  //vscel sensor setup
  
  //note old set up here, now fills first few values of the array
  count =0;
  //prev = sensor.getDistance();
  //delay(100);

  //new vscel setup
  for(int i =0; i < pdLength; i++){
    pastValRL[i] =sensor.getDistance();
    pastValUD[i] = pastValRL[i];
    delay(200); //overdelayed to allow sensor warmup and any kind of positioning error
    }

  //co2 sensor setup and interface
  //questions on how to have multiple sdc connect
  // can you just do an analog in?
  
 Serial.println("Setup Complete!");
}

void loop() {


  //print baud rate on monitor, ensure syncing
  //Serial.println("Baud Rate %d", baudSensor); //why can't you do this in arduino haha
  
  //grab sensor data for that movement cycle
  ir_dist = sensor.getDistance(); //double var type
  ir_amb  = sensor.getAmbientLight(GAIN_1);
  delay(10);
// simple testing of getting values
   Serial.println(ir_dist);
   Serial.println(ir_amb);

  /*
   * pick direction based on previous counters
  count = count+1;
  count = count%10;
  if (count < 5){
    
    Serial.println("Going Left");
    //if(prev < ir_dist){
      stepper17A->step(20, FORWARD, SINGLE);
   // }else{
   
    Serial.println("Going Right");
      stepper17A->step(20, BACKWARD, SINGLE);
  //  };
    
  }else{
    Serial.println("Going UP");
    //if(prev < ir_dist){
      stepper17B->step(20, FORWARD, SINGLE);
    //}else{
    
    Serial.println("Going Down");
      stepper17B->step(20, BACKWARD, SINGLE);
    //};
  };
  */
  //end of old code
  
  
  
  
  /*move motors
  Serial.println("Forward");
  stepper17A->step(20, FORWARD, SINGLE);
  Serial.println("Backward");
  stepper17A->step(20, BACKWARD, SINGLE);
  
  stepper17B->step(20, FORWARD, SINGLE);
  Serial.println("Backward");
  stepper17B->step(20, BACKWARD, SINGLE);

  */
 // prev = ir_dist;

  for(count = 0; count < 4; count++){
    prev = ir_dist;
    thresh = 15;
   //better direction picking algo 2/20
   //assuming stepperA moves forward is left movement
   //stepperB forward is upward
   //count tied to direction. four cycles of comparison
   //need to make a way to swap right left on count depending on previous value
   //count zero is center value
    if (count = 0){
      pastValRL[1] = ir_dist;
      pastValUD[1] = ir_dist;
    }
   //count 1 goes left, logs (if obvious skip count 2)
    if (count = 1){
      dirl = 0; // 1 is going left, 0 center, -1 going right
      stepper17A->step(20, FORWARD, SINGLE);
      delay(20);
      ir_dist = sensor.getDistance();
      delay(20);
      pastValRL[0] = ir_dist;
      if( pastValRL[1] -ir_dist > thresh){
          dirl = 1;
          count = 2; // skips right if past thresh
      }
    }
   //count 2 goes center then right
      if (count = 2){
      stepper17A->step(40, BACKWARD, SINGLE);
      delay(40);
      ir_dist = sensor.getDistance();
      delay(20);
      pastValRL[2] = ir_dist;
      if(pastValRL[1] -ir_dist > thresh && pastValRL[1]-pastValRL[0] < pastValRL[1] -ir_dist){
          dirl = -1; // skips right if past thresh
      }
    }
    //count 3 goes up, logs (if obvious skip count 4)
    if (count = 3){
      diru = 0;
      stepper17B->step(20, FORWARD, SINGLE);
      delay(20);
      ir_dist = sensor.getDistance();
      delay(20);
      pastValRL[0] = ir_dist;
      if( pastValRL[1] -ir_dist > thresh){
          diru = 1;
          count = 4; // skips right if past thresh
      }
    }
   //count 4 goes center then down
      if (count = 4){
      stepper17B->step(40, BACKWARD, SINGLE);
      delay(40);
      ir_dist = sensor.getDistance();
      delay(20);
      pastValUD[2] = ir_dist;
      if(pastValUD[1]-ir_dist > thresh && pastValUD[1]-pastValUD[0] < pastValUD[1] -ir_dist){
          diru = -1;
          count = 2; // skips right if past thresh
      }
    }
    
    
  } //end of picking a direction

  //now that you have decided a direction (you are already in bottom right)
  if( diru = 1){
    stepper17B->step(40, FORWARD, SINGLE);
    delay(40);
  } else if( diru =0){ // stay centered so no need to shift past
    stepper17B->step(20, FORWARD, SINGLE);
    delay(20);
  }
    if( dirl = 1){
    stepper17A->step(40, FORWARD, SINGLE);
    delay(40);
    //shift left with new array
    //save prev 3 direction
  } else if( dirl =0){ // stay centered so no need to shift past
    stepper17A->step(20, FORWARD, SINGLE);
    delay(20);
  }else{
    //shift right with newArray
    //save prev 3 direction
  }
  
  //move forward with third motor now that you are directionally tuned
  //stepper23->step(30, FORWARD, SINGLE);

  //this then repeats

  
  //things to still add
  //previous array values (adding 2dimensions shouldnt be that hard [3 for each direction][paststacks]
  //this slope between paststacks could be useful in directional gridding (how to do linear regression in arduino?)
  //add on in picking direction section (see commented out sectors on newARRAY
  //array shifting to optimize (since we 'know' the last center value
  //ex if we are going left, we already have our right (prev Center) and center (Prev left) values
  //althogh this saves writing and reduces the amount of get sensor functions, not sure if we have the accuracy/ stability for this


  //ir camera idea
  //outputs a grid of temperature values that can be displayed
  // like our saving grid idea, have past stacks dimesion
  // would need some image recognition to understand tomography, however the center should always be coolest (low temp toward lungs) point to aim for
  // smaller sensors but on the same baud rate
  // would be WAAYYYYYY more accurate and easier to calculate any kind of gradient descent (much larger array off one read rather than individually filling values
  // once we have our gradient, we have our direction
}
