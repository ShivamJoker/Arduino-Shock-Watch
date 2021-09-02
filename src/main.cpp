#include <Arduino.h>
#include <U8g2lib.h>

// JC_Button - Version: Latest
#include <JC_Button.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

//Set the time of now
unsigned int hour = 10;
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
Button ModeBtn(2);
Button IncreaseBtn(3);

// Shock pin and buzzer pin

#define buzzerPin 11
#define shockPin 10

// Battery font
const uint8_t battery19[185] U8G2_FONT_SECTION("battery19") = 
  "\10\0\3\3\4\5\1\1\5\10\23\0\0\23\0\23\0\0\0\0\0\0\234\60\13\70\317\42\35\304\372\377"
  "\371 \61\17\70\317\42\35\304\372\177\241H(\342\203\0\62\23\70\317\42\35\304\372/\24\11E,\241H"
  "(\342\203\0\63\27\70\317\42\35\304\372\205\42\241\210%\24\11E,\241H(\342\203\0\64\33\70\317\42"
  "\35\304\272P$\24\261\204\42\241\210%\24\11E,\241H(\342\203\0\65\37\70\317\42\35\304\22\212\204"
  "\42\226P$\24\261\204\42\241\210%\24\11E,\241H(\342\203\0\66\27\70\317\42\35\304\332B\262\220"
  "^(\22\212H\247\230(&V>\10\67\5\0\316\0\0\0\0\4\377\377\0";

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
  display.setFont(u8g2_font_inb16_mf);
  display.setFontMode(0);
  display.setDrawColor(1);
  display.setCursor(10, 0);
  display.print(Al_time);

  display.setFont(u8g2_font_inb16_mf);

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
  display.setFont(u8g2_font_8x13_tf);
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

  display.clearBuffer();
  display.setFont(u8g2_font_VCR_OSD_tu);

  uint8_t fontHeight = 18;
  if (hour < 10)
  {
    display.setCursor(5, fontHeight);
  }
  else
  {
    display.setCursor(0, fontHeight);
  }
  display.print(time);
  display.setCursor(100, fontHeight);
  display.print(AMorPM);
  if (settingsMode)
  {
    // openSettings();
  }
  if (settingsMode)
  {
    if (editHH)
    {
      display.setCursor(1, fontHeight + 2);
    }
    else if (editMM)
    {
      display.setCursor(40, fontHeight + 2);
    }
    else if (editAmPm)
    {
      display.setCursor(100, fontHeight + 2);
    }
    display.setFont(u8g2_font_8x13_tf);
    display.print("__");
    display.setCursor(35, 32);
    display.print("Set Time");
    display.print("__");
  }
  display.setFont(battery19);
  // Battery Full
  display.drawGlyph(0, 0, 0x0035);
  if (!settingsMode)
  {
    if (ledVoltage > 3.5)
    {
      // Battery Charging
      display.drawGlyph(5, 32, 0x0036);
    }
    else if (batteryVoltage > 4)
    {
      // Battery Full
      display.drawGlyph(5, 32, 0x0035);
    }
    else if (batteryVoltage < 3.7)
    {
      // Battery Down
      display.drawGlyph(5, 32, 0x0030);
    }
  }

  display.updateDisplay();
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
  display.setFont(u8g2_font_inb16_mf);
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
    display.setPowerSave(0);
    DpreviousMillis = DcurrentMillis;
  }

  // turn off oled after 5 seconds
  if (DcurrentMillis - DpreviousMillis >= Dinterval && oledOn && !settingsMode)
  {
    DpreviousMillis = DcurrentMillis;
    display.setPowerSave(1);
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
