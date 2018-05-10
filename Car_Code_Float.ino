#include <RoboClaw.h>

#include <Stepper.h>
#include <SoftwareSerial.h>
#include <RoboClaw.h>


//See limitations of Arduino SoftwareSerial
SoftwareSerial serial(10,11);  
RoboClaw roboclaw(&serial,10000);

#define address 0x80

float goalSpeed = -1;
float currentSpeed = -1;
float goalDegree = 0;
double currentDegree = 0; 
long currentStepSteer = 0;
long currentStepAccel = 0;
int goalStepSteer = 0;
const int stp1 = 13; // sets step and direction pins for drivers stp1 = x and stp2 = y  
const int dir1 = 12;  
const int steerStp = 6;
const int steerDir = 7;
const float tol = 1.5;

void setup() {
  //Open roboclaw serial ports
  roboclaw.begin(38400);
  Serial.begin( 19200 );
  // Sets the  pins as Outputs
  pinMode(stp1,OUTPUT); 
  pinMode(dir1,OUTPUT);
  pinMode( steerStp, OUTPUT );
  pinMode( steerDir, OUTPUT );
  roboclaw.BackwardM2(address, 127);
  delay(2500);
  roboclaw.BackwardM2(address, 0);
}

void loop() {

  readIncoming();
  if ( goalSpeed == 0 ) {
    goHome();
    brake();
  } else if ( currentSpeed < goalSpeed && currentStepAccel <= 40 ) {
    acellerateOneStep();
    currentStepAccel++;
  } else if ( currentSpeed > goalSpeed && currentStepAccel >= 0 ) {
    decelerateOneStep();
    currentStepAccel--;
  }
  
  if ( goalDegree > currentDegree + tol && currentStepSteer <= 1200 && currentStepSteer >= -1200 ) {
    steerRight();
  } else if ( goalDegree < currentDegree - tol && currentStepSteer <= 1200 && currentStepSteer >= -1200 ) {
    steerLeft();
  }
}

/**
 * First number is goal, second number is actual.
 */
void readIncoming() {
  float incomingGoalSpeed;
  float incomingCurrentSpeed;
  float incomingGoalDeg = 0;
  if ( Serial.available() > 12 ) {
    if ( Serial.find( ":" ) ) {
      incomingGoalSpeed = Serial.parseFloat();
      if ( incomingGoalSpeed != goalSpeed ) {
        goalSpeed = incomingGoalSpeed;
      }
      delayMicroseconds( 50 );
      incomingCurrentSpeed = Serial.parseFloat();
      if ( incomingCurrentSpeed != currentSpeed ) {
        currentSpeed = incomingCurrentSpeed;
      }
      delayMicroseconds( 50 );
      incomingGoalDeg = Serial.parseFloat();
      if ( incomingGoalDeg != goalDegree ) {
        goalDegree = incomingGoalDeg;
      }
    }
  

    /*  
      Serial.write( Serial.print(goalSpeed) );
      Serial.write( " " );
      Serial.write( Serial.print(currentSpeed) ); 
      Serial.write( " " );
      Serial.write( Serial.print( goalDegree ) ); 
      Serial.write( " " );
      Serial.write( Serial.print( currentDegree, 6 ) );
      Serial.write( "\n");
      */
      
  }
}

void steerRight() {
  
  digitalWrite( steerDir, HIGH );
  for ( int n = 0; n < 175 && goalDegree > currentDegree; n++ ) {
    digitalWrite( steerStp, HIGH );
    delayMicroseconds( 100 );
    digitalWrite( steerStp, LOW );
    delay( 1 );
    currentStepSteer++;
    convertToDegree();
  }
  
}

void steerLeft() {
  digitalWrite( steerDir, LOW );
  for ( int i = 0; i < 175 && goalDegree < currentDegree; i++ ) {
    digitalWrite( steerStp, HIGH );
    delayMicroseconds( 100 );
    digitalWrite( steerStp, LOW );
    delay( 1 );
    currentStepSteer--;
    convertToDegree();
  }
}

void acellerateOneStep() {

  digitalWrite( dir1, LOW );
  delayMicroseconds( 50 );
  digitalWrite( stp1, HIGH );
  delayMicroseconds( 50 );
  digitalWrite( stp1, LOW );
  //delay( 4 );

}

void decelerateOneStep() {

  digitalWrite( dir1, HIGH );
  delayMicroseconds( 50 );
  digitalWrite( stp1, HIGH );
  delayMicroseconds( 50 );
  digitalWrite( stp1, LOW );
  //delay( 4 );
  
}
 

void brake() {
  roboclaw.ForwardM2( address, 127 );
  delay(2500);
  roboclaw.BackwardM2( address, 127 );
  delay(1500);
  while ( goalSpeed == 0 ) {
    roboclaw.ForwardM2( address, 0 );
    readIncoming();
  }
  roboclaw.BackwardM2( address, 127 );
  delay(1000);
  roboclaw.ForwardM2( address, 0 );
}

void goHome() {
  digitalWrite( dir1, HIGH);
  for ( int i = currentStepAccel; i >= 0; i-- ) {
      digitalWrite( stp1, HIGH);
      delayMicroseconds(50);
      digitalWrite( stp1, LOW );
      delay( 2 );
      currentStepAccel--;
  }
}

void convertToDegree() {
  currentDegree = currentStepSteer / 2.22;
}

int convertToStep( float degree ){
  return (int) degree / 4.4;
}

