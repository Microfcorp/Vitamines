#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClock.h>
#include <EEPROM.h>

const int PinSignal[] = {PA7, PB0};
const int Pad[] = {PA0, PA1, PB5};

int statusbut = 1; // 1 - вверх, 2 - вниз, 3 - влево, 4 - вправо, 5 - селект, 6 - влево и селект, 7 - вправо и селект

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);
RTClock rt (RTCSEL_LSE); // initialise

byte Verh[] = {
  0x00,
  0x04,
  0x0E,
  0x15,
  0x04,
  0x04,
  0x04,
  0x00
};

byte Vniz[] = {
  0x00,
  0x04,
  0x04,
  0x04,
  0x15,
  0x0E,
  0x04,
  0x00
};

byte OK[] = {
  0x00,
  0x00,
  0x04,
  0x02,
  0x1F,
  0x02,
  0x04,
  0x00
};

byte TimerOne[] = {
  0x04,
  0x0E,
  0x15,
  0x15,
  0x15,
  0x15,
  0x0E,
  0x04
};
byte TimerTwo[] = {
  0x04,
  0x0A,
  0x11,
  0x1F,
  0x1F,
  0x11,
  0x0A,
  0x04
};

String Menus[] = {
  "Exit",
  "Times 1",
  "Times 2"
};

int Timers1[] = {0, 10}; // star, kazd
int Timers2[] = {0, 20}; // star, kasd

int statuss = 0;
int curmenus = 0;

bool sig[] = {false,false};

void setup()
{
  // initialize the LCD
  lcd.begin();
  rt.attachSecondsInterrupt(Whiles);// Call blink
  Serial.begin(112500);
  
  pinMode(PinSignal[0], OUTPUT);
  pinMode(PinSignal[1], OUTPUT);
  
  pinMode(Pad[0], INPUT_ANALOG);
  pinMode(Pad[1], INPUT_ANALOG);
  pinMode(Pad[2], INPUT_PULLUP);
  Timers1[0] = GetTimes();
  Timers2[0] = GetTimes();

  Timers1[1] = EEPROM.read(0x10);
  Timers2[1] = EEPROM.read(0x20); 

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello, world!");
  lcd.createChar(0, Verh);
  lcd.createChar(1, Vniz);
  lcd.createChar(2, OK);
  lcd.createChar(3, TimerOne);
  lcd.createChar(4, TimerTwo);
  lcd.home();
  //lcd.write(2);
}

void loop()
{ 
  if (statuss == 0) { //main
    if (GetStatus() == 5) {
      SetStatus();
    }
    lcd.clear();
    lcd.home();
    lcd.setCursor(0, 0);
    lcd.print("Time " + ParseTime(GetTimes()));
    lcd.setCursor(0, 1);
    lcd.write(3);
    lcd.print(" " + (String)OstToTimers(Timers1));
    lcd.setCursor(8, 1);
    lcd.write(4);
    lcd.print(" " + (String)OstToTimers(Timers2));    
  }
  else if (statuss == 1 & !sig[0] & !sig[1]) { //settings
    lcd.clear();
    lcd.home();
    lcd.setCursor(0, 0);
    lcd.write(2);
    lcd.print(Menus[curmenus]);
    if(curmenus == 1){    
      lcd.setCursor(0, 1);
      lcd.print(ParseTime(Timers1[1]));
      if (GetStatus() == 4) {
        Timers1[1]++;
        EEPROM.write(0x10, ((float)Timers1[1]));       
      }
      else if (GetStatus() == 3) {
        Timers1[1]--;
        EEPROM.write(0x10, ((float)Timers1[1]));
      }
      else if (GetStatus() == 5) {
        Timers1[1] = Timers1[1] + 3600;
        EEPROM.write(0x10, ((float)Timers1[1]));
      }
      else if (GetStatus() == 6) {
        Timers1[1] = Timers1[1] - 3600;
        EEPROM.write(0x10, ((float)Timers1[1]));
      }
    }
    if(curmenus == 2){    
      lcd.setCursor(0, 1);
      lcd.print(ParseTime(Timers2[1]));
      if (GetStatus() == 4) {
        Timers2[1]++;
        EEPROM.write(0x20, ((float)Timers2[1]));
      }
      else if (GetStatus() == 3) {
        Timers2[1]--;
        EEPROM.write(0x20, ((float)Timers2[1]));
      }
      else if (GetStatus() == 5) {
        Timers2[1] = Timers2[1] + 3600;
        EEPROM.write(0x20, ((float)Timers2[1]));
      }
      else if (GetStatus() == 6) {
        Timers2[1] = Timers2[1] - 3600;
        EEPROM.write(0x20, ((float)Timers2[1]));
      }
    }
    if (GetStatus() == 2) {
      SetCurmenus();
      delay(200);
    }
    if (GetStatus() == 5) {
      switch (curmenus) {
        case 0:
          statuss = 0;
          break;
      }
    }
  }
  else{
    sig[0] = false;
    sig[1] = false;
    digitalWrite(PinSignal[1], LOW);
    digitalWrite(PinSignal[2], LOW);
    delay(200);
  }

  Serial.println("Test");
  Serial.println(GetStatus());
  delay(600);
}

void Whiles(){
  if(IsTimer(Timers1)){
      digitalWrite(PinSignal[1], HIGH);
      sig[0] = true;
  }
  if(IsTimer(Timers2)){
      digitalWrite(PinSignal[2], HIGH);
      sig[1] = true;
  }
}

void SetStatus() {
  statuss++;
  if (statuss > 1) {
    statuss = 0;
  }
}
void SetCurmenus() {
  curmenus++;
  if (curmenus > 2) {
    curmenus = 0;
  }
}

int GetStatus() {
  int Y = analogRead(Pad[0]);
  int X = analogRead(Pad[1]);

  //4085
  if (Y > 4085) {
    return 1;
  }
  else if (Y < 10) {
    return 2;
  }
  else if (X > 4085) {
    return 4;
  }
  else if (X > 4085 & digitalRead(Pad[2]) == LOW) {
    return 7;
  }
  else if (X < 10) {
    return 3;
  }
  else if (X < 10 & digitalRead(Pad[2]) == LOW) {
    return 6;
  }
  else if (digitalRead(Pad[2]) == LOW) {
    return 5;
  }
  else {
    return 0;
  }
}


