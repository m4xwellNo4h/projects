#include <Servo.h>

Servo servo1;
int val;
boolean recentlyOpened;

void setup()
{
servo1.attach(9);
recentlyOpened = true;
}
void loop()
{
  val = analogRead(0);  
  int position;
  servo1.write(0);  
 
  if(val <= 500 && recentlyOpened == true){
    recentlyOpened=false;
    for(position = 180; position >= 0; position -= 20)
    {
      servo1.write(position);
      delay(500); 
    }
  } 
  
  if (val>=800){
    recentlyOpened = true; 
  }
} 
