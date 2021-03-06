/*
  GPSParserSerial7Seg.ino
  By: Ho Yun "Bobby" Chan
  Updated April, 15th 2017
  September 27th, 2015
  //last upload to project w/ Arduino.cc's Arduino IDE v1.6.8

  Description: This code parses the GPS data when there is a satellite lock
  and outputs the data to a 7-segment serial display using
  software serial pins. The clock is adjust for Mountain Standard
  Time with an optional mode for daylight savings.*/


#include <TinyGPS++.h> //GPS parser from arduiniana.org
#include <SoftwareSerial.h>

#define RXPIN 2
#define TXPIN 3
SoftwareSerial uart_gps(RXPIN, TXPIN); //Rx = 2, Tx = 3
SoftwareSerial s7s(8, 7); //rx = 8, tx =7

#define GPSBAUD 4800 //baud rate of GPS
TinyGPSPlus gps; //create instance of TinyGPS object

void getgps(TinyGPSPlus &gps);

//used to convert GPS UTC time to Mountain Time
int time_hour = 0;
int time_minute = 0;
int time_second = 0;

//for 7 segment display
int time_hourTemp = 0;
int time_minuteTemp = 0;
int time_secondTemp = 0;

byte zero = 0;//can't compile and use Serial.write(0x00);, must use Serial.write(zero);

boolean MDT = true; //Mountain Daylight Time == false, for Daylight Savings Time == true
//MDT button
const int buttonMDTPin = 5; //MDT button
const int MDTLEDPin = 4;    //yellow LED
int buttonMDTState = HIGH; //set MDT button HIGH, so not pressing
//keep track if MDT  button press when held down
boolean prev_buttonMDTState = false;
boolean current_buttonMDTState = false;

boolean AM = false; //AM or PM?
const int AM_statusLED = 13;

void setup() {
  Serial.begin(115200);

  s7s.begin(9600);
  s7s.write(0x76);  // Clear display command
  s7s.write(0x77);//send command byte to turn on colon
  s7s.write(0b010000);//turn on colon
  s7s.print("-HI-");
  delay(500);
  s7s.write(0x76);  // Clear display command
  s7s.write(0x77);//send command byte to turn on colon
  s7s.write(0b010000);//turn on colon

  s7s.end();

  uart_gps.begin(GPSBAUD);

  Serial.println("");
  Serial.println("GPS Parser Initializing");
  Serial.println(" ...waiting for lock...");
  Serial.println("");

  pinMode(AM_statusLED, OUTPUT); // PM indicator

  pinMode(buttonMDTPin, INPUT_PULLUP); //use internal pullup resistor to MDT button
  pinMode(MDTLEDPin, OUTPUT);//set MDT LED
  digitalWrite(MDTLEDPin, HIGH);//turn LED ON
}

void loop() {
  while (uart_gps.available()) { //while there is data on the RX pin
    buttonMDTState = digitalRead(buttonMDTPin);//check state of MDT button

    //----------MDT  Mode----------
    //if MDT button pressed, change MDT

    if (buttonMDTState == LOW) {
      current_buttonMDTState = true; //button has been pressed
      if (prev_buttonMDTState != current_buttonMDTState) {

        if (MDT == true) {//change to Mountain Standard Time
          digitalWrite(MDTLEDPin, LOW);//turn LED OFF
          MDT = false;
        }
        else {//MDT == false, change back to Mount Daylight Time
          digitalWrite(MDTLEDPin, HIGH);
          MDT = true;
        }
      }
      prev_buttonMDTState = current_buttonMDTState;//update MDT button's state
    }
    else if (buttonMDTState == HIGH) {
      current_buttonMDTState = false;//button has not been pressed
      if (prev_buttonMDTState != current_buttonMDTState) {
        //do nothing here when button is not pressed

      }
      prev_buttonMDTState = current_buttonMDTState;//update MDT button's state
    }

    //update GPS time
    int c = uart_gps.read(); //load the data into a variable...
    if (gps.encode(c)) { //if there is a new valid sentence...
      getgps(gps);//then grab the data
    }
  }
}

void getgps(TinyGPSPlus &gps) {
  Serial.println("HH:MM:SS");
  time_hour = gps.time.hour(); //get raw hour (0-23) (u8)
  //Note: Time obtained is Universal Time Coordinated (UTC)/Greenwich Mean Time (GMT)
  //adjust time for mountain! below!
  //--------------------Mountain Daylight Time --------------------
  //UTC ahead 6 hrs Mountain Daylight Time so we need to subtact 6 to convert
  //UTC-6 => Mountain Time Conversion
  //AM
  //06 to 11 and 12 to 18<=00 to 06 and 07 to 12
  //PM
  //18 to 23 and 00 to 07<=12 to 16 and 17 to 00

  //in the AM, we will not have negative numbers going back 6 hrs
  if (MDT == true) {
    //12:00am to 12:59am
    if (time_hour == 6) {
      time_hour = time_hour - 6 + 12;
      AM = true;// set for AM
    }
    //1:00am to 11:59am
    else if (time_hour >= 7 && time_hour < 18) {
      time_hour = time_hour - 6;
      AM = true;//set for AM
    }
    //in the PM
    //12:00pm to 12:59pm
    else if (time_hour == 18) {
      time_hour = time_hour - 6;
      AM = false;//set for PM
    }
    //1:00pm to 5:59pm
    else if (time_hour > 18 && time_hour <= 23) {
      time_hour = time_hour - 6 - 12;
      AM = false;//set for PM
    }
    //6:00pm to 11:59pm
    else if (time_hour >= 00 && time_hour < 6) { //never reaches 24, it goes back to 00
      time_hour = time_hour + 12 - 6; //adjust so hour is not negative
      AM = false;//set for PM
    }
  }
  //-------------------- End Mountain Daylight Time --------------------

  //-------------------- Mountain Savings Time --------------------
  //UTC ahead 7 hrs Mountain Daylight Time so we need to subtact 7 to convert
  //UTC-7 => Mountain Time Conversion

  //AM
  //07 to 11 and 12 to 19<=00 to 06 and 07 to 12
  //PM
  //19 to 23 and 00 to 07<=12 to 16 and 17 to 00

  //in the AM, we will not have negative numbers going back 7 hrs

  else { //MDT == false
    //12:00am to 12:59am
    if (time_hour == 7) {
      time_hour = time_hour - 7 + 12;
      AM = true;// set for AM
    }
    //1:00am to 11:59am
    else if (time_hour >= 8 && time_hour < 19) {
      time_hour = time_hour - 7;
      AM = true;//set for AM
    }
    //in the PM
    //12:00pm to 12:59pm
    else if (time_hour == 19) {
      time_hour = time_hour - 7;
      AM = false;//set for PM
    }
    //1:00pm to 4:59pm
    else if (time_hour > 19 && time_hour <= 23) {
      time_hour = time_hour - 7 - 12;
      AM = false;//set for PM
    }
    //5:00pm to 11:59pm
    else if (time_hour >= 00 && time_hour < 7) { //never reaches 24, it goes back to 00
      time_hour = time_hour + 12 - 7; //adjust so hour is not negative
      AM = false;//set for PM
    }
  }
  //--------------------End Mountain Savings Time --------------------

  //-------------------- Output to Serial UART for Debugging --------------------
  if (MDT == true) {
    Serial.println("Mountain Daylight Time");
    Serial.println("Daylight savings ended so spring forward an hour");
  }
  else {
    Serial.print("Mountain Standard Time");
    Serial.println("Daylight savings begins so fall back an hour!");
  }

  Serial.print(time_hour);
  Serial.print(":");
  time_minute = gps.time.minute();//get raw minute (0-59) (u8)
  Serial.print(time_minute);
  Serial.print(":");
  time_second = gps.time.second(); //get raw sec (0-59) (u8)
  Serial.print(time_second);

  if (AM == true) {
    digitalWrite(AM_statusLED, LOW); //it's AM
    Serial.println(" AM");
    Serial.println();
  }
  else {
    digitalWrite(AM_statusLED, HIGH); //it's PM
    Serial.println(" PM");
    Serial.println();
  }

  //--------------------End Output to Serial UART for Debugging --------------------
  uart_gps.end();//temporarily turn of gps UART, can only have one at a time

  s7s.begin(9600);
  if (time_hourTemp != time_hour) {
    time_hourTemp = time_hour;
    if (time_hour >= 10) {
      s7s.write(0x79);//command byte for cursor control
      s7s.write(zero);
      s7s.print(time_hour);
    }
    else { //hour<10
      s7s.write(0x79);//command byte for cursor control
      s7s.write(zero);
      
      s7s.write(0x7B);//turn off if the hour switched from 12 to 1
      s7s.write(zero);
      
      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x01);
      s7s.print(time_hour);
    }
  }
  else { //hour the same, do nothing
  }

  if (time_minuteTemp != time_minute) {
    time_minuteTemp = time_minute;

    if (time_minuteTemp >= 10) {//between 10 m to 59 min
      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x02);
      s7s.print(time_minute);
    }
    else if (time_minuteTemp > 0 && time_minuteTemp < 10) {//between 1 min to 9 min
      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x02);
      s7s.print(0);//place 0 in position if individual digit
      s7s.print(time_minute);
    }
    else if (time_minuteTemp == 0) {//0 min
      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x02);
      s7s.print(0);//place 0 in position if individual digit

      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x03);
      s7s.print(0);//place 0 in position if individual digit
    }
  }
  else {
    //minutes the same, do nothing here
  }

  s7s.end();//turn of software serial for 7-segment serial display

  uart_gps.begin(GPSBAUD);//turn back on
}
