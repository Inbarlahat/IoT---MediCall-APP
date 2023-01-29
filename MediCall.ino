/*************************************************************

  Exercise: Blynk Messaging App // MediCall – App for hotspital patients and medical staff (primarly nurses) :)

  The system I demonstrated is a hospital mobile system that allows patients and medical staff to communicate.
  In this app, there are two tabs, the first and most important one being "Home" - it has some features for instant needs. 
  
  The green button will let the medical staff know whether a patient is in need of a meal (if so - he will also be choosing the heat level).
  The nurse will see the green light amount (between 1-10) and the number represents how warm the patient wants the food would be.
  
  In case the patient is having an emergency, the system will turn the lights RED and an emergency siren will play. 
  Once the staff will notice, they will cancel the alarm by pressing the left button (and of course they'll go straight to the patient and alert other medical staff if needed).
  
  If the patient is cold he can ask for a blanket via the app (on the blanket button) and the blue lights will turn on - which will alert the staff to bring the blanket to the patient.
  
  Regarding the second tab - "Next Dose Timer", when patienta are in need to take medicine in specific times, 
  they can set the timer to the next time they need to take it, then, once the time is up - an alarm will let the staff 
  know and they will update the patient and give them their medicine as needed.
  
  In case there is a shift change of the medical staff, they will click for a few seconds on the right click and a push message 
  will pop up and will let the patients know that their needs will be taken care of shortly. 

  Created By Inbar Lahat 315976647
 
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLYhvYhU65"
#define BLYNK_DEVICE_NAME           "MediCall"
#define BLYNK_AUTH_TOKEN            "q_cTg26oDsG6J3H7B7YZPbuEVBTyPqYG"

// Comment this out to disable prints and save space
#define BLYNK_PRINT SerialUSB

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Omer";
char pass[] = "hashla109";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))

// modes
bool EmergencyBtn = false;
bool approveButton = false;
bool MealBtn = false;
bool BlanketBtn = false;
bool StartTimerCountdown = false;
bool cancelTimer = false;
int countdownRemain = 0;
int countdownTimer = 0;
int mealHeatLevel = 0;
int nextDoseHour;
int nextDoseMin;


int buttons[] = {V2, V3, V4, V8};

// resets all the modes 
void resetAll() 
{
  EmergencyBtn = false;
  approveButton = false;
  MealBtn = false;
  BlanketBtn = false;
  mealHeatLevel = 0;
  nextDoseHour = 0;
  nextDoseMin = 0;

  CircuitPlayground.clearPixels();
  CircuitPlayground.playTone(0, 0);
  Blynk.virtualWrite(V0, "Hey there! Choose an action :)");
  Blynk.virtualWrite(V6, "Waiting for instructions");

  for (int i = 0; i < ARRAY_LENGTH(buttons); i++) {
    Blynk.virtualWrite(buttons[i], 0);
  }
}

void resetTimer() 
{
  nextDoseHour = 0;
  nextDoseMin = 0;
  StartTimerCountdown = false;
  cancelTimer = false;
  countdownRemain = 0;
  countdownTimer = 0;
  Blynk.virtualWrite(V13, "");
}

BlynkTimer timer;

// check if emergancy button is clicked
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    EmergencyBtn = true;
    Blynk.virtualWrite(V0, "HELP IS ON THE WAY!");
  } 
}

// check if food button is clicked
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    MealBtn = true;
    Blynk.virtualWrite(V0, "Got you ;) Your meal is coming right up.");
  }
}

// blanket push button
BLYNK_WRITE(V4)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    BlanketBtn = true;
    Blynk.virtualWrite(V0, "Got you ;) Getting your blanket.");
  }
}

// heat volume
BLYNK_WRITE(V8)
{
  int pinValue = param.asInt();
  mealHeatLevel = pinValue;
}

// check the hours the man wants to sleep
BLYNK_WRITE(V9)
{
  int pinValue = param.asInt();
  nextDoseHour = pinValue;
}

// check the minutes the man wants to sleep
BLYNK_WRITE(V10)
{
  int pinValue = param.asInt();
  nextDoseMin = pinValue;
}

// Press to set timer function
BLYNK_WRITE(V11)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {   
    countdownRemain = (nextDoseHour * 60 + nextDoseMin) * 60;
    StartTimerCountdown = true;
  }
}

// cancel the dose timer push button
BLYNK_WRITE(V12)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    cancelTimer = true;
  }
}

void setup()
{
  CircuitPlayground.begin();
  
  // Debug console
  SerialUSB.begin(115200);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);
  delay(10);
  
  // Setup a function to be called every second
  timer.setInterval(1000L, setStartTimerCountdown);
}


// sets emergancy status
void emergancyStatus() 
{
  for (int i = 0; i < 10; i++) {
       CircuitPlayground.setPixelColor(i, 255,   0,   0);
   }  

  if (approveButton) 
  {
    Serial.println("EMERGENCY STAFF ARRIVING");
    delay(1000);
    resetAll();
    } else {
      Serial.println("EMERGENCY RINGTON");
      CircuitPlayground.playTone(500, 0);
      }
}

// sets food status
void getFood() {
   for (int i = 0; i < mealHeatLevel; i++) {
       CircuitPlayground.setPixelColor(i, 0,   255,   0);
   }
   for (int i = mealHeatLevel; i < 10; i++) {
       CircuitPlayground.setPixelColor(i, 0,   0,   0);
   }

   if (approveButton) {
    resetAll();
   }
}

// getting a blanket notification
void getBlanket() {
   for (int i = 0; i < 10; i++) {
       CircuitPlayground.setPixelColor(i, 0,   0,   255);
   }

   if (approveButton) {
    resetAll();
   }
}

void CountdownShowFormatted(int seconds) {
  long hours = 0;
  long mins = 0;
  long secs = 0;
  String secs_o = ":";
  String mins_o = ":";
  String hours_o = "";
  secs = seconds; // set the seconds remaining
  mins = secs / 60; //convert seconds to minutes
  hours = mins / 60; //convert minutes to hours
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max

  if (secs < 10) {
    secs_o = ":0";
  }
  if (mins < 10) {
    mins_o = ":0";
  }
  if (hours < 10) {
    hours_o = "0";
  }
  
  Blynk.virtualWrite(V13, "Next dose is in " + hours_o + hours + mins_o + mins + secs_o + secs);
}

void setStartTimerCountdown() 
{
  if(StartTimerCountdown == false)
  {
    Serial.println("Coundown OFF");
  }
  
  if(countdownRemain > 0 && StartTimerCountdown == true)
  {
    Serial.println("Coundown ON");
    countdownRemain--;
    CountdownShowFormatted(countdownRemain); 
    }
  
  else if (countdownRemain == 0 && StartTimerCountdown == true)
  {
    Serial.println("Coundown FINISHED");
    for (int i = 0; i < 10; i++) 
    {
       CircuitPlayground.setPixelColor(i, 255,0,100);
       }
    if (approveButton) 
    {
      resetAll();
      resetTimer();
      } else {
        CircuitPlayground.playTone(500, 0); 
        }  
      }

    if (cancelTimer) {
      resetAll();
      resetTimer();
    }  
}
  
void loop()
{
  Blynk.run();
  timer.run();

  // check modes by importance
  if (EmergencyBtn) {
    emergancyStatus();
  } else if (MealBtn) {
    getFood();
  } else if (BlanketBtn) {
    getBlanket();
  }

  // confirms that the med team saw the message
  bool leftButtonPressed = CircuitPlayground.leftButton();
  if (leftButtonPressed) {
    approveButton = true;
  }

   bool rightButton = CircuitPlayground.rightButton();
  if (rightButton) {
    Blynk.virtualWrite(V0, "Message from the medical staff:");
    Blynk.virtualWrite(V6, "We are changing shifts, we will be right back!!");
  }
}
