#include"dht.h"                      // Including library for dht
#include<LiquidCrystal.h>
LiquidCrystal lcd(8,9,4,5,6,3);
#include<Timer.h>
Timer t;
#include <SoftwareSerial.h>
#define RX 10             // Arduino RX -> ESP8266 TX
#define TX 11
SoftwareSerial esp8266(RX, TX);
#define dht_dpin 2 
#define Buzzer 12
#define heart 13
#define FAN_PIN 7 
dht DHT;
char *api_key="GR5GCIX16W4TO6X1";     // Enter your Write API key from ThingSpeak
static char postUrl[150];
int humi,tem;
void httpGet(String ip, String path, int port=80);
void setup()
{
 lcd.begin(16, 2);
 pinMode(FAN_PIN, OUTPUT);
 pinMode(Buzzer, OUTPUT);
 digitalWrite(FAN_PIN, LOW);
 lcd.clear();
 lcd.print("   Humidity   ");
 lcd.setCursor(0,1);
 lcd.print("  Measurement ");
 delay(2000);
 lcd.clear();
 lcd.print("Circuit Digest ");
 lcd.setCursor(0,1);
 lcd.print("Welcomes You");
 delay(2000);
 esp8266.begin(115200);
 Serial.begin(9600);
 lcd.clear();
 lcd.print("WIFI Connecting");
 lcd.setCursor(0,1);
 lcd.print("Please wait....");
 Serial.println("Connecting Wifi....");
 connect_wifi("AT",1000);
 connect_wifi("AT+CWMODE=1",1000);
 connect_wifi("AT+CWQAP",1000);  
 connect_wifi("AT+RST",5000);
 connect_wifi("AT+CWJAP=\"1st floor\",\"muda1884\"",10000);
 Serial.println("Wifi Connected"); 
 lcd.clear();
 lcd.print("WIFI Connected.");
 pinMode(heart, OUTPUT);
 delay(2000);
 t.oscillate(heart, 1000, LOW);
 t.every(20000, send2server);
}
void loop()
{
  DHT.read11(dht_dpin);
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  humi=DHT.humidity;
  lcd.print(humi);   // printing Humidity on LCD
  lcd.print(" %    ");
  lcd.setCursor(0,1);
  lcd.print("Temperature:");
  tem=DHT.temperature;
  lcd.print(tem);   // Printing temperature on LCD
  lcd.write(1);
  lcd.print("C   ");
  delay(1000);
  t.update();
  // Fan control (threshold = 30 °C)
  if (tem >30) {
    digitalWrite(FAN_PIN, HIGH);
  } else {
    digitalWrite(FAN_PIN, LOW);
  }
  if (tem >100) {
    digitalWrite(Buzzer, HIGH);
  } else {
    digitalWrite(Buzzer, LOW);
  }
}
void send2server()
{
  char tempStr[8];
  char humidStr[8];
  dtostrf(tem, 5, 3, tempStr);
  dtostrf(humi, 5, 3, humidStr);
  sprintf(postUrl, "update?api_key=GR5GCIX16W4TO6X1&field1=0&field2=0",api_key,humidStr,tempStr);
  httpGet("api.thingspeak.com", postUrl, 80);
}
void httpGet(String ip, String path, int port)
{
  int resp;
  String atHttpGetCmd = "GET /"+path+" HTTP/1.0\r\n\r\n";
  String atTcpPortConnectCmd = "AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"";
  connect_wifi(atTcpPortConnectCmd,1000);
  int len = atHttpGetCmd.length();
  String atSendCmd = "AT+CIPSEND=";
  atSendCmd+=len;
  connect_wifi(atSendCmd,1000);
  connect_wifi(atHttpGetCmd,1000);
}
void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    lcd.clear();
    lcd.print(cmd);
    Serial.println(cmd);
    esp8266.print(cmd); 
    while(esp8266.available())
    {
      if(esp8266.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  {
   Serial.println("OK");
        lcd.setCursor(0,1);
      lcd.print("OK");
  }
  else
  {
   Serial.println("Error");
         lcd.setCursor(0,1);
      lcd.print("Error");
  }
}