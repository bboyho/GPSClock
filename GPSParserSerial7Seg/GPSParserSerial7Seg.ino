/*
  GPSParserSerial7Seg.ino
  By: Ho Yun "Bobby" Chan
  Updated April, 11th 2017
  September 27th, 2015

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

int time_hour = 0;
int time_minute = 0;
int time_second = 0;

int time_hourTemp = 0;
int time_minuteTemp = 0;
int time_secondTemp = 0;

byte zero = 0;//can't compile and use Serial.write(0x00);, must use Serial.write(zero);

boolean MDT = true; //Mountain Daylight Time, for Daylight Savings Time
boolean AM = true; //AM or PM?

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

  pinMode(13, OUTPUT);

}

void loop() {
  while (uart_gps.available()) { //while there is data on the RX pin
    int c = uart_gps.read(); //load the data into a variable...
    if (gps.encode(c)) { //if there is a new valid sentence...
      getgps(gps);//then grab the data
    }
  }
}

void getgps(TinyGPSPlus &gps) {
  //Note: time obtained is Universal Time Coordinated (UTC)/Greenwich Mean Time (GMT)
  //Mountain Standard Time = 7 hours behind UTC
  //Mountain Daylight Time = 6 hours behind UTC during Daylight Savings Time
  Serial.println("HH:MM:SS");
  time_hour = gps.time.hour(); //get raw hour (0-23) (u8)
  //adjust time for Mountain!
  //--------------------Mountain Standard Time --------------------
  if (MDT == 0) { //Mountain Standard Time

    //UTC=>Mountain Time Conversion
    //7am-12pm=>12am-5am
    //12pm-18pm=>5am-11pm

    //19pm-0am=>12pm-5pm
    //0am-6am=>5pm-11am

    if (time_hour >= 7 && time_hour < 19) { //in the 12AM-11AM
      time_hour = time_hour - 7;
      AM = true;//set for AM
    }

    else { //time_hour<6 && time_hour>=19 therefore, in the 12PM-11PM
      if (time_hour < 6) {
        time_hour = time_hour + 12 - 7; //adjust so hour is not negative
      }
      else if (time_hour >= 19) {
        time_hour = time_hour - 7;
      }
      AM = false;//set for PM
    }
  }
  //-------------------- End Mountain Standard Time --------------------

  //-------------------- Mountain Daylight Time --------------------
  else { //Mountain Daylight Time
    if (time_hour < 12) {
      time_hour = time_hour + 12 - 6;
      AM = false;//set for PM
    }
    else if (time_hour > 12) {
      time_hour = time_hour - 6;
    }
  }

  //--------------------End Mountain Daylight Time --------------------

  //-------------------- Output to UART pins --------------------
  Serial.print(time_hour);
  Serial.print(":");
  time_minute = gps.time.minute();//get raw minute (0-59) (u8)
  Serial.print(time_minute);
  Serial.print(":");
  time_second = gps.time.second(); //get raw sec (0-59) (u8)
  Serial.print(time_second);

  if (AM == true) {
    digitalWrite(13, LOW); //it's AM

    Serial.println(" AM");

  }
  else {
    digitalWrite(13, HIGH); //it's PM
    Serial.println(" PM");
  }
  //--------------------End Output to UART pins --------------------
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
      s7s.write(0x01);
      s7s.print(time_hour);
    }
  }
  else { //hour the same, do nothing
  }
  if (time_minuteTemp != time_minute) {
    time_minuteTemp = time_minute;
    if (time_minuteTemp >= 10) {
      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x02);
      s7s.print(time_minute);
    }
    else { //minutes<10
      s7s.write(0x79);//command byte for cursor control
      s7s.write(0x02);
      s7s.print(0);
      s7s.print(time_minute);
    }
  }
  else { //minutes the same, do nothing
  }

  s7s.end();

  uart_gps.begin(GPSBAUD);//turn back on
}
