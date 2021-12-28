#include <LiquidCrystal.h>
#include <Wire.h>
#include "DS3231.h"
#include <Time.h>
#include <Servo.h>

RTClib rtc;
Servo myservo;


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//temperature variable
float temp,set_temp = 26,current_temp = 0;

int heater_pin= 22,cooler_pin = 23,pump_pin = 24,light_pin = 25,servo_pin=10;

//button pins
int menu = 14,enter =15,prev =16,next= 17,plus =18,minus= 19;

int main_menu = -1;
int menu_size = 9;


int light_duration = 10,light_start = 6, filter_interval = 12,filter_duration=1,feed_interval = 6,feed_duration = 1;

bool temp_correct = false,filter_on = false;

byte second =      50; //0-59
byte minute =      59; //0-59
byte hour =        11; //0-23

int delay_lcd = 0;




#define DS1307_ADDRESS 0x68
byte zero = 0x00; //workaround for issue #527

void Temp_Control()
{
  temp = analogRead(A0);
  temp = (temp*500)/1024;
  Serial.println(temp);
  current_temp = temp;

  if(current_temp > set_temp+1)
  {
    temp_correct = true;
    digitalWrite(heater_pin,HIGH);
    digitalWrite(cooler_pin,LOW);
    digitalWrite(pump_pin,LOW);
  }
  else if(current_temp < set_temp-1)
  {
    temp_correct = true;
    digitalWrite(heater_pin,LOW);
    digitalWrite(cooler_pin,HIGH);
    digitalWrite(pump_pin,LOW);
  }
  else
  {
    temp_correct = false;
    digitalWrite(heater_pin,HIGH);
    digitalWrite(cooler_pin,HIGH);
    if(!filter_on)
    {
      digitalWrite(pump_pin,HIGH);
    }
    
  }
  
  
  
}
void setDateTime(){
  
  
  byte weekDay =     6;  //1-7
  byte monthDay =    27; //1-31
  byte month =       12; //1-12
  byte year  =       15; //0-99
  int input_string;
  

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator

  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));

  Wire.write(zero); //start 

  Wire.endTransmission();

}

byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

String Get_Time()
{
  DateTime now = rtc.now();
  hour = now.hour();
  second = now.second();
  minute = now.minute();
  

  String times = "TIME: " + String(hour,DEC) + ":" + String(minute,DEC) + ":" + String(second,DEC);

  return times;
}
void Light_Control()
{
  
  if(hour>= light_start && hour<= light_duration + light_start)
  {
    digitalWrite(light_pin,LOW);
  }
  else{
    digitalWrite(light_pin,HIGH);
  }
}
void Filter_Control()
{
  if(hour%filter_interval == 0 && minute <= filter_duration)
  {
    filter_on = true;
    Serial.println("Pump on!!");
    digitalWrite(pump_pin,LOW);
  }
  else if(!temp_correct)
  {
    filter_on = false;
    digitalWrite(pump_pin,HIGH);
  }
  else{
    filter_on = false;
  }
}
void Feed_Control()
{
  if(hour%feed_interval == 0 && minute <= feed_duration)
  {
    myservo.write(180);   
  }
  else{
    myservo.write(0);
  }
}


void Main_Check_Buttons()
{
  if(digitalRead(next)== HIGH)
  {
    if(main_menu < menu_size)
    {
      main_menu += 1;
      delay(500);
    }
    
    
  }
  else if(digitalRead(prev)== HIGH)
  {
    if(main_menu>0)
    {
      main_menu -= 1;
      delay(500);
    }
    
    
  }
  else if(digitalRead(enter)== HIGH)
  {
    
    delay(500);
    
  }
  else if(digitalRead(menu)== HIGH )
  {
    if(main_menu == -1)
    {
      main_menu = 0;
    }
    
    else
    {
      main_menu = -1;
    }
    delay(500);
    
  }
  
}
void Hour_Menu()
{
  
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 0)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Set Hour:");
      lcd.setCursor(0,1);
      String s =  String(hour);
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(hour< 24)
      {
        hour = hour + 1;
        
        delay(500);
      }
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(hour> 0){
        hour = hour - 1;
        delay(500);
      }
      
    }
    Main_Check_Buttons();
  }
  setDateTime();
}
void Minute_Menu()
{
  
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 1)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Set Minute: ");
      lcd.setCursor(0,1);
      String s =  String(minute);
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(minute< 60)
      {
        minute = minute + 1;
        
        delay(500);
      }
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(minute> 0){
        minute = minute - 1;
        delay(500);
      }
      
    }
    Main_Check_Buttons();
  }
  setDateTime();
}
void Second_Menu()
{
  
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 2)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Set Second: ");
      lcd.setCursor(0,1);
      String s = String(second);
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(second< 60)
      {
        second = second + 1;
        
        delay(500);
      }
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(second> 0){
        second = second - 1;
        delay(500);
      }
      
    }
    Main_Check_Buttons();
  }
  setDateTime();
}
/*void Time_Menu()
{
  while(main_menu == 0)
  {
      String s = Get_Time();
        if(delay_lcd != second)
            {
              delay_lcd = second;
              lcd.clear();
              lcd.print("Menu: 01");
              lcd.setCursor(0,1);
              lcd.print(s);
            }

        Main_Check_Buttons();
       
    }
        
        
*
  }*/
  
      
void Temp_Menu()
{
  
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 3)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Temperature: ");
      lcd.setCursor(0,1);
      String s =  String(set_temp) + " Degree";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(set_temp< 40)
      {
        set_temp = set_temp + 1;
        
        delay(500);
      }
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(set_temp> 0){
        set_temp = set_temp - 1;
        delay(500);
      }
      
    }
    Main_Check_Buttons();
  }
}
void Feed_Menu()
{
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 4)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Feed Interval: ");
      lcd.setCursor(0,1);
      String s =String(feed_interval) + " Hour";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(feed_interval<24)
      {
        feed_interval = feed_interval + 1;
        
        delay(500);
      }
      
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(feed_interval>0)
      {
        feed_interval = feed_interval - 1;
        delay(500);
      }
      
      
    }
    Main_Check_Buttons();
  }
}
void Feed_Duration_Menu()
{
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 5)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Feed Duration: ");
      lcd.setCursor(0,1);
      String s =String(feed_duration) + " Minute";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(feed_duration < 59)
      {
        feed_duration = feed_duration + 1;
        
        delay(500);
      }
      
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(feed_duration > 0)
      {
        feed_duration = feed_duration - 1;
        delay(500);
      }
      
      
    }
    Main_Check_Buttons();
  }
}
void Light_Menu()
{
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 6)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Daylight Time:");
      lcd.setCursor(0,1);
      String s =  String(light_duration) + "Hours";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(light_duration < 24)
      {
        light_duration = light_duration + 1;
        
        delay(500);
      }
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(light_duration > 0){
        light_duration = light_duration - 1;
        delay(500);
      }
      
    }
    Main_Check_Buttons();
  }
  
}
void Light_Start_Menu()
{
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 7)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Daylight Start:");
      lcd.setCursor(0,1);
      String s =  String(light_start) + ":00";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(light_start < 24)
      {
        light_start = light_start + 1;
        
        delay(500);
      }
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(light_start > 0){
        light_start = light_start - 1;
        delay(500);
      }
      
    }
    Main_Check_Buttons();
  }
  
}
void Filter_Menu()
{
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 8)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Filter Interval: ");
      lcd.setCursor(0,1);
      String s =String(filter_interval) + " Hour";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(filter_interval < 24)
      {
        filter_interval = filter_interval + 1;
        
        delay(500);
      }
      
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(filter_interval > 0)
      {
        filter_interval = filter_interval - 1;
        delay(500);
      }
      
      
    }
    Main_Check_Buttons();
  }
}
void Filter_Duration_Menu()
{
  unsigned long previousMillis = 0;
  const long interval = 500;
  while(main_menu == 9)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.print("Filter Duration: ");
      lcd.setCursor(0,1);
      String s =String(filter_duration) + "Minute";
      lcd.print(s);
    }
    
    if(digitalRead(plus)== HIGH)
    {
      if(filter_duration < 59)
      {
        filter_duration = filter_duration + 1;
        
        delay(500);
      }
      
    }
    else if(digitalRead(minus)== HIGH)
    {
      if(filter_duration > 0)
      {
        filter_duration = filter_duration - 1;
        delay(500);
      }
      
      
    }
    Main_Check_Buttons();
  }
}
void Main_Window()
{
      while(main_menu == -1)
      {
        String s = Get_Time();
        
        String s1 = "TEMP: " + String(current_temp);
        if(delay_lcd != second)
        {
          delay_lcd = second;
          lcd.clear();
          lcd.print(s);
          lcd.setCursor(0,1);
          lcd.print(s1);
        }
        Light_Control();
        Filter_Control();
        Temp_Control();
        Feed_Control();
        Main_Check_Buttons();
        }
}
void Option_Menu()
{
  
  delay(20);
  
  
  while(true)
  {
    if(digitalRead(menu)== HIGH )
     {
      
      delay(30);
      break;
     }
     Main_Check_Buttons();
     Serial.println(main_menu);
     switch(main_menu)
      {
        case 0:
          Hour_Menu();
          break;
        case 1:
          Minute_Menu();
          break;
        case 2:
          Second_Menu();
          break;
        case 3:
          Temp_Menu();
          break;
        case 4:
          Feed_Menu();
          break;
        case 5:
          Feed_Duration_Menu();
          break;
        case 6:
          Light_Menu();
          break;
        case 7:
          Light_Start_Menu();
          break;
        case 8:
          Filter_Menu();
          break;
        case 9:
          Filter_Duration_Menu();
          break;
        
        case -1:
          Main_Window();
          break;
         
      }
     
     
    
    
    
      

      
     
    
    
  }
}
void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Wire.begin();
  
  setDateTime(); //MUST CONFIGURE IN FUNCTION
  
  Serial.begin(9600);

  pinMode(heater_pin,OUTPUT);
  pinMode(cooler_pin,OUTPUT);
  pinMode(pump_pin,OUTPUT);
  pinMode(light_pin,OUTPUT);

  myservo.attach(servo_pin);
  myservo.write(0);   

  digitalWrite(heater_pin,HIGH);
  digitalWrite(cooler_pin,HIGH);
  digitalWrite(pump_pin,HIGH);
  digitalWrite(light_pin,HIGH);

  pinMode(menu,INPUT);
  pinMode(enter,INPUT);
  pinMode(prev,INPUT);
  pinMode(next,INPUT);
  pinMode(plus,INPUT);
  pinMode(minus,INPUT);
  
  

  lcd.print("Smart Aqauarium!");
  
  delay(500);
  lcd.clear();

  

  
  

}
void loop() {
  
    
    
    
     Option_Menu();
     
  
  

}
