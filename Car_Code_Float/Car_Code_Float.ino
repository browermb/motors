#include <Adafruit_MCP4725.h>
#include <Wire.h>

Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;

int throttleRelay = 9;
int brakeRelay = 8;
const int magicStart = 42;
const int magicEnd = 21;


void setup() {
  dac1.begin(0x62);
  dac2.begin(0x63);
  pinMode(throttleRelay, OUTPUT);
  pinMode(brakeRelay, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    delay(15);
  }
  Serial.println("STARTING");
}

void loop() {
 
  delay(5);
  int throttle = -1;
  int brakeVal = -1;
  int steering = -1;

  int firstByte = -1;
  int secondByte = -1;
  while (firstByte != magicStart || secondByte != magicEnd) {
    firstByte = Serial.read();
    secondByte = Serial.read();
      Serial.println(firstByte);
    Serial.println(secondByte);
  }
    
  /**
  if (firstByte != -1) {
Serial.println(firstByte);
    while(firstByte != magicStart) {
      firstByte = Serial.read();
    }

    int 

    while(secondByte != magicEnd) {
      firstByte = Serial.read();
      while(firstByte != magicStart) {
        firstByte = Serial.read();
      }
      secondByte = Serial.read();
    }
    **/
    throttle = Serial.read();

    brakeVal = Serial.read();

    byte steeringByteVal1 = Serial.read();
    byte steeringByteVal2 = Serial.read();
    steering = calculateVal(steeringByteVal1, steeringByteVal2);
    Serial.println(firstByte);
    Serial.println(secondByte);
    Serial.println(throttle);
    Serial.println(brakeVal);
    Serial.println(steering);
  

  if(throttle != -1 && brakeVal != -1 && steering != -1) {
    
    accel(throttle);
    brake(brakeVal);
    //steer(steering);
  }
}

void accel(int throttleVal) {
  int val = map(throttleVal, 0, 255, 409, 3645);
  Serial.println("Setting throttle...");
  
  dac1.setVoltage(val, false);
  Serial.print("Throttle set ");
  Serial.println(throttleVal);
  if (throttleVal == 0) {
    digitalWrite(throttleRelay, LOW);
  } else {
    digitalWrite(throttleRelay, HIGH); 
  }
}

void brake(int brakeVal) {
  int val = map(brakeVal, 0, 255, 409, 3767); 
  Serial.println("Setting brake...");
  dac2.setVoltage(val, false);
  Serial.println("Brake set.");
  if (brakeVal == 255) {
    digitalWrite(brakeRelay, LOW);
  } else {
    digitalWrite(brakeRelay, HIGH); 
  }
}

void steer(int steerVal) {

}

int calculateVal(byte byteVal1, byte byteVal2) {
  return ((byteVal1 & 0xff) << 8 | (byteVal2 & 0xff));

}
