/*
 Arduino matrix_0 LED Nixie Clock

 A Nixie clock made with four Modulinos LED matrix_0

 Made by Julián Caro Linares for Arduino INC

 CC-BY-SA
*/

#include <Arduino_Modulino.h>
#include "ArduinoGraphics.h"
#include "Modulino_LED_Matrix.h"
// #include "LEDMatrixGallery.h" // This header contains predefined animations for the LED matrix_0 display.
#include "matrix_collection.h"
#include "RTC.h"
#include "Arduino_LED_Matrix.h" // Library for the Integrated Matrix of the UNO R4/UNOQ

// RTC and NTP Synch libraries
#include "RTC.h"
#include <NTPClient.h>

// Wifi Libraries
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h" 

// NTP Settings
// Offset is in seconds: e.g., UTC+1 = 3600, UTC-5 = -18000
int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);


// Modulino Matrix Objects
ModulinoLEDMatrix matrix_0;
ModulinoLEDMatrix matrix_1(0x40);
ModulinoLEDMatrix matrix_2(0x41);
ModulinoLEDMatrix matrix_3(0x42);

// Modulino Objects
ModulinoKnob knob;
ModulinoBuzzer buzzer;

// Arduino Matrix
ArduinoLEDMatrix integrated_matrix;

// Modulino knob variables
int position = 0; 
bool click = false;
int8_t direction = 0;

// Modulino buzzer variables
int frequency = 440;  // Frequency of the tone in Hz
int duration = 100;  // Duration of the tone in milliseconds


// RTC Variables
// RTCTime currentTime;
int hours = 0;
int minutes = 0;
// DayOfWeek day_of_week = DayOfWeek::THURSDAY;

// Initial time
RTCTime currentTime(7, Month::APRIL, 2026, 0, 0, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);

// Alarm time & Variables
RTCTime alarmTime(7, Month::APRIL, 2026, 8, 15, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
bool alarm_status = true;

// State Machines Variables
int state = 0;

void setup() {

  Serial.begin(115200);
  RTC.begin();

  // Modulinos Begin
  Modulino.begin();
  knob.begin();
  buzzer.begin();

  // Matrix_ Init Setup
  matrix_0.begin();
  matrix_1.begin();
  matrix_2.begin();
  matrix_3.begin();

  // Integrated Matrix
  integrated_matrix.begin();

  connectToWiFi();
  RTC.begin();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  // Get the current date and time from an NTP server and convert
  // it to UTC +2 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
  auto timeZoneOffsetHours = 2;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));



  // Time and Date Init Setup
  // currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::TUESDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(currentTime);
  matrix_show_time();

  integrated_matrix_date();

  // set_time(); // Manual time set
  
}

void loop() {

  // Transitions of the State Machine
  // TODO

  // State Machine
  switch(state){
    case 0: // Clock Mode
      // Get current time from RTC
      RTC.getTime(currentTime);

      matrix_show_time();
      integrated_matrix_date();
      check_alarm();
      delay(10000); // FIXME change this for a millis thing
    break;

    case 1: // Manual Time Set
      set_time();
      state =0;
    break;
  }

}


void integrated_matrix_date()
{

  DayOfWeek day_of_week = currentTime.getDayOfWeek();
  if(day_of_week == DayOfWeek::MONDAY){
    integrated_matrix.loadFrame(frame_MON);
  }
  else if(day_of_week == DayOfWeek::TUESDAY){
    integrated_matrix.loadFrame(frame_TUE);
  }
  else if(day_of_week == DayOfWeek::WEDNESDAY){
    integrated_matrix.loadFrame(frame_WED);
  }
  else if(day_of_week == DayOfWeek::THURSDAY){
    integrated_matrix.loadFrame(frame_THU);
  }
  else if(day_of_week == DayOfWeek::FRIDAY){
    integrated_matrix.loadFrame(frame_FRI);
  }
  else if(day_of_week == DayOfWeek::SATURDAY){
    integrated_matrix.loadFrame(frame_SAT);
  }
  else if(day_of_week == DayOfWeek::SUNDAY){
    integrated_matrix.loadFrame(frame_SUN);
  }
  delay(100);
}

void check_alarm(){
  int current_hour = currentTime.getHour();
  int alarm_hour = alarmTime.getHour();
  int current_minutes = currentTime.getMinutes();
  int alarm_minutes = alarmTime.getMinutes();

  // Alarm is ON
  if (alarm_status == true && current_hour == alarm_hour && current_minutes == alarm_minutes ){
    for(int i = 0; i<20; i++){
      buzzer.tone(494, 500);
      delay(250);
      buzzer.tone(0, 500);
      delay(250);
    }
    alarm_status = false;
  }
}

void matrix_show_time()
{

  int hour_tens = currentTime.getHour()/10;
  int hour_units = currentTime.getHour()%10;
  int minutes_tens = currentTime.getMinutes()/10;
  int minutes_units = currentTime.getMinutes()%10;

  switch (hour_tens){
    case 0:
      matrix_3.setFrame(SEG_NUM_0);
      break;
    case 1:
      matrix_3.setFrame(SEG_NUM_1);
      break;
    case 2:
      matrix_3.setFrame(SEG_NUM_2);
      break;
    case 3:
      matrix_3.setFrame(SEG_NUM_3);
      break;
    case 4:
      matrix_3.setFrame(SEG_NUM_4);
      break;
    case 5:
      matrix_3.setFrame(SEG_NUM_5);
      break;
    case 6:
      matrix_3.setFrame(SEG_NUM_6);
      break;
    case 7:
      matrix_3.setFrame(SEG_NUM_7);
      break;
    case 8:
      matrix_3.setFrame(SEG_NUM_8);
      break;
    case 9:
      matrix_3.setFrame(SEG_NUM_9);
      break;
  }

  switch (hour_units){
    case 0:
      matrix_2.setFrame(COLON_NUM_0);
      break;
    case 1:
      matrix_2.setFrame(COLON_NUM_1);
      break;
    case 2:
      matrix_2.setFrame(COLON_NUM_2);
      break;
    case 3:
      matrix_2.setFrame(COLON_NUM_3);
      break;
    case 4:
      matrix_2.setFrame(COLON_NUM_4);
      break;
    case 5:
      matrix_2.setFrame(COLON_NUM_5);
      break;
    case 6:
      matrix_2.setFrame(COLON_NUM_6);
      break;
    case 7:
      matrix_2.setFrame(COLON_NUM_7);
      break;
    case 8:
      matrix_2.setFrame(COLON_NUM_8);
      break;
    case 9:
      matrix_2.setFrame(COLON_NUM_9);
      break;
  }

  switch (minutes_tens){
    case 0:
      matrix_1.setFrame(SEG_NUM_0);
      break;
    case 1:
      matrix_1.setFrame(SEG_NUM_1);
      break;
    case 2:
      matrix_1.setFrame(SEG_NUM_2);
      break;
    case 3:
      matrix_1.setFrame(SEG_NUM_3);
      break;
    case 4:
      matrix_1.setFrame(SEG_NUM_4);
      break;
    case 5:
      matrix_1.setFrame(SEG_NUM_5);
      break;
    case 6:
      matrix_1.setFrame(SEG_NUM_6);
      break;
    case 7:
      matrix_1.setFrame(SEG_NUM_7);
      break;
    case 8:
      matrix_1.setFrame(SEG_NUM_8);
      break;
    case 9:
      matrix_1.setFrame(SEG_NUM_9);
      break;
  }

  switch (minutes_units){
    case 0:
      matrix_0.setFrame(SEG_NUM_0);
      break;
    case 1:
      matrix_0.setFrame(SEG_NUM_1);
      break;
    case 2:
      matrix_0.setFrame(SEG_NUM_2);
      break;
    case 3:
      matrix_0.setFrame(SEG_NUM_3);
      break;
    case 4:
      matrix_0.setFrame(SEG_NUM_4);
      break;
    case 5:
      matrix_0.setFrame(SEG_NUM_5);
      break;
    case 6:
      matrix_0.setFrame(SEG_NUM_6);
      break;
    case 7:
      matrix_0.setFrame(SEG_NUM_7);
      break;
    case 8:
      matrix_0.setFrame(SEG_NUM_8);
      break;
    case 9:
      matrix_0.setFrame(SEG_NUM_9);
      break;
  }

}


void set_time(){

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  // Hours tens
  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_HOUR);
    // Serial.println(hours);
    if (direction == 1 && hours < 24){
      buzzer.tone(330, duration);
      hours++;
      currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }
    else if(direction == -1 && hours > 0){
      hours--;
      currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  // Serial.println("Hours Set");
  delay(1000); // Delay to avoid the detection of the previous click of the Knob

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_MIN);
    // Serial.println(minutes);
    if (direction == 1 && minutes < 60){
      buzzer.tone(330, duration);
      minutes++;
      currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }
    else if(direction == -1 && minutes > 0){
      minutes--;
      currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  // Serial.println("Minutes Set");
  buzzer.tone(frequency, duration);

}


//---------------------- WIFI & NTP FUNCTIONS -----------------//
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
}



//---------------------- DEBUGGING FUNCTIONS -----------------//

void numbers_demo_mode()
{
  // Numbers Demo Mode - Small Numbers
  matrix_0.setFrame(NUM_0); 
  matrix_1.setFrame(NUM_1);
  matrix_2.setFrame(NUM_2);
  matrix_3.setFrame(NUM_3);
  delay(5000);

  matrix_0.setFrame(NUM_4); 
  matrix_1.setFrame(NUM_5);
  matrix_2.setFrame(NUM_6);
  matrix_3.setFrame(NUM_7);
  delay(5000);

  // Clear the display
  matrix_0.clear();
  matrix_1.clear();
  matrix_2.clear();
  matrix_3.clear();
  delay(500);

  matrix_0.setFrame(NUM_8); 
  matrix_1.setFrame(NUM_9);
  delay(5000);

    // Clear the display
  matrix_0.clear();
  matrix_1.clear();
  matrix_2.clear();
  matrix_3.clear();
  delay(500);

  // Numbers Demo Mode
  matrix_0.setFrame(BIG_NUM_0); 
  matrix_1.setFrame(BIG_NUM_1);
  matrix_2.setFrame(BIG_NUM_2);
  matrix_3.setFrame(BIG_NUM_3);
  delay(5000);

  matrix_0.setFrame(BIG_NUM_4); 
  matrix_1.setFrame(BIG_NUM_5);
  matrix_2.setFrame(BIG_NUM_6);
  matrix_3.setFrame(BIG_NUM_7);
  delay(5000);

  // Clear the display
  matrix_0.clear();
  matrix_1.clear();
  matrix_2.clear();
  matrix_3.clear();
  delay(500);

  matrix_0.setFrame(BIG_NUM_8); 
  matrix_1.setFrame(BIG_NUM_9);
  delay(5000);

  //7 Segment Numbers
  matrix_0.setFrame(SEG_NUM_0); 
  matrix_1.setFrame(SEG_NUM_1);
  matrix_2.setFrame(SEG_NUM_2);
  matrix_3.setFrame(SEG_NUM_3);
  delay(5000);

  matrix_0.setFrame(SEG_NUM_4); 
  matrix_1.setFrame(SEG_NUM_5);
  matrix_2.setFrame(SEG_NUM_6);
  matrix_3.setFrame(SEG_NUM_7);
  delay(5000);

  // Clear the display
  matrix_0.clear();
  matrix_1.clear();
  matrix_2.clear();
  matrix_3.clear();
  delay(500);

  matrix_0.setFrame(SEG_NUM_8); 
  matrix_1.setFrame(SEG_NUM_9);
  delay(5000);

  // Clear the display
  matrix_0.clear();
  matrix_1.clear();
  matrix_2.clear();
  matrix_3.clear();
  delay(500);
}

void dates_demo_mode()
{
  integrated_matrix.loadFrame(frame_MON);
  delay(1000);

  integrated_matrix.loadFrame(frame_TUE);
  delay(1000);

  integrated_matrix.loadFrame(frame_WED);
  delay(1000);

  integrated_matrix.loadFrame(frame_THU);
  delay(1000);

  integrated_matrix.loadFrame(frame_FRI);
  delay(1000);

  integrated_matrix.loadFrame(frame_SAT);
  delay(1000);

  integrated_matrix.loadFrame(frame_SUN);
  delay(1000);
}

void debug_time()
{
  // Print out date (DD/MM//YYYY)
  Serial.print(currentTime.getDayOfMonth());
  Serial.print("/");
  Serial.print(Month2int(currentTime.getMonth()));
  Serial.print("/");
  Serial.print(currentTime.getYear());
  Serial.print(" - ");

  // Print time (HH/MM/SS)
  Serial.print(currentTime.getHour());
  Serial.print(":");
  Serial.print(currentTime.getMinutes());
  Serial.print(":");
  Serial.println(currentTime.getSeconds());  
}




