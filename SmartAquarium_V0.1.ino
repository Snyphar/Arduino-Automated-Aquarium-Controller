#include <RTClib.h>
#include <Wire.h>


//////////////////////////////////  Delay //////////////////////////////////////////
struct Delay 
/////////////////////////////////////// temperature probe ////////////////////////////////
const double VCC = 3.3;             // NodeMCU on board 3.3v vcc
const double R2 = 10450;            // 10k ohm series resistor
const double adc_resolution = 1023; // 10-bit adc

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741;
const int cooler = 14;
const int heater = 12;
double temp_low = 24;
double temp_high = 28;


double Temperature()
{
  double Vout, Rth, temperature, adc_value;

  adc_value = analogRead(A0);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

  /*  Steinhart-Hart Thermistor Equation:
      Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
      where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
  temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)), 3))));  // Temperature in kelvin

  temperature = temperature - 273.15;  // Temperature in degree celsius
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" degree celsius");
  return temperature;
}
double TempControl(double temp, double low, double high)
{
  if(temp<low)
  {
    digitalWrite(cooler,HIGH);
    digitalWrite(heater,LOW);
  }
  else if(temp > high)
  {
    digitalWrite(cooler,LOW);
    digitalWrite(heater,HIGH);
  }
  else{
    digitalWrite(cooler,HIGH);
    digitalWrite(heater,HIGH);
  }
}
////////////////////////////////////  RTC library   //////////////////////////////////////////////////////////
RTC_DS3231 rtc;

char t[32];
const int feedtime[] = {2159,2200,2201,2216,2218};
bool feedcounter[] = {false,false,false,false,false};

void  CurrentTime()
{
  DateTime now = rtc.now();

  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  
  Serial.print(F("Date/Time: "));
  Serial.println(t);
  
}
void FeedTime()
{
  DateTime now = rtc.now();
  int cur_time = (now.hour()*100)+ now.minute();
  for(int i = 0; i<sizeof(feedtime);i++)
  {
    if(cur_time == feedtime[i] && !feedcounter[i])
    {
      Serial.println("Feed Time!");
      feedcounter[i] = true;
      
      break;
      
    }
    if(cur_time == feedtime[i] && feedcounter[i])
    {
      
      Serial.println("Check Delay!");
      break;
      
    }
    
    else if(cur_time != feedtime[i] && feedcounter[i])
    {
      feedcounter[i] = false;
      break;
    }
    
    
  }
}

/////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  
  pinMode(cooler,OUTPUT);
  pinMode(heater,OUTPUT);
  digitalWrite(cooler,HIGH);
  digitalWrite(heater,HIGH);

  Wire.begin();

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  

  

}

void loop() {
  // put your main code here, to run repeatedly:
  double temp = Temperature();
  TempControl(temp,temp_low,temp_high);
  CurrentTime();
  FeedTime();
  delay(1000);
  
  
  
  

}
