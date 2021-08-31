#include <Arduino.h>
#include <U8g2lib.h>

// JC_Button - Version: Latest
#include <JC_Button.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

//Set the time of now
unsigned int hour = 8;
unsigned int minute = 2;
unsigned int second = 50;

String AMorPM = "PM";

//Set the alaram variables
unsigned int Al_hour = 8;
unsigned int Al_minute = 1;

String Al_AMorPM = "PM";

// Editing mode
bool editHH;
bool editMM;
bool editAmPm;

bool settingsMode;
bool alarmMode;
bool shockMode;
bool editShockP;
bool editShockT;
bool oledOn = true;

unsigned int modeBtnCount = 1;
bool modeBtnPressed = false;

// Declare for how much time u want shock in seconds
unsigned int shockTime = 20;

float batteryVoltage;
float ledVoltage;

//use millis to increase second
unsigned long TpreviousMillis = 0;
unsigned long DpreviousMillis = 0;

const long Tinterval = 983;

const long Dinterval = 10000;
// adjust according to your hardware

// Button Pins

Button DecreaseBtn(5);
Button ModeBtn(6);
Button IncreaseBtn(7);

// Shock pin and buzzer pin

#define buzzerPin 11
#define shockPin 10

// Battery bitmaps

// Battery charging
const unsigned char BatteryCharging[] PROGMEM = {
    0x7f, 0xff, 0xf8, 0x80, 0x00, 0x06, 0x80, 0x00, 0x02, 0x80, 0x00, 0x06, 0x80, 0x30, 0x03, 0x86,
    0x3c, 0x03, 0x83, 0xff, 0x03, 0x80, 0xff, 0x83, 0x80, 0x30, 0x03, 0x80, 0x00, 0x06, 0x80, 0x00,
    0x02, 0x80, 0x00, 0x06, 0x7f, 0xff, 0xfc};

// 'battery full', 24x13px
const unsigned char BatteryFull[] PROGMEM = {
    0x7f, 0xff, 0xf8, 0xf9, 0xef, 0x3c, 0xf9, 0xef, 0x3c, 0xf9, 0xef, 0x3c, 0xf9, 0xef, 0x3f, 0xf9,
    0xef, 0x3f, 0xf9, 0xef, 0x3f, 0xf9, 0xef, 0x3f, 0xf9, 0xef, 0x3f, 0xf9, 0xef, 0x3c, 0xf9, 0xef,
    0x3c, 0xf9, 0xef, 0x3c, 0x7f, 0xff, 0xf8};

// 'battery down', 24x13px
const unsigned char BatteryDown[] PROGMEM = {
    0x7f, 0xff, 0xf8, 0x80, 0x00, 0x06, 0x80, 0x00, 0x02, 0x80, 0x00, 0x06, 0x80, 0x00, 0x03, 0x80,
    0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x06, 0x80, 0x00,
    0x02, 0x80, 0x00, 0x06, 0x7f, 0xff, 0xfc};

void setup()
{

  Serial.begin(115200);
  display.begin();

  // Initialize the buttons

  DecreaseBtn.begin();
  ModeBtn.begin();
  IncreaseBtn.begin();

  // Set buzzer pin and shock pin mode to output
  pinMode(buzzerPin, OUTPUT);
  pinMode(shockPin, OUTPUT);
}

// alaram function

void setAlaram()
{
  if (Al_minute > 60)
  {
    Al_minute = 59;
  }
  if (Al_minute > 59)
  {
    Al_minute = 0;
  }

  if (Al_hour > 12)
  {
    Al_hour = 1;
  }

  if (Al_hour < 1)
  {
    hour = 12;
  }

  String Al_time = String(Al_hour) + ":" + String(Al_minute) + " " + String(Al_AMorPM);

  if (Al_minute < 10)
  {
    Al_time = String(Al_hour) + ":" + "0" + String(Al_minute) + " " + String(Al_AMorPM);
  }

  display.clearDisplay();
  display.setFont(u8g2_font_ncenB24_tf);
  display.setFontMode(0);
  display.setDrawColor(1);
  display.setCursor(10, 0);
  display.print(Al_time);

  display.setFont(u8g2_font_lubB18_tf);

  display.setCursor(25, 20);
  display.print("Set Alaram");
  if (editHH)
  {
    display.setCursor(10, 10);
  }
  else if (editMM)
  {
    display.setCursor(50, 10);
  }
  else if (editAmPm)
  {
    display.setCursor(80, 10);
  }
  display.print("__");

  display.display();
}

void openSettings()
{
  display.setFont(u8g2_font_lubB18_tf);
  display.setCursor(35, 20);
  display.print("Set Time");
}

// function to show and maintain the time

void showTime()
{
  if (minute > 60)
  {
    minute = 59;
  }
  if (minute > 59)
  {
    minute = 0;
  }

  if (hour > 12)
  {
    hour = 1;
  }

  if (hour < 1)
  {
    hour = 12;
  }

  String time = String(hour) + ":" + String(minute) + ":" + String(second);

  if (minute < 10)
  {
    time = String(hour) + ":" + "0" + String(minute) + ":" + String(second);
  }

  if (second < 10)
  {
    time = String(hour) + ":" + String(minute) + ":" + "0" + String(second);
  }

  if (second < 10 && minute < 10)
  {
    time = String(hour) + ":" + "0" + String(minute) + ":" + "0" + String(second);
  }

  display.clearDisplay();
  display.setFont(u8g2_font_ncenB24_tf);
  display.setFontMode(0);
  display.setDrawColor(1);
  if (hour < 10)
  {
    display.setCursor(5, 0);
  }
  else
  {
    display.setCursor(0, 0);
  }
  display.print(time);
  display.setCursor(100, 0);
  display.print(AMorPM);
  if (settingsMode)
  {
    openSettings();
  }
  if (settingsMode)
  {
    if (editHH)
    {
      display.setCursor(1, 10);
    }
    else if (editMM)
    {
      display.setCursor(40, 10);
    }
    else if (editAmPm)
    {
      display.setCursor(100, 10);
    }
    display.print("__");
  }
  if (!settingsMode)
  {
    if (ledVoltage > 3.5)
    {
      display.drawXBM(5, 19, 13, 24, BatteryCharging);
    }
    else if (batteryVoltage > 4)
    {
      display.drawXBM(5, 19, 13, 24, BatteryFull);
    }
    else if (batteryVoltage < 3.7)
    {
      display.drawXBM(5, 19, 13, 24, BatteryDown);
    }
  }

  display.display();
}

// increase time every second

void increaseTime()
{
  //  Store the currentMillis
  unsigned long TcurrentMillis = millis();

  if (TcurrentMillis - TpreviousMillis >= Tinterval)
  {
    // save the last time
    TpreviousMillis = TcurrentMillis;
    second++;
    if (second > 59)
    {
      second = 0;
      minute++;
      if (minute > 59)
      {
        minute = 0;
        hour++;
        if (hour == 12)
        {
          if (AMorPM == "PM")
          {
            AMorPM = "AM";
          }
          else if (AMorPM == "AM")
          {
            AMorPM = "PM";
          }
        }
      }
    }
  }
}

// define function to show shock settings
void shockSettings()
{

  if (editShockT)
  {
    if (DecreaseBtn.wasPressed())
    {
      shockTime -= 10;
    }
    if (IncreaseBtn.wasPressed())
    {
      shockTime += 10;
    }
  }

  if (shockTime > 50)
  {
    shockTime = 0;
  }

  // make shock strings
  String shockTimeStr = "Time  " + String(shockTime) + "s";

  display.clearDisplay();
  display.setFont(u8g2_font_ncenB24_tf);
  display.setFontMode(0);
  display.setDrawColor(1);
  display.setCursor(0, 0);
  display.print(shockTimeStr);
  display.display();
}

// this is the loop
void loop()
{

  increaseTime();
  // calling increaseTime every time

  //read the button states
  DecreaseBtn.read();
  ModeBtn.read();
  IncreaseBtn.read();

  // check if settings mode is on and inscrease according to button pressed
  if (settingsMode)
  {
    if (editHH)
    {

      if (DecreaseBtn.wasPressed())
      {
        hour--;
      }
      if (IncreaseBtn.wasPressed())
      {
        hour++;
      }
    }
    else if (editMM)
    {

      if (DecreaseBtn.wasPressed())
      {
        minute--;
      }
      if (IncreaseBtn.wasPressed())
      {
        minute++;
      }
    }
    else if (editAmPm)
    {

      if (DecreaseBtn.wasPressed())
      {
        if (AMorPM == "PM")
        {
          AMorPM = "AM";
        }
        else if (AMorPM = "AM")
        {
          AMorPM = "PM";
        }
      }
      if (IncreaseBtn.wasPressed())
      {
        if (AMorPM == "PM")
        {
          AMorPM = "AM";
        }
        else if (AMorPM = "AM")
        {
          AMorPM = "PM";
        }
      }
    }
  }

  if (alarmMode)
  {
    if (editHH)
    {

      if (DecreaseBtn.wasPressed())
      {
        Al_hour--;
      }
      if (IncreaseBtn.wasPressed())
      {
        Al_hour++;
      }
    }
    else if (editMM)
    {

      if (DecreaseBtn.wasPressed())
      {
        Al_minute--;
      }
      if (IncreaseBtn.wasPressed())
      {
        Al_minute++;
      }
    }
    else if (editAmPm)
    {

      if (DecreaseBtn.wasPressed())
      {
        if (Al_AMorPM == "PM")
        {
          Al_AMorPM = "AM";
        }
        else if (Al_AMorPM = "AM")
        {
          Al_AMorPM = "PM";
        }
      }
      if (IncreaseBtn.wasPressed())
      {
        if (Al_AMorPM == "PM")
        {
          Al_AMorPM = "AM";
        }
        else if (Al_AMorPM == "AM")
        {
          Al_AMorPM = "PM";
        }
      }
    }
  }

  // open settings mode if btn pressed for 1 sec

  if (ModeBtn.pressedFor(1000) && !settingsMode)
  {
    modeBtnPressed = true;
    settingsMode = true;
  }

  if (ModeBtn.wasPressed() && settingsMode)
  {
    modeBtnCount++;
  }

  // we are using switch cases to toggle between hour minute & AmPm
  if (settingsMode)
  {
    switch (modeBtnCount)
    {
    case 1:
      editHH = true;
      break;
    case 2:
      editHH = false;
      editMM = true;
      break;
    case 3:
      editMM = false;
      editAmPm = true;
      break;
    case 4:
      editAmPm = false;
      settingsMode = false;
      alarmMode = true;
      modeBtnCount = 0;
    }
  }

  if (ModeBtn.wasPressed() && alarmMode)
  {
    modeBtnCount++;
  }

  if (!alarmMode && !shockMode && oledOn)
  {
    showTime();
  }

  unsigned long DcurrentMillis = millis();

  // if any button pressed turn on the display

  if (ModeBtn.wasPressed() || IncreaseBtn.wasPressed() || DecreaseBtn.wasPressed() && !oledOn)
  {
    oledOn = true;
    DpreviousMillis = DcurrentMillis;
  }

  // turn off oled after 5 seconds
  if (DcurrentMillis - DpreviousMillis >= Dinterval && oledOn && !settingsMode)
  {
    DpreviousMillis = DcurrentMillis;
    oledOn = false;
    display.clearDisplay();
    display.display();
  };

  if (alarmMode)
  {
    switch (modeBtnCount)
    {
    case 1:
      editHH = true;
      break;
    case 2:
      editHH = false;
      editMM = true;
      break;
    case 3:
      editMM = false;
      editAmPm = true;
      break;
    case 4:
      editAmPm = false;
      settingsMode = false;
      alarmMode = false;
      shockMode = true;
      modeBtnCount = 0;
    }

    setAlaram();
  }
  if (ModeBtn.wasPressed() && shockMode)
  {
    modeBtnCount++;
  }

  if (shockMode)
  {
    switch (modeBtnCount)
    {
    case 1:
      editShockT = true;
      break;

    case 2:
      editShockT = false;
      shockMode = false;
      modeBtnCount = 1;
      break;
    }
  }

  if (shockMode)
  {
    shockSettings();
  }

  // ring the alaram if it matches with the Time
  if (Al_hour == hour && Al_minute == minute && second < 10)
  {
    digitalWrite(buzzerPin, HIGH);
  }
  else
  {
    digitalWrite(buzzerPin, LOW);
  }
  // give shock every sec after 10 sec of alaram beeping
  if (Al_hour == hour && Al_minute == minute && second < shockTime + 10 && second > 10 && second % 2 == 0)
  {
    digitalWrite(shockPin, HIGH);
  }
  else
  {
    digitalWrite(shockPin, LOW);
  }

  // reading battery voltages to show battery indicator
  // read the input on analog pin 0:
  int batteryValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // I noticed increase in voltage reading so i decreased .3
  batteryVoltage = ((batteryValue * 5.0) / 1023.0) - 0.3;
  // read the input on analog pin 0:

  // print out the value you read
  // Serial.println(batteryVoltage);

  int ledValue = analogRead(A1);
  ledVoltage = ((ledValue * 5.0) / 1023.0) - 0.3;

  // Serial.println("LED voltage here");
  // print out the value you read
  // Serial.println(ledVoltage);

  delay(50);
}
