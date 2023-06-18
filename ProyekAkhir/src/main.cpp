#define BLYNK_TEMPLATE_ID "TMPL6pdX-iLuA"
#define BLYNK_TEMPLATE_NAME "AQ Detector"
#define BLYNK_AUTH_TOKEN "SIUH659_pjS3_L9ysLLK3VesKHJSCXSA"

#include <Blynk.h>
#include <DHT.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <Preferences.h>
#include <BlynkSimpleEsp32.h>
#include "MQ135.h"

    char auth[] = BLYNK_AUTH_TOKEN;

#define MQ135_PIN 34   // Pin ADC untuk sensor MQ-135
#define MQ2_PIN 35     // Pin ADC untuk sensor MQ-2
#define DHT_PIN 2      // Pin data sensor DHT22 terhubung ke GPIO 4
#define DHT_TYPE DHT22 // Tipe sensor DHT22, ganti menjadi DHT11 jika menggunakan sensor DHT11
#define RED_PIN 18     // Pin GPIO yang terhubung ke kaki merah LED RGB
#define GREEN_PIN 19   // Pin GPIO yang terhubung ke kaki hijau LED RGB
#define BLUE_PIN 21    // Pin GPIO yang terhubung ke kaki biru LED RGB

const int buzzerPin = 12;

// Change the virtual pins according the rooms
#define VPIN_Temperature V0
#define VPIN_Humidity V1
#define VPIN_AirQuality V2
#define VPIN_SmokeDetector V3
#define VPIN_Judul V4
#define VPIN_Data V5
#define VPIN_DataLevel V6
#define VPIN_Red V7
#define VPIN_Yellow V8
#define VPIN_Green V9

float temperature = 0;
float humidity = 0;
float airquality = 0;
float smoke = 0;
int red, green, yellow = 0;

BlynkTimer timer;
DHT dht(DHT_PIN, DHT_TYPE);

void checkBlynkStatus()
{ // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false)
  {
    Serial.println("Blynk Not Connected");
  }
  if (isconnected == true)
  {
    Serial.println(" Blynk IoT Connected ");
    Blynk.virtualWrite(VPIN_Judul, "Monitoring Air Quality and Smoke Emissions at Urban Bus Stops");
  }
  // display.display();
  delay(1000);
}

void notifikasi(int temperature, int PPM)
{

  if (temperature < 35 && PPM < 1300)
  {
    Serial.println("Aman");
    Blynk.virtualWrite(VPIN_Data, "OFF");
    digitalWrite(buzzerPin, LOW); // no tone
    Serial.println("Buzzer OFF");
  }
  else if ((temperature >= 36 && temperature < 45) || (PPM >= 1300 && PPM < 1500))
  {
    Serial.print("Siaga");
    Serial.print("Buzzer ON");
    Blynk.virtualWrite(VPIN_Data, "ON"); // buzzer on
    digitalWrite(buzzerPin, HIGH);       // send tone
    delay(1000);
    digitalWrite(buzzerPin, LOW); // no tone
    delay(1000);
    digitalWrite(buzzerPin, HIGH); // send tone
    delay(1000);
    digitalWrite(buzzerPin, LOW); // no tone
    delay(1000);
  }
  else if (temperature >= 45 && PPM >= 1500)
  {
    Serial.print("Bahaya");
    Serial.print("Buzzer ON");
    Blynk.virtualWrite(VPIN_Data, "ON");
    digitalWrite(buzzerPin, HIGH); // send tone
    delay(3000);
  }
}

void smokedetector()
{
  float temperature = dht.readTemperature();
  int sensorValue = analogRead(MQ2_PIN); // Membaca nilai dari sensor MQ-2
  int smokePPM = map(sensorValue, 0, 1023, 0, 1000);
  Serial.print("(MQ2)Konsentrasi Asap : ");
  Serial.print(smokePPM);
  Serial.println(" PPM");
  notifikasi(temperature, smokePPM);
  Blynk.virtualWrite(VPIN_SmokeDetector, smokePPM);
}

void readdht22()
{
  float temperature = dht.readTemperature(); // Membaca suhu dalam derajat Celsius
  float humidity = dht.readHumidity();       // Membaca kelembaban dalam persen

  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.println(" %");

  Blynk.virtualWrite(VPIN_Temperature, temperature);
  Blynk.virtualWrite(VPIN_Humidity, humidity);
}

void lamprgb(int led)
{
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);

  if (led >= 0 && led <= 50)
  {
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
    Blynk.virtualWrite(VPIN_Green, 1);
    Blynk.virtualWrite(VPIN_Yellow, 0);
    Blynk.virtualWrite(VPIN_Red, 0);
  }
  else if (led >= 51 && led <= 150)
  {
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(RED_PIN, HIGH); // kuning
    Blynk.virtualWrite(VPIN_Green, 0);
    Blynk.virtualWrite(VPIN_Yellow, 1);
    Blynk.virtualWrite(VPIN_Red, 0);
  }
  else if (led >= 151)
  {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    Blynk.virtualWrite(VPIN_Green, 0);
    Blynk.virtualWrite(VPIN_Yellow, 0);
    Blynk.virtualWrite(VPIN_Red, 1);
  }
}

void indexquality(int Air_index)
{
  String quality;

  if (Air_index >= 0 && Air_index <= 50)
  {
    quality = "Good";
  }
  else if (Air_index >= 51 && Air_index <= 100)
  {
    quality = "Moderate";
  }
  else if (Air_index >= 101 && Air_index <= 150)
  {
    quality = "Unhealthy for Sensitive Groups";
  }
  else if (Air_index >= 151 && Air_index <= 200)
  {
    quality = "Unhealthy";
  }
  else if (Air_index >= 201 && Air_index <= 300)
  {
    quality = "Very Unhealthy";
  }
  else if (Air_index > 300)
  {
    quality = "Hazardous";
  }
  Serial.print("AQI : ");
  Serial.print(quality);
  Serial.println();
  Blynk.virtualWrite(VPIN_DataLevel, quality);
}

void qualityair()
{
  MQ135 gasSensor = MQ135(34);
  float air_quality = gasSensor.getPPM();

  int value = analogRead(34);
  Serial.print("Air Quality: ");
  Serial.print(air_quality);
  Serial.println(" PPM");
  Serial.print("Air Quality 2: ");
  Serial.print(value, DEC);
  Serial.println(" PPM");

  indexquality(air_quality);
  lamprgb(air_quality);
  Blynk.virtualWrite(VPIN_AirQuality, air_quality);
}

void setup()
{
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  // cobain pake wifi manager
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("Reconnect", "12345678"); // password protected ap
  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    Serial.print("connected...yeey:)");
  }

  pinMode(MQ135_PIN, INPUT);  // Mengatur pin sensor MQ-135 sebagai input
  pinMode(MQ2_PIN, INPUT);    // Mengatur pin sensor MQ-2 sebagai input
  dht.begin();                // Inisialisasi sensor DHT22
  pinMode(RED_PIN, OUTPUT);   // Mengatur pin merah sebagai output
  pinMode(GREEN_PIN, OUTPUT); // Mengatur pin hijau sebagai output
  pinMode(BLUE_PIN, OUTPUT);  // Mengatur pin biru sebagai output

  timer.setInterval(2000L, checkBlynkStatus);
  timer.setInterval(1000L, readdht22);
  timer.setInterval(1000L, qualityair);
  timer.setInterval(1000L, smokedetector);
  Blynk.config(auth);
}

void loop()
{
  Blynk.run();
  timer.run();
  readdht22();     // sudah normal
  qualityair();    // sudah normal
  smokedetector(); // butuh analisa lagi
  Serial.println(" ");
  Serial.println("=================================");
  delay(2000);
}