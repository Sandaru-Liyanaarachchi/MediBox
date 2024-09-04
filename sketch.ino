//include the libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>

//define the parameters
//OLED display
#define Screen_width 128
#define Screen_height 64
#define OLED_reset -1
#define Screen_address 0x3C

//buzzer, LEDs & push buttons pins
#define Buzzer 5
#define LED_1 15

#define LED_2 13
#define LED_3 14

#define PB_Cancel 34
#define PB_Up 33
#define PB_OK 32
#define PB_Down 35


#define DHT 12 //DHT22 pin

//NTP server
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET_DST 0

//declare objects
Adafruit_SSD1306 display(Screen_width, Screen_height, &Wire, OLED_reset);
DHTesp dhtSensor;

//global variables
//time
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

//alarm
bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0,1,0};
int alarm_minutes[] = {1,1,1};
bool alarm_triggered[] = {false, false,false};

//buzzer
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C,D,E,F,G,A,B,C_H};
int n_notes = 8;

//NTP server
int UTC_hour = 5;
int UTC_minute = 30;
int UTC_offset = 3600*5 + 60*30;

//menu
int current_mode = 0;
int max_modes = 5;
String modes[] = {"1-Set Time", "2-Set Alarm_1", "3-Set Alarm_2", "4-Set Alarm_3", "5-Disable Alarms"};

void setup() {
  // set the pins
  pinMode(Buzzer, OUTPUT);
  pinMode(LED_1, OUTPUT);

  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  pinMode(PB_Cancel, INPUT);
  pinMode(PB_Up, INPUT);
  pinMode(PB_Down, INPUT);
  pinMode(PB_OK, INPUT);

  dhtSensor.setup(DHT, DHTesp::DHT22); // set the DHT sensor

  
  Serial.begin(9600);//serial communication

  //generate the dispaly voltage from 3.3V internally
  if (!display.begin (SSD1306_SWITCHCAPVCC, Screen_address)){ 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  //show the display buffer on the screen 
  display.display();
  delay(2000);
  
  //connect to the WIFI
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WIFI", 2, 0, 0);
    
  }

  display.clearDisplay();
  print_line("Connected to WIFI", 2, 0, 0);

  configTime(UTC_offset, UTC_OFFSET_DST, NTP_SERVER);

  //clear the buffer
  display.clearDisplay();

  //set up the OLED display
  print_line("WELCOME to MEDIBOX!!!",1 , 1, 2);
  delay(2000);
  display.clearDisplay();

}

void loop() {
  
  update_time_with_check_alarm();
  if(digitalRead(PB_OK) == LOW){
    delay(200);
    go_to_menu();
  }

  check_temp();
  
}

//function for printing the msg on OLED display
void print_line(String msg, int textSize, int column, int row){
  
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(msg);

  display.display();
}

//function for displaying the time in OLED display  
void print_time_now(void){
  display.clearDisplay();
  print_line(String(days), 2, 0, 0);
  print_line(":", 2, 20, 0);  
  print_line(String(hours), 2, 30, 0);
  print_line(":", 2, 50, 0);
  print_line(String(minutes), 2, 60, 0);
  print_line(":", 2, 80, 0);
  print_line(String(seconds), 2, 90, 0);
}

//function to update the time automatically
void update_time(void){
  configTime(UTC_offset, UTC_OFFSET_DST, NTP_SERVER);
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  hours = atoi(timeHour);

  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  minutes = atoi(timeMinute);

  char timeSecond[3];
  strftime(timeSecond, 3, "%S", &timeinfo);
  seconds = atoi(timeSecond);

  char timeDay[3];
  strftime(timeDay, 3, "%d", &timeinfo);
  days = atoi(timeDay);
  
}

//function to ring the alarm
void ring_alarm(){
  display.clearDisplay();
  print_line("Medicine Time!!!", 2, 0, 0); // show this msg on display

  digitalWrite(LED_1, HIGH); //on the LED when the alarm is ringing

  bool break_Happened = false;

  //ring the buzzer until the alarm is cancelled
  while (break_Happened == false && digitalRead(PB_Cancel) == HIGH){
    for(int i = 0; i < n_notes; i++){
      if(digitalRead(PB_Cancel) == LOW){
        delay(200);
        break_Happened = true;
        break;
      }
      tone(Buzzer, notes[i]);
      delay(500);
      noTone(Buzzer);
      delay(2);
    }
  }
  
  digitalWrite(LED_1, LOW);
  display.clearDisplay();

}

//function to update time and check alarm
void update_time_with_check_alarm(void){
  update_time();
  print_time_now();

  if(alarm_enabled == true){
    for (int i = 0; i < n_alarms; i++){
      if(alarm_triggered[i] == false && alarm_hours[i] == hours && alarm_minutes[i] == minutes){
        ring_alarm();
        alarm_triggered[i] = true;
      }
    }
  }
}

// wait till press the push buttons
int wait_for_button_press(){
  while (true){
    if(digitalRead(PB_Up) == LOW){
      delay(200);
      return PB_Up;
    }
    
    else if(digitalRead(PB_Down) == LOW){
      delay(200);
      return PB_Down;
    }

    else if(digitalRead(PB_OK) == LOW){
      delay(200);
      return PB_OK;
    }
    
    else if(digitalRead(PB_Cancel) == LOW){
      delay(200);
      return PB_Cancel;
    }
    
    update_time(); //after pressing the push button update the time
  }
}

//function to navigate to the menu
void go_to_menu(){
  
  while (digitalRead(PB_Cancel) == HIGH){
    display.clearDisplay();
    print_line(modes[current_mode], 2, 0, 0);//show the current mode in display

    int pressed = wait_for_button_press();
    if (pressed == PB_Up){
      delay(200);
      current_mode += 1;
      current_mode = current_mode % max_modes;
    }

    
    else if (pressed == PB_Down){
      delay(200);
      current_mode -= 1;
      if(current_mode < 0){
        current_mode = max_modes - 1;
      } 
    }

    else if (pressed == PB_OK){
      delay(200);
      run_mode(current_mode);
    }

    else if (pressed == PB_Cancel){
      delay(200);
      break;
    }
  }
}

//function to set teh UTC time
void set_time(){
  //for hours
  int temp_hour = UTC_hour;

  while(true){
    display.clearDisplay();
    print_line("Enter hour:" + String(temp_hour), 2, 0, 0);

    int pressed = wait_for_button_press();
    if (pressed == PB_Up){
      delay(200);
      temp_hour = ((temp_hour+12+1) % 25) - 12;
    }

    
    else if (pressed == PB_Down){
      delay(200);
      temp_hour -= 1;
      if(temp_hour < -12){
        temp_hour = 12;
      } 
    }

    else if (pressed == PB_OK){
      delay(200);
      UTC_hour = temp_hour;
      break;
    }

    else if (pressed == PB_Cancel){
      delay(200);
      break;
    }
  }
  //for minutes
  int temp_minute = UTC_minute;

  while(true && UTC_hour != 12 && UTC_hour != -12){
    display.clearDisplay();
    print_line("Enter minute:" + String(temp_minute), 2, 0, 0);

    int pressed = wait_for_button_press();
    if (pressed == PB_Up){
      delay(200);
      temp_minute += 10;
      temp_minute = temp_minute % 60;
    }

    
    else if (pressed == PB_Down){
      delay(200);
      temp_minute -= 10;
      if(temp_minute < 0){
        temp_minute = 50;
      } 
    }

    else if (pressed == PB_OK){
      delay(200);
      UTC_minute = temp_minute;
      break;
    }

    else if (pressed == PB_Cancel){
      delay(200);
      break;
    }
  }

  if (UTC_hour < 0){
    UTC_offset = 3600*UTC_hour - 60*UTC_minute;
  }
  else{
    UTC_offset = 3600*UTC_hour + 60*UTC_minute;
  }

  display.clearDisplay();
  print_line("Time is set", 2, 0, 0); //show the msg on dispaly when the time is set
  delay(1000);
  
}

//funtion to set the three alarms
void set_alarm (int alarm) {
  //set hours in alarm
  int temp_hour = alarm_hours[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 2, 0, 0);


    int pressed = wait_for_button_press();
    //increase one hour by pressing the button
    if (pressed == PB_Up) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }
    //decrease one hour by pressing the button
    else if (pressed == PB_Down) {
      delay(200);
      temp_hour -= 1;

      if (temp_hour < 0) {
        temp_hour = 23;
      }
    }

    else if (pressed = PB_OK) {
      delay(200);
      alarm_hours [alarm] = temp_hour;
      alarm_enabled = true;
      alarm_triggered[alarm] = false;
      break;
    }

    else if (pressed == PB_Cancel) {
      delay(200);
      break;
    }
  }

  // set minutes in alarm
  int temp_minute = alarm_minutes [alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter minute:" + String (temp_minute), 2, 0, 0);
    int pressed = wait_for_button_press();

    //increase one minute by pressing the button 
    if (pressed == PB_Up) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }

    //descrease one minute by pressing the button
    else if (pressed == PB_Down) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    }

    else if (pressed == PB_OK) {
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      alarm_enabled = true;
      alarm_triggered[alarm] = false;
      break;
    }
    else if (pressed == PB_Cancel) {
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Alarm is set", 2, 0, 0);
  delay(1000);
}

//function to act the tasks in the selected modes.
void run_mode(int mode){
  //set the time
  if(mode == 0){
    set_time();
  }

  //set the alarms
  else if (mode == 1 || mode == 2 || mode == 3){
    set_alarm(mode-1);
  }

  //disable alarms
  else if (mode == 4){
    alarm_enabled = false;
  }
}

//function to check if the temperature and humidity are within the suitable range
void check_temp(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity(); // read the data of temperature and humidity
  bool temp_OK = true;
  
  //if temperature > 32C then dipaly the msg and turn on the red LED
  if(data.temperature > 32){
    display.clearDisplay();
    temp_OK = false;
    digitalWrite(LED_2, HIGH);
    print_line("Temp High", 1, 0, 40);
  }

  //if temperature < 26C then dipaly the msg and turn on the red LED
  else if(data.temperature < 26){
    display.clearDisplay();
    temp_OK = false;
    digitalWrite(LED_2, HIGH);
    print_line("Temp Low", 1, 0, 40);
  }

  //if temperature in range then turn off the red LED
  if(temp_OK){
    digitalWrite(LED_2, LOW);
  }

  bool humi_OK = true;

  //if humidity > 80 then dipaly the msg and turn on the purple LED
  if(data.humidity > 80){
    display.clearDisplay();
    humi_OK = false;
    digitalWrite(LED_3, HIGH);
    print_line("Humidity High", 1, 0, 50);
  }

  //if humidity < 60 then dipaly the msg and turn on the purple LED
  else if(data.humidity < 60){
    display.clearDisplay();
    humi_OK = false;
    digitalWrite(LED_3, HIGH);
    print_line("Humidity Low", 1, 0, 50);
  }

  //if humidity is in the range then turn onoff the purple LED
  if (humi_OK){
    digitalWrite(LED_3, LOW);
  }
}