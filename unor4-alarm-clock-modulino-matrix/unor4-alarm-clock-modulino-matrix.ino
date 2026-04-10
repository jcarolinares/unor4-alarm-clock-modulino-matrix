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
int hours = 0;
int minutes = 0;

// Initial time
RTCTime currentTime(7, Month::APRIL, 2026, 0, 0, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);

// Alarm time & Variables
RTCTime alarmTime(7, Month::APRIL, 2026, 15, 46, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
bool alarm_status = true;

// State Machines Variables
int state = 3;

void setup() {

  Serial.begin(115200);
  RTC.begin();

  // Modulinos Begin
  Modulino.begin();
  knob.begin();
  buzzer.begin();

  // Matrix Init Setup
  matrix_0.begin();
  matrix_1.begin();
  matrix_2.begin();
  matrix_3.begin();

  // Integrated Matrix Init Setup
  integrated_matrix.begin();

  // Wifi Init Setup
  connectToWiFi();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  // RTC, NTP and Timezone Init Setup
  auto timeZoneOffsetHours = 2; // Time zone offset
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Final RTC Timezone Configured
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));


  // Time and Date Init Setup
  // currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::TUESDAY, SaveLight::SAVING_TIME_ACTIVE); // Debugging Manual Overiding
  RTC.setTime(currentTime);
  
  // Initial Matrixes frames
  matrix_show_time();
  integrated_matrix_date();

}

void loop() {

  
  // Transitions of the State Machine
  // TODO
  
  // Human interface sensors reading
  click = knob.isPressed();

  // Initial press entering menu mode
  if (click == true && state != 0){ // Knob click enters Menu mode // FIXME after clock set or alarm sets it enters direcly on the menu, should not work that way
    state = 3; // TODO should be 0 but I enter alarm mode until this menu is finished 
    delay(500); // To avoid to skips because of the previous knob click
  }


  // State Machine
  switch(state){
    
    case 0: // Menu mode
      matrix_3.setFrame(MENU_M);
      matrix_2.setFrame(MENU_E);
      matrix_1.setFrame(MENU_N);
      matrix_0.setFrame(MENU_U);
    break;

    case 1: // Clock Mode
      // Get current time from RTC
      RTC.getTime(currentTime);

      matrix_show_time();
      integrated_matrix_date();
      check_alarm();
      delay(1000); // FIXME change this for a millis thing
    break;

    case 2: // Manual Time Set
      set_time();
      state = 1;
      delay(500); // To avoid to enter into the menu because of the previous knob click
    break;

    case 3: // Manual Alarm Time Set
      set_alarm();
      state = 1;
      delay(500); // To avoid to enter into the menu because of the previous knob click
    break;

  }

}



//---------------------- FUNCTIONALITY FUNCTIONS -----------------//

// Check the Alarm time and triggered it when is the right time
void check_alarm(){
  int current_hour = currentTime.getHour();
  int alarm_hour = alarmTime.getHour();
  int current_minutes = currentTime.getMinutes();
  int alarm_minutes = alarmTime.getMinutes();

  // Alarm is ON
  if (alarm_status == true && current_hour == alarm_hour && current_minutes == alarm_minutes ){
    
    click = knob.isPressed();
    integrated_matrix.loadFrame(icon_ALARM);
  
    while(click == false){
      buzzer.tone(494, 500);
      
      integrated_matrix.loadSequence(alarm_animation);
      integrated_matrix.play(true);
      
      delay(250);
      buzzer.tone(0, 500);
      delay(250);
      
      click = knob.isPressed();
    }
    integrated_matrix.play(false);
    alarm_status = false;
    integrated_matrix_date();
  }
}

// Manually set time using the Modulino Knob
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

  Serial.println("Hours Set");
  delay(500); // Delay to avoid the detection of the previous click of the Modulino Knob

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

  Serial.println("Minutes Set");
  buzzer.tone(frequency, duration);
}

// Manually set time using the Modulino Knob
void set_alarm(){

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  // Hours tens
  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_ALARM_SET);
    // Serial.println(hours);
    if (direction == 1 && hours < 24){
      buzzer.tone(330, duration);
      hours++;
      alarmTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }
    else if(direction == -1 && hours > 0){
      hours--;
      alarmTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Hours Set");
  delay(500); // Delay to avoid the detection of the previous click of the Modulino Knob

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_ALARM_SET);
    // Serial.println(minutes);
    if (direction == 1 && minutes < 60){
      buzzer.tone(330, duration);
      minutes++;
      alarmTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }
    else if(direction == -1 && minutes > 0){
      minutes--;
      alarmTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Minutes Set");
  buzzer.tone(frequency, duration);
}

//---------------------- MATRIX FRAMES FUNCTIONS -----------------//

// Shows on the R4 Integrated Matrix the Date of the Day
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

// Shows the time on each Modulino's Matrix
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

// Shows the time on each Modulino's Matrix
void matrix_show_alarm_time()
{
  int hour_tens = alarmTime.getHour()/10;
  int hour_units = alarmTime.getHour()%10;
  int minutes_tens = alarmTime.getMinutes()/10;
  int minutes_units = alarmTime.getMinutes()%10;

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

//---------------------- WIFI & NTP FUNCTIONS -----------------//

// Manages the Wifi connection
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

// Shows on the serial the status of the Wifi connection
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

// Shows all the available number frames on each Modulino's Matrix
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

// Shows the days of the week frames on the R4 Matrix
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

// Shows on serial the actual current time to debug problems with the clock
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




