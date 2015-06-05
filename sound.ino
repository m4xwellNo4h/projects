#include <Wtv020sd16p.h>

/*
 Example: Control a WTV020-SD-16P module to play voices from an Arduino board
 */

#include <Wtv020sd16p.h>

int val;
int resetPin = 2;  // The pin number of the reset pin.
int clockPin = 3;  // The pin number of the clock pin.
int dataPin = 4;  // The pin number of the data pin.
int busyPin = 5;  // The pin number of the busy pin.

/*
Create an instance of the Wtv020sd16p class.
 1st parameter: Reset pin number.
 2nd parameter: Clock pin number.
 3rd parameter: Data pin number.
 4th parameter: Busy pin number.
 */
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

void setup() {
  //Initializes the module.
  Serial.begin(9600);
  wtv020sd16p.reset();
}

void loop() {
  // play sound file 0
  val = analogRead(0);
  //wtv020sd16p.asyncPlayVoice(5);
  if(val >= 60){
    wtv020sd16p.asyncPlayVoice(5);
    delay(3000);
    wtv020sd16p.asyncPlayVoice(1);
    delay(3000);
    wtv020sd16p.asyncPlayVoice(2);
    delay(3000);
    wtv020sd16p.asyncPlayVoice(6);
    delay(3000);
    wtv020sd16p.stopVoice();
  }
  Serial.println(val);
  //delay(500);  
  

 

}
