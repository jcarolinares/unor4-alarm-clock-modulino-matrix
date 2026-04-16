/*
 Arduino matrix_0 LED Nixie Clock

 A Nixie clock made with four Modulinos LED matrix_0

 Made by Julián Caro Linares for Arduino INC

 CC-BY-SA
*/

#include <Arduino_Modulino.h>
#include "ArduinoGraphics.h"
#include "Modulino_LED_Matrix.h"
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
DayOfWeek day_of_week[] = {DayOfWeek::MONDAY, DayOfWeek::TUESDAY, DayOfWeek::WEDNESDAY, DayOfWeek::THURSDAY, DayOfWeek::FRIDAY, DayOfWeek::SATURDAY, DayOfWeek::SUNDAY};
Month months_of_year[] = {Month::JANUARY, Month::FEBRUARY, Month::MARCH, Month::APRIL, Month::MAY, Month::JUNE, Month::JULY, Month::AUGUST, Month::SEPTEMBER, Month::OCTOBER, Month::NOVEMBER, Month::DECEMBER};
int day_of_month_index = 1;


// Initial time
RTCTime currentTime(day_of_month_index, Month::JANUARY, 2026, 0, 0, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);

// Alarm time & Variables
RTCTime alarmTime(day_of_month_index, Month::JANUARY, 2026, 15, 46, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
bool alarm_status = false;

// Millis Variables
const long interval_time = 10000;
unsigned long previousMillis = 0;
unsigned long currentMillis = millis();

// State Machines Variables
int state = 1;
int menu_option = 0;

// Date Show Up Variables
int show_date_ite = 0; // When the date shows up after a number of time iterations


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

  getInternetTime();

  // // Time and Date Init Setup
  // // currentTime = RTCTime(7, Month::APRIL, 2026, hours, minutes, 00, DayOfWeek::TUESDAY, SaveLight::SAVING_TIME_ACTIVE); // Debugging Manual Overiding
  // RTC.setTime(currentTime);
  
  // Initial Matrixes frames
  matrix_show_time();
  integrated_matrix_date();
}

void loop() {

  // Transitions of the State Machine
  // Human interface sensors reading
  direction = knob.getDirection();
  click = knob.isPressed();

  // Initial press entering menu mode
  if (direction == 1){
    buzzer.tone(frequency, duration);

    // Instant show up of Date and year
    RTC.getTime(currentTime);
    show_date_and_month();

    state = 5;
    delay(500); // To avoid to skips because of the previous knob click
  }
  else if (direction == -1){
    buzzer.tone(frequency, duration);

    // Get current time from RTC
    RTC.getTime(currentTime);

    matrix_show_time();
    integrated_matrix_date();

    state = 1;
    delay(500); // To avoid to skips because of the previous knob click
  }
  else if (click == true && state != 0){ // Knob click enters Menu mode
    buzzer.tone(frequency, duration);
    state = 0;
    delay(500); // To avoid to skips because of the previous knob click
  }


  // State Machine
  switch(state){
    
    case 0: // Menu mode

      // Matrixes Animation
      matrix_3.setFrame(MENU_M);
      matrix_2.setFrame(MENU_E);
      matrix_1.setFrame(MENU_N);
      matrix_0.setFrame(MENU_U);

      click = knob.isPressed();

      // Menu 
      // Initial press entering menu mode
      menu_option = 0;
      while (click == false){ // Knob click enters Menu mode
        click = knob.isPressed();
        direction = knob.getDirection();

        if(direction == 1 && menu_option < 4){
          buzzer.tone(494, duration);
          menu_option++;
        }
        else if(direction == -1 && menu_option  > 0){
          buzzer.tone(330, duration);
          menu_option--;
        }

        switch(menu_option){ // Menu Navigation & Selection
          case 0:
            integrated_matrix.loadFrame(icon_MENU_TIME);
            state = 2;
           break;
          case 1:
            integrated_matrix.loadFrame(icon_MENU_ALARM);
            state = 3;
            break;
          case 2:
            integrated_matrix.loadFrame(icon_MENU_SYNC);
            state = 4;
            break;
          case 3:
            integrated_matrix.loadFrame(icon_MENU_DATE);
            state = 6;
            break;               
          case 4:
            integrated_matrix.loadFrame(icon_MENU_EXIT);
            state = 1;
            break;
        }
      }
    buzzer.tone(frequency, duration);  
    delay(500); // To avoid Double knob click
    break;

    case 1: // Clock Mode
      currentMillis = millis();
      
      if(show_date_ite>3){ // Every a number of seconds we show the date instead of the time
        show_date_ite = 0;
        // Get current time from RTC
        RTC.getTime(currentTime);
        show_date_and_month();
        check_alarm();
      }
      else if (currentMillis - previousMillis >= interval_time){
        previousMillis = currentMillis;

        // Get current time from RTC
        RTC.getTime(currentTime);

        matrix_show_time();
        integrated_matrix_date();
        check_alarm();
        show_date_ite++;
      } 
      
    break;

    case 2: // Manual Time Set 
      set_time();
      state = 1;
      delay(750); // To avoid to enter into the menu because of the previous knob click
    break;

    case 3: // Manual Alarm Time Set
      set_alarm();
      state = 1;
      delay(750); // To avoid to enter into the menu because of the previous knob click
    break;

    case 4: // NTP Time Synch
      getInternetTime();
      state = 1;
    break;
    
    case 5: // Date and Month Mode

      currentMillis = millis();
      if (currentMillis - previousMillis >= interval_time){
        previousMillis = currentMillis;

        // Get current time from RTC
        RTC.getTime(currentTime);
        show_date_and_month();
        check_alarm();
      } 
    break;

    case 6: // Date and Month Setup
      set_date();
      state = 1;
      delay(750);
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

  matrix_3.setFrame(SEG_NUM_0);
  matrix_2.setFrame(SEG_NUM_0);
  matrix_1.setFrame(SEG_NUM_0);
  matrix_0.setFrame(SEG_NUM_0);

  DayOfWeek current_day_of_week = currentTime.getDayOfWeek();
  Month current_month = currentTime.getMonth();
  int day_of_month = currentTime.getDayOfMonth();

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  // Hours Set
  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_HOUR);
    // Serial.println(hours);
    if (direction == 1 && hours < 24){
      buzzer.tone(494, duration);
      hours++;
      currentTime = RTCTime(day_of_month, current_month, 2026, hours, minutes, 00, current_day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }
    else if(direction == -1 && hours > 0){
      buzzer.tone(330, duration);
      hours--;
      currentTime = RTCTime(day_of_month, current_month, 2026, hours, minutes, 00, current_day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Hours Set");
  buzzer.tone(frequency, duration);
  delay(500); // Delay to avoid the detection of the previous click of the Modulino Knob

  // Minutes Set
  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_MIN);
    // Serial.println(minutes);
    if (direction == 1 && minutes < 60){
      buzzer.tone(494, duration);
      minutes++;
      currentTime = RTCTime(day_of_month, current_month, 2026, hours, minutes, 00, current_day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }
    else if(direction == -1 && minutes > 0){
      buzzer.tone(330, duration);
      minutes--;
      currentTime = RTCTime(day_of_month, current_month, 2026, hours, minutes, 00, current_day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      matrix_show_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Minutes Set");
  buzzer.tone(frequency, duration);
  delay(500); // Delay to avoid the detection of the previous click of the Modulino Knob

  
  // Day set
  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  int day_of_week_index = 0;
  integrated_matrix_date();
  while(click == 0)
  {
    // integrated_matrix.loadFrame(icon_MIN);
    // Serial.println(minutes);
    if (direction == 1 && day_of_week_index < 6){
      buzzer.tone(494, duration);
      day_of_week_index++;
      currentTime = RTCTime(day_of_month, current_month, 2026, hours, minutes, 00, day_of_week[day_of_week_index], SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      integrated_matrix_date();
    }
    else if(direction == -1 && day_of_week_index > 0){
      buzzer.tone(330, duration);
      day_of_week_index--;
      currentTime = RTCTime(day_of_month, current_month, 2026, hours, minutes, 00, day_of_week[day_of_week_index], SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      integrated_matrix_date();
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

  matrix_3.setFrame(SEG_NUM_0);
  matrix_2.setFrame(SEG_NUM_0);
  matrix_1.setFrame(SEG_NUM_0);
  matrix_0.setFrame(SEG_NUM_0);

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  int alarm_hours = 0;
  int alarm_minutes = 0;

  // Hours tens
  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_ALARM_SET);
    // Serial.println(hours);
    if (direction == 1 && alarm_hours < 24){
      buzzer.tone(494, duration);
      alarm_hours++;
      alarmTime = RTCTime(7, Month::APRIL, 2026, alarm_hours, alarm_minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }
    else if(direction == -1 && alarm_hours > 0){
      buzzer.tone(330, duration);
      alarm_hours--;
      alarmTime = RTCTime(7, Month::APRIL, 2026, alarm_hours, alarm_minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Alarm Hours Set");
  buzzer.tone(frequency, duration);
  delay(500); // Delay to avoid the detection of the previous click of the Modulino Knob

  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();

  while(click == 0)
  {
    integrated_matrix.loadFrame(icon_ALARM_SET);
    // Serial.println(alarm_minutes);
    if (direction == 1 && alarm_minutes < 60){
      buzzer.tone(494, duration);
      alarm_minutes++;
      alarmTime = RTCTime(7, Month::APRIL, 2026, alarm_hours, alarm_minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }
    else if(direction == -1 && alarm_minutes > 0){
      buzzer.tone(330, duration);
      alarm_minutes--;
      alarmTime = RTCTime(7, Month::APRIL, 2026, alarm_hours, alarm_minutes, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE); // Day, month and Day of the week are not used for the Alarm mode so definition is generic
      matrix_show_alarm_time();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Alarm Minutes Set");
  buzzer.tone(frequency, duration);
  alarm_status = true;
}

// Manually set Day and Month using the Modulino Knob
void set_date(){

  int current_hour = currentTime.getHour();
  int current_minutes = currentTime.getMinutes();
  DayOfWeek day_of_week = currentTime.getDayOfWeek();

  click = knob.isPressed();
  direction = knob.getDirection();

  show_date_and_month();

  // Day of the month
  while(click == 0)
  {
    if (direction == 1 && day_of_month_index < 31){
      buzzer.tone(494, duration);
      day_of_month_index++;
      currentTime = RTCTime(day_of_month_index, Month::JANUARY, 2026, current_hour, current_minutes, 00, day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      show_date_and_month();
    }
    else if(direction == -1 && day_of_month_index > 0){
      buzzer.tone(330, duration);
      day_of_month_index--;
      currentTime = RTCTime(day_of_month_index, Month::JANUARY, 2026, current_hour, current_minutes, 00, day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      show_date_and_month();
    }

    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.print("Day of the month Set: ");
  Serial.println(currentTime);

  buzzer.tone(frequency, duration);
  delay(500); // Delay to avoid the detection of the previous click of the Modulino Knob

  click = knob.isPressed();
  direction = knob.getDirection();

  int month_index = 0;
  show_date_and_month();
  while(click == 0)
  {
    if (direction == 1 && month_index < 12){
      buzzer.tone(494, duration);
      month_index++;
      currentTime = RTCTime(day_of_month_index, months_of_year[month_index], 2026, current_hour, current_minutes, 00, day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      show_date_and_month();
    }
    else if(direction == -1 && month_index > 0){
      buzzer.tone(330, duration);
      month_index--;
      currentTime = RTCTime(day_of_month_index, months_of_year[month_index], 2026, current_hour, current_minutes, 00, day_of_week, SaveLight::SAVING_TIME_ACTIVE);
      RTC.setTime(currentTime);
      show_date_and_month();
    }

    position = knob.get();
    click = knob.isPressed();
    direction = knob.getDirection();
  }

  Serial.println("Month Set: ");
  Serial.print(currentTime);
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

// Shows on the R4 Integrated Matrix theMonth
void integrated_matrix_month()
{
  Month currently_month = currentTime.getMonth();
  if(currently_month == Month::JANUARY){
    integrated_matrix.loadFrame(frame_JAN);
  }
  else if(currently_month == Month::FEBRUARY){
    integrated_matrix.loadFrame(frame_FEB);
  }
  else if(currently_month == Month::MARCH){
    integrated_matrix.loadFrame(frame_MAR);
  }
  else if(currently_month == Month::APRIL){
    integrated_matrix.loadFrame(frame_APR);
  }
  else if(currently_month == Month::MAY){
    integrated_matrix.loadFrame(frame_MAY);
  }
  else if(currently_month == Month::JUNE){
    integrated_matrix.loadFrame(frame_JUN);
  }
  else if(currently_month == Month::JULY){
    integrated_matrix.loadFrame(frame_JUL);
  }
  else if(currently_month == Month::AUGUST){
    integrated_matrix.loadFrame(frame_AUG);
  }
  else if(currently_month == Month::SEPTEMBER){
    integrated_matrix.loadFrame(frame_SEP);
  }
  else if(currently_month == Month::OCTOBER){
    integrated_matrix.loadFrame(frame_OCT);
  }
  else if(currently_month == Month::NOVEMBER){
    integrated_matrix.loadFrame(frame_NOV);
  }
  else if(currently_month == Month::DECEMBER){
    integrated_matrix.loadFrame(frame_DEC);
  }
  delay(100);
}

// Helper functions to avoid a lot of switch cases on the matrix_show_time function
void setSegmentFrame(ModulinoLEDMatrix& matrix, int digit) {
  switch (digit) { // You can change here the Number Font
    case 0: matrix.setFrame(SEG_NUM_0); break;
    case 1: matrix.setFrame(SEG_NUM_1); break;
    case 2: matrix.setFrame(SEG_NUM_2); break;
    case 3: matrix.setFrame(SEG_NUM_3); break;
    case 4: matrix.setFrame(SEG_NUM_4); break;
    case 5: matrix.setFrame(SEG_NUM_5); break;
    case 6: matrix.setFrame(SEG_NUM_6); break;
    case 7: matrix.setFrame(SEG_NUM_7); break;
    case 8: matrix.setFrame(SEG_NUM_8); break;
    case 9: matrix.setFrame(SEG_NUM_9); break;
  }
}

// Helper function for the colon numbers (matrix 2)
void setColonFrame(ModulinoLEDMatrix& matrix, int digit) {
  switch (digit) { // You can change here the Number Font
    case 0: matrix.setFrame(COLON_NUM_0); break;
    case 1: matrix.setFrame(COLON_NUM_1); break;
    case 2: matrix.setFrame(COLON_NUM_2); break;
    case 3: matrix.setFrame(COLON_NUM_3); break;
    case 4: matrix.setFrame(COLON_NUM_4); break;
    case 5: matrix.setFrame(COLON_NUM_5); break;
    case 6: matrix.setFrame(COLON_NUM_6); break;
    case 7: matrix.setFrame(COLON_NUM_7); break;
    case 8: matrix.setFrame(COLON_NUM_8); break;
    case 9: matrix.setFrame(COLON_NUM_9); break;
  }
}

// Shows the time on each Modulino's Matrix
void matrix_show_time()
{
  int hour_tens = currentTime.getHour() / 10;
  int hour_units = currentTime.getHour() % 10;
  int minutes_tens = currentTime.getMinutes() / 10;
  int minutes_units = currentTime.getMinutes() % 10;

  // Call the helpers to set the correct frame on each matrix
  setSegmentFrame(matrix_3, hour_tens);
  setColonFrame(matrix_2, hour_units); // Units of hours is special it includes a colon on the frames
  setSegmentFrame(matrix_1, minutes_tens);
  setSegmentFrame(matrix_0, minutes_units);
}

// Shows the Alarm time on each Modulino's Matrix
void matrix_show_alarm_time()
{
  int hour_tens = alarmTime.getHour()/10;
  int hour_units = alarmTime.getHour()%10;
  int minutes_tens = alarmTime.getMinutes()/10;
  int minutes_units = alarmTime.getMinutes()%10;

  // Call the helpers to set the correct frame on each matrix
  setSegmentFrame(matrix_3, hour_tens);
  setColonFrame(matrix_2, hour_units); // Units of hours is special it includes a colon on the frames
  setSegmentFrame(matrix_1, minutes_tens);
  setSegmentFrame(matrix_0, minutes_units);
}


// Shows the Date and Month
void show_date_and_month(){
  int day_tens = currentTime.getDayOfMonth()/10;
  int day_units = currentTime.getDayOfMonth()%10;

  setSegmentFrame(matrix_3, day_tens);
  setSegmentFrame(matrix_2, day_units);

  matrix_1.setFrame(MENU_t_lower);
  matrix_0.setFrame(MENU_h_lower);

  integrated_matrix_month();
}


//---------------------- WIFI & NTP FUNCTIONS -----------------//

// Manages the Wifi connection
bool connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    return false;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  int attempts_counter = 0; // To store total number of connection attemps before connection timeout
  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) { //TODO add here a 1 minute timeout
    attempts_counter++;
    
    if(attempts_counter >1){ // 20 seconds timeout
      Serial.println("Network Connection Timeout - Not possible to connect");
      return false;
    }
    
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);

  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
  return true;
}

void getInternetTime(){
  // Wifi Init Setup
  if (connectToWiFi() == true){
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
  }
  else{
    Serial.println("\nInternet connection failed. Starting Manual Set Time");
    state = 2;
  }
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




