#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
#define fadePin 3

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(fadePin, OUTPUT);

  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(0,50,10,2,15,1,18);
}

void setDS3231time(
    byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(
    byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10) {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10) {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");
    break;
  case 2:
    Serial.println("Monday");
    break;
  case 3:
    Serial.println("Tuesday");
    break;
  case 4:
    Serial.println("Wednesday");
    break;
  case 5:
    Serial.println("Thursday");
    break;
  case 6:
    Serial.println("Friday");
    break;
  case 7:
    Serial.println("Saturday");
    break;
  }
}

void loop(){
  // Set the time for full brightness.
  int wakeHour = 5;
  int wakeMinute = 45;
  int fadeMinutes = 12;  // minutes to transition to full brightness
  int offMinutes = 8;  // minutes after wake time to transition off
  
  // Determine when to begin the fade based on wake time.
  int startHour = wakeHour;
  int startMinute = wakeMinute - fadeMinutes;
  while (startMinute < 0) {
    startHour--;
    startMinute = startMinute + 60; // Borrow 60 minutes
  }
  
  Serial.print("startMinute is ");
  Serial.println(startMinute);
  Serial.print("startHour is ");
  Serial.println(startHour);

  float fadeDelay = 1000 * ((fadeMinutes * 60) / 255.0); // milliseconds between an analogWrite step
  Serial.print("fadeDelay is ");
  Serial.println(fadeDelay);
  
  
  // fetch the time
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  
  
  
  Serial.print("hour is ");
  Serial.println(hour);

  Serial.print("minute is ");
  Serial.println(minute);

  if (hour == startHour && minute == startMinute) {
    int fadeLevel = 0;
    while (fadeLevel < 255) {
      analogWrite(fadePin, fadeLevel);
      fadeLevel++;
      Serial.print("setting fadeLevel to ");
      Serial.println(fadeLevel);
      delay( (int) fadeDelay );
      Serial.print("fadeDelay is ");
      Serial.println( (int) fadeDelay );
    }
  }
 
  // Determine when to fade out. This is just a fixed fade.
  int offMinute = wakeMinute + offMinutes;
  int offHour = wakeHour;
  if (offMinute > 59) {
    offHour++;
    offMinute = offMinute - 60;
  }
  
  if (minute == offMinute && hour == offHour){
    int fadeOutLevel = 255;
    while (fadeOutLevel > 0) {
      fadeOutLevel--;
      Serial.print("setting fadeOutLevel to ");
      Serial.println(fadeOutLevel);
      analogWrite(fadePin, fadeOutLevel);
      delay(400);
    }
    Serial.println("turning off");
  }
}
