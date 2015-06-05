#include <LiquidCrystal.h>
#include <SoftwareSerial.h>  
#include <LinkedList.h>
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>


LiquidCrystal lcd(25,24,27,26,29,28);
int sandman[] = {1,3,5,7,6,5,3,1,2,4,6,8,7,1,1,3,5,7,6,5,3,1,2,4,6,8,7,1,1,3,5,7,6,5,3,1,2,4,6,8,7,1};
SdFat sd;
SFEMP3Shield MP3player;

const int FLOORTIME = 3000;
const int NUMFLOORS = 9;
const int PLAY = 1;
const int READ = 0;
int lightLevel[NUMFLOORS];
int high[NUMFLOORS];
int low[NUMFLOORS];

int floorsVisited = 0;
int currentFloor = 0;
int previousFloor = 0;
int lastTime = 0;
boolean stopped = 0;

const int PUSHED = LOW; 

int startButton = 22;
int stopButton =23;

int mode = 2;

LinkedList<int> floorList = LinkedList<int>();
LinkedList<int> timeList = LinkedList<int>();

void setup()
{
  // LED initialization
  Serial.begin(9600);
  for (int i = 0; i<NUMFLOORS;i++){
    pinMode(i+30,OUTPUT); //LEDs start at pin 30 and go sequentially
  }
  Serial.println("begin");
  
  // Array initialization
  for (int i = 0; i < NUMFLOORS;i++) {
    lightLevel[i]=0;
    high[i]=0;
    low[i] = 1023;
  }
  // SD card and MP3 initialization
  sd.begin(SD_SEL, SPI_HALF_SPEED);
  MP3player.begin();
  uint16_t volume = 0;
  MP3player.setVolume(volume);
  
  // LCD initialization
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Want to hear our elevator pitch?");
  
  //button initialization
  pinMode(startButton, INPUT);
 
  pinMode(stopButton, INPUT);
}


void loop()
{
  
  //read buttons
  int startState = digitalRead(startButton);
  int stopState = digitalRead(stopButton);
  
  if (startState == PUSHED && mode==READ) {
    // enter PLAY mode
    mode=PLAY;
  } else if (mode == PLAY && stopState!=PUSHED){
    // stay in PLAY
    
  } else if  (mode==PLAY && stopState==PUSHED){
    // enter READ mode
    mode=READ;
  } else {
    //stay in READ mode
  }
  if (mode==PLAY){ // enter play mode when start button is pushed
    int j = 0;
    while (stopState!=PUSHED){ // exit play mode when stop button pushed.
      int rand = pickStart();
      playFromList(rand+j % 9);
      delay(300);
      MP3player.stopTrack();
      j++;
    }
  } else if (mode==READ){
  
  
    int currentTime = millis();
    previousFloor = currentFloor;
    readFloors();
    Serial.print("Current Floor: ");
    Serial.println(currentFloor);
    for (int i=0; i<NUMFLOORS;i++){
      if (i+1!=currentFloor) {
         digitalWrite(i+30,LOW);
      } else {
        digitalWrite(i+30,HIGH);
      }
    }
    if (previousFloor != currentFloor) {
       Serial.println("Floor Changed: ");     
       Serial.println(floorsVisited);
       stopped=false;
       lastTime = currentTime;
    } else if (currentTime - lastTime > FLOORTIME && !stopped){
      floorsVisited++;
      stopped = true;
      floorList.add(currentFloor);
      timeList.add(millis());
    }
  }
  delay(100);
}


void autoTune()
{
//  Serial.print("Reading floors: ");
  for (int i =0;i< NUMFLOORS; i++){
    if (lightLevel[i]< low[i]){
      low[i] = lightLevel[i];
    }
    if (lightLevel[i]> high[i]){
      high[i] = lightLevel[i];
    }
//    Serial.println("Gonna start without adjusting");
//    Serial.print(" Floor #");
//    Serial.print(i+1);
//    Serial.print(": ");
//    Serial.print(lightLevel[i]);
    lightLevel[i] = map(lightLevel[i], low[i]+30, high[i]-30, 0, 255);
    lightLevel[i] = constrain(lightLevel[i], 0, 255); // 255 - lightLevel is nightlight mode
//    Serial.print(" Floor #");
//    Serial.print(i+1);
//    Serial.print(": ");
//    Serial.print(lightLevel[i]);
  }
  Serial.print("\n");
  
  
}

void readFloors(){ // this is where we will implement funtionality for reading the values
  for (int i =0;i< NUMFLOORS; i++){
    lightLevel[i] = analogRead(i+7);
  }
  autoTune();
  int tmp = lightLevel[0];
  //argmax i in lightLevel
  for (int i =0;i< NUMFLOORS; i++){
    tmp = max(tmp,lightLevel[i]);
    if (lightLevel[i]==tmp){
      currentFloor = i+1; // current floor is analog input + 1
    }
  }
}

int pickStart() { // returns the rand so that you know where to start
  int rand = random(0,floorList.size());
  // use random number to add things find time
  int time = timeList.get(rand);
  int ago = millis()-time; // milliseconds since 
  // decide what time to display using ago
  lcdDisplay(ago);
  return rand;
}
// uses a time to decide what message to display
void lcdDisplay(int ago) {
  //decide what to display
  lcd.clear();
  int seconds = ago/1000;
  int minutes = seconds/60;
  int hours = minutes/60;
  if (hours) {
    lcd.print(hours);
    lcd.print(" hours ago...");
  } else if (minutes) {
    lcd.print(minutes);
    lcd.print(" minutes ago..");
  } else {
    lcd.print("Just seconds ago!!!");
  }
}
// plays a floor's sound using an index in the array of floors
void playFromList(int index){
  int note = floorList.get(index);
  MP3player.playTrack(note);
}
// Use this for defeating cold start.
void initializeFloorArray(){
  int len = 42;
  for (int i = 0; i< len;i++){
    floorList.add(random(0,9)); 
    timeList.add((42-i)*-10000);
  }
  
  // {1,3,5,7,6,5,3,1,2,4,6,8,7,1,1,3,5,7,6,5,3,1,2,4,6,8,7,1,1,3,5,7,6,5,3,1,2,4,6,8,7,1}
}
