#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

String apiKey = "M9CZDTWH78YX3RU9";

const char *ssid =  "SmartIrrigation";
const char *pass =  "Smart123";

const char* server = "api.thingspeak.com";

#define DHTPIN 12
#define SoilPin A0
#define led 2
#define relay 14

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  pinMode(SoilPin, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.print("Smart Irrigation");
  lcd.setCursor(0, 1);
  lcd.print("     System     ");
  delay(2000);
  lcd.clear();
  lcd.print("    using    ");
  lcd.setCursor(0, 1);
  lcd.print("IOT & ThingSpeak");
  delay(1500);
  Serial.println("Connecting to Internet");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  lcd.clear();
  lcd.print("Connecting to");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    Serial.println("Wait... ");
  }
  Serial.println("WiFi Connected.... ");
  lcd.clear();
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
  dht.begin();
}

void loop()
{
  int temp, hum, water;

  float t = dht.readTemperature();
  temp = t;
  float h = dht.readHumidity();
  hum = h;
  water = analogRead(SoilPin);
  water = map(water, 0, 1024, 0, 100);
  delay(300);
  lcd.clear();  lcd.print("T: ");  lcd.print(temp);
  lcd.setCursor(8, 0);  lcd.print("H: ");  lcd.print(hum);
  lcd.setCursor(0, 1);  lcd.print("Water: ");   lcd.print(water);
  delay(1000);

  if (water > 80)
  {
    digitalWrite(relay, LOW);
    Serial.println("Water Level Low! Motor ON");
    delay(1000);
  }
  if (water < 40)
  {
    digitalWrite(relay, HIGH);
    Serial.println("Water Level High! Motor OFF");
    delay(1000);
  }

  lcd.clear();  lcd.print("Uploading... ");
  delay(1000);

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(water);
    postStr += "\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("   Data Sent    ");
    lcd.setCursor(0, 1);  lcd.print("-- Data Sent -- ");
    delay(1000);
    delay(500);
  }
  else
  {
    Serial.println("Data not Upload");
    lcd.setCursor(0, 1);  lcd.print("-- Data Error --");
    delay(500);
  }
  client.stop();
  delay(1000);
}