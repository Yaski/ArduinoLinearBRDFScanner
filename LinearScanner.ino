#include <SPI.h>
#include <LiquidCrystal-SPI.h>

#define BUTTONS  A0

#define B_SENSOR1  A4
#define B_SENSOR2  A5

#define LCD_UPDATE_DELAY 100

#define WORK_MAX_POS 10
#define WORK_MAX_STEP_SIZE 200

struct Button 
{
  int minValue;
  int maxValue;
  
  bool wasDown;
  
  bool pressed;
  
  Button(int min, int max) : minValue(min), maxValue(max), wasDown(false), pressed(false) { }
};

struct BlockingSensor
{
  int pin;
  
  int value;
  
  BlockingSensor(int pinIndex) : pin(pinIndex), value(0) { }
};

struct Settings
{
  int startL = 0;
//  int startR = 0;
  int endL = 0;
//  int endR = 0;
  int stepSize = 10;
};

struct Screen
{
  unsigned long refreshTimer;
    
};
LiquidCrystal lcd(10);

Screen screen;

Button button1(0, 20);
Button button2(400, 580);
Button button3(660, 700);
Button button4(750, 800);

//BlockingSensor sensor1(B_SENSOR1);
//BlockingSensor sensor2(B_SENSOR2);

Settings settings;

void setupSensors()
{
  // TODO: setup sensors
}

void updateSensors()
{
  int value = getButtonsState(BUTTONS);
  
  bool down;
  down = value >= button1.minValue && value <= button1.maxValue;
  button1.pressed = down && !button1.wasDown;
  button1.wasDown = down;
  
  down = value >= button2.minValue && value <= button2.maxValue;
  button2.pressed = down && !button2.wasDown;
  button2.wasDown = down;

  down = value >= button3.minValue && value <= button3.maxValue;
  button3.pressed = down && !button3.wasDown;
  button3.wasDown = down;

  down = value >= button4.minValue && value <= button4.maxValue;
  button4.pressed = down && !button4.wasDown;
  button4.wasDown = down;
  
  // TODO: get sensors value
}

enum Stage {
 SETUP = 0,
 GOTO_BEGIN1 = 1,
 SCANNING1 = 2, 
 GOTO_BEGIN2 = 3,
 SCANNING2 = 4,
 FINISHED = 5,
 PAUSE = 10
};

Stage currentStage = static_cast<Stage>(0);
int   currentSelected = 0;

void setup() {
  setupSensors();

  lcd.begin(16, 2);
  
//  Serial.begin(9600);
}

void loop() {
  updateSensors();
  
  switch (currentStage)
  {
    case SETUP:
      StageSetup();
      break;
    case GOTO_BEGIN1:
      StageGoToBegin();
      break;
    case SCANNING1:
      StageScanning();
      break;
    case FINISHED:
      StageFinished();
      break;
    case PAUSE:
      StagePause();
      break;
  }
//  updateScreen();
}

long value1 = 100000;
long value2 = 3;

int readSensor(int pin)
{
  return value1 > 0 ? 1000 : 30;
}

bool MoveToInitial()
{
  // Read blocking sensors
  int sensor1 = readSensor(B_SENSOR1);
  int sensor2 = readSensor(B_SENSOR2);
  if (sensor1 < 100 && sensor2 < 100)
  {
    return true;
  }
  // move engines
  if (sensor1 >= 100)
  {
    value1 -= 1;
  }
  if (sensor2 >= 100)
  {
    value2 -= 1;
  }
  return false;
}

void StageSetup()
{
  bool initialized = MoveToInitial();

  if (button1.pressed)
  {
//    Serial.println("button1");
    switch (currentSelected)
    {
      case 0:
        settings.startL = constrain(settings.startL - 1, 0, WORK_MAX_POS);
        break;
//      case 1:
//        settings.startR = constrain(settings.startR - 1, 0, WORK_MAX_POS);
//        break;
      case 1:
        settings.endL = constrain(settings.endL - 1, settings.startL, WORK_MAX_POS);
        break;
//      case 3:
//        settings.endR = constrain(settings.endR - 1, 0, WORK_MAX_POS);
//        break;
      case 2:
        settings.stepSize = constrain(settings.stepSize - 5, 5, WORK_MAX_STEP_SIZE);
        break;
    }
  }
  if (button2.pressed)
  {
//    Serial.println("button2");
    switch (currentSelected)
    {
      case 0:
        settings.startL = constrain(settings.startL + 1, 0, WORK_MAX_POS);
        break;
//      case 1:
//        settings.startR = constrain(settings.startR + 1, 0, WORK_MAX_POS);
//        break;
      case 1:
        settings.endL = constrain(settings.endL + 1, settings.startL, WORK_MAX_POS);
        break;
//      case 3:
//        settings.endR = constrain(settings.endR + 1, 0, WORK_MAX_POS);
//        break;
      case 2:
        settings.stepSize = constrain(settings.stepSize + 5, 5, WORK_MAX_STEP_SIZE);
        break;
    }
  }
  if (button3.pressed)
  {
//    Serial.println("button3");
    if (currentSelected == 2)
    {
      clearScreen();
    }
    currentSelected = constrain(currentSelected - 1, 0, 1000);
  }
  if (button4.pressed)
  {
//    Serial.println("button4");
    if (currentSelected == 0)
    {
      settings.endL = constrain(settings.endL, settings.startL, WORK_MAX_POS);
    }
    if (currentSelected == 1)
    {
      clearScreen();
    }
    if (!initialized)
    {
      currentSelected = constrain(currentSelected + 1, 0, 2);
    }
    else
    {
      currentSelected = constrain(currentSelected + 1, 0, 4);
      if (currentSelected == 4)
      {
        currentSelected = 0;
        clearScreen();
        currentStage = GOTO_BEGIN1;
        return;
      }
    }
  }
  if (isScreenReady())
  {
//    Serial.println(currentSelected);
     if (currentSelected < 2)
     {
        lcd.setCursor(1, 0);
        lcd.print("Start L");  
        lcd.setCursor(1, 1);
        lcd.print("End   L");
        lcd.setCursor(12, 0);
        lcd.print("R");
        lcd.setCursor(12, 1);
        lcd.print("R");
        lcd.setCursor(8, 0);
        printFixed2(settings.startL);
        lcd.setCursor(13, 0);
        printFixed2(settings.startL);
        lcd.setCursor(8, 1);
        printFixed2(settings.endL);
        lcd.setCursor(13, 1);
        printFixed2(settings.endL);
        if (currentSelected == 0)
        {
          lcd.setCursor(8, 0);
        }
        if (currentSelected == 1)
        {
          lcd.setCursor(8, 1);
        }
        lcd.cursor();
     }
     else
     {
        lcd.setCursor(1, 0);
        lcd.print("Step");
        lcd.setCursor(3, 1);
        if (initialized)
        {
          lcd.print("   SCAN   ");
        }
        else
        {
          lcd.print("...WAIT...");
        }
        lcd.setCursor(8, 0);
        printFixed3(settings.stepSize);

        if (currentSelected == 2)
        {
          lcd.setCursor(8, 0);
        }
        if (currentSelected == 3)
        {
          lcd.setCursor(6, 1);
        }
        lcd.cursor();
     }
  }
}

long e1;
long e2;

bool updateEngine()
{
  if (e1 <= 0)
  {
    return true;
  }
  e1--;
  return false;
}

void StageGoToBegin()
{
  if (currentSelected == 0)
  {
    // set engines target positions
    lcd.setCursor(1, 0);
    lcd.print("STARTING...");
    currentSelected = 1;
    e1 = 100000;
  }
  if (updateEngine())
  {
    clearScreen();
    currentSelected = 0;
    currentStage = SCANNING1;
    return;
  }
}

void StageScanning()
{
  if (currentSelected == 0)
  {
    e1 = 0;
    e2 = 10;
    currentSelected = 1;    
  }
  if (currentSelected == 1)
  {
    lcd.setCursor(1, 0);
    lcd.print("SCANNING...");
    currentSelected = 2;
  }
  if (updateEngine())
  {
    if (e2 <= 0)
    {
      currentStage = FINISHED;
      currentSelected = 0;
      clearScreen();
      return;
    }
    // makePhoto
    e2--;
    e1 = 10000;
  }
  if (button4.pressed)
  {
    currentStage = PAUSE;
    currentSelected = 0;
    clearScreen();
  }
}

void StageFinished()
{
  if (currentSelected == 0)
  {
    lcd.setCursor(4, 1);
    lcd.print("FINISHED");
    currentSelected = 1;
  }
  if (button4.pressed)
  {
    currentStage = SETUP;
    currentSelected = 0;
    clearScreen();
  }
}

void StagePause()
{
  if (button1.pressed)
  {
    currentSelected = constrain(currentSelected - 1, 0, 1);
  }
  if (button2.pressed)
  {
    currentSelected = constrain(currentSelected + 1, 0, 1);
  }
  if (button4.pressed)
  {
    if (currentSelected == 0)
    {
      currentStage = SCANNING1;
      currentSelected = 1;
      clearScreen();
      return;
    }
    if (currentSelected == 1)
    {
      currentStage = SETUP;
      currentSelected = 0;
      clearScreen();
      return;
    }
  }
  if (isScreenReady())
  {
    lcd.setCursor(4, 0);
    lcd.print("Continue");
    lcd.setCursor(4, 1);
    lcd.print(" Finish ");
    
    if (currentSelected == 0)
    {
      lcd.setCursor(4, 0);
      lcd.cursor();
    }
    if (currentSelected == 1)
    {
      lcd.setCursor(5, 1);
      lcd.cursor();
    }
  }
}

void printFixed2(int value)
{
  if (value < 10)
  {
    lcd.print(" ");
  }
  lcd.print(value);
}

void printFixed3(int value)
{
  if (value < 10)
  {
    lcd.print("  ");
  }
  else if (value < 100)
  {
    lcd.print(" ");
  }
  lcd.print(value);
}

void clearScreen()
{
  screen.refreshTimer = 0;
  lcd.clear();
  lcd.noBlink();
  lcd.noCursor();
}

bool isScreenReady()
{
  unsigned long time = millis();
  if ((time - screen.refreshTimer) >= LCD_UPDATE_DELAY)
  {
    screen.refreshTimer = time;
    return true;
  }
  return false;
}

int getButtonsState(int pin)
{
  int value = analogRead(pin);
 
  if (value <= 900) {
    delay(10);
    
    return analogRead(pin);
  }
  return value;
}

