#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SoftwareSerial.h>


// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define JoyBTN 2
#define TRIG_PIN 9
#define ECHO_PIN 10
#define SPEED_OF_SOUND 340.29


//String AP = "Elamark_2.4GHz";       // CHANGE ME
//String PASS = "0547002008"; // CHANGE ME
//String API = "618OJ83CAPS0FQVH";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
//String field = "field1";
int countTrueCommand;
int countTimeCommand;
int valSensor = 1;
int count = 0;
int Index = 0;
bool once = true;
int maxSonic = 1000;
String Options[5] = {"GeneralAnalog", "Digital", "AdjustableAlg", "Help"};


int SSIDINDEX = 0;
int ALPHINDEX = 1;
int VlasToSend[250];
bool DataSaveType = true; // True = one in X, False = average in x
int AmountForSetion = 5;
int TillSaveData = 5;

void setup() {
  Serial.begin(9600);

  pinMode(12, OUTPUT);
  analogWrite(12, 10);

  pinMode(JoyBTN, INPUT);
  pinMode(8, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(JoyBTN, HIGH);

  display.begin();
  display.setContrast(60);
  display.display();
  display.clearDisplay();
}

int i = -1;
int Press = 0;
bool SubMode = true;
int Mode = 0;
int maxx = 100;



int get_distance(int mx) {
  int duration;
  int distance;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  /* Measure the sensor's output pulse */
  duration = pulseIn(ECHO_PIN, HIGH);
  /* Calculate the distance */
  distance = SPEED_OF_SOUND * (duration / 2) / 10000;

  if (distance > mx) {
    distance = mx;
  }
  if (distance < 1) {
    distance = 1;
  }
  return distance;
}
/*
   Modes -
   0. Menu
   1. Genral Analog
   2. Genral Digital
   3. Limited
*/


void loop() {
  int JoyX = analogRead(X_pin);
  int JoyY = analogRead(Y_pin);
  count = 0;

  switch (Mode) {
    case 0:
      display.clearDisplay();

      display.println(Options[0]);
      display.println(Options[1]);
      display.println(Options[2]);
      display.println(Options[3]);

      if (JoyY > 600) {
        Index += 1;
      } else if (JoyY < 200) {
        Index -= 1;

      }

      Options[0] = "GeneralAnalog";
      Options[1] = "Digital";
      Options[2] = "AdjustableAlg";
      Options[3] = "Help";


      if (digitalRead(JoyBTN) == 0) {
        ChangeMode(Index + 1);
        once = true;
      }
      if (Index > 3) {
        Index = 0;
      } else if (Index < 0) {
        Index = 3;
      }
      Options[Index] += "<";
      //Serial.println(JoyY);

      // down = 1034 up = 0 middle ~ 498

      break;
    case 1:
      if (once) {
        display.clearDisplay();
      }
      if (digitalRead(JoyBTN) == HIGH) {
        i = Graf(1024, analogRead(A2), i);
      }
      once = false;
      if (digitalRead(JoyBTN) != HIGH) {
        display.clearDisplay();
        display.print(analogRead(A2));
        once = true;
        i = 0;
      }
      if (JoyX < 100) {
        ChangeMode(0);
      }
      break;
    case 2:
      if (once) {
        display.clearDisplay();
      }
      if (digitalRead(JoyBTN) == HIGH) {
        i = Graf(1, digitalRead(8), i);
      }
      once = false;
      if (digitalRead(JoyBTN) != HIGH) {
        display.clearDisplay();
        display.print(digitalRead(8));
        once = true;
        i = 0;
      }
      if (JoyX < 100) {
        ChangeMode(0);
      }
      if (TillSaveData == 0) {

      }
      break;
    case 3:
      if (once) {
        display.clearDisplay();
        once = false;
      }/*
      if (digitalRead(JoyBTN) == HIGH) {*/

      i = Graf(200, get_distance(maxSonic), i);
      if (JoyX < 100) {
        ChangeMode(0);
      }/*

      }
      once = false;

      if (digitalRead(JoyBTN) != HIGH) {
        display.clearDisplay();
        display.println(get_distance(maxSonic));
        display.println("");
        display.println("Swipe Up/Down To Change");
        display.println("Max Value: ");
        display.println(String(maxSonic));
        once = true;
        if (JoyY > 600) {
          maxSonic -= 50;
        } else if (JoyY < 200) {
          maxSonic += 50;
        }
        i = 0;
      }*/

      break;

    case 4:
      display.clearDisplay();// ADD HELP !!!
      display.println("This is the help panel. Good luck.");
      if (JoyX < 200) {
        ChangeMode(0);
      }
      break;

  }
  display.display();
  delay(100);
}

int joyStickChange(bool xExes) {
  if (xExes) {
    if (analogRead(X_pin) > 800) {
      return 2;
    } else if (analogRead(X_pin) < 100) {
      return 0;
    }
  } else {
    if (analogRead(Y_pin) > 600) {
      return 2;
    } else if (analogRead(Y_pin) < 200) {
      return 0;
    }
  }
  return 1;
}
void ChangeMode(int ThisMode) {
  SubMode = true;
  Mode = ThisMode;
  TillSaveData = 0;
}

int Graf(int Max, int RawInPut, int i) {
  display.drawLine(i - 5, 45, i - 5, 45, 0);
  i += 1;
  if (i >= 85) {
    display.drawLine(85, 45, 80, 45, 0);
    i = 0;
  }
  int y = map(RawInPut, 0, Max, 0, 43);

  display.drawLine(i, 0, i, 50, 0); //if doesnt work: 50 ---> 43
  display.drawLine(i, 0, i, int(y), 1);
  display.drawLine(i, 45, i, 45, 1);
  return (i);
}
