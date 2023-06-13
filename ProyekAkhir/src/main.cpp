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
#include <MQ2.h>

char auth[] = BLYNK_AUTH_TOKEN;

#define RELAY_PIN 4    // Pin relay terhubung ke GPIO 12
#define MQ135_PIN 34   // Pin ADC untuk sensor MQ-135
#define MQ2_PIN 35     // Pin ADC untuk sensor MQ-2
#define DHT_PIN 2      // Pin data sensor DHT22 terhubung ke GPIO 4
#define DHT_TYPE DHT22 // Tipe sensor DHT22, ganti menjadi DHT11 jika menggunakan sensor DHT11
#define RED_PIN 21     // Pin GPIO yang terhubung ke kaki merah LED RGB
#define GREEN_PIN 19   // Pin GPIO yang terhubung ke kaki hijau LED RGB
#define BLUE_PIN 18    // Pin GPIO yang terhubung ke kaki biru LED RGB
#define BUZZER_PIN 22  // pin Buzzer

// threshold
#define MQ135_THRESHOLD_1 1000
#define MQ2_Threshold 400

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

int lpg_gas, co_gas, smoke_gas;
MQ2 mq2(MQ2_PIN);
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
    Blynk.virtualWrite(VPIN_Judul, "Controlling and Monitoring At bus Stop");
  }
  // display.display();
  delay(1000);
}

BLYNK_CONNECTED()
{
  // update the latest state to the server
  Blynk.virtualWrite(VPIN_Judul, "TEST");
  Blynk.syncVirtual(VPIN_Temperature, temperature);
  Blynk.syncVirtual(VPIN_Humidity, humidity);
  Blynk.syncVirtual(VPIN_AirQuality, airquality);
  Blynk.syncVirtual(VPIN_SmokeDetector, smoke);
  Blynk.syncVirtual(VPIN_Data, "USE LEVEL AQI");
  Blynk.syncVirtual(VPIN_DataLevel, "Fresh");
  Blynk.syncVirtual(VPIN_Red, red);
  Blynk.syncVirtual(VPIN_Yellow, yellow);
  Blynk.syncVirtual(VPIN_Green, green);
}

void smokedetector()
{

  int sensorValue = analogRead(MQ2_PIN); // Membaca nilai dari sensor MQ-2
  Serial.print("Output MQ-2 : ");
  Serial.println(sensorValue);

  float *values = mq2.read(true); // jika diset "false" tidak akan dimunculkan di serial monitor

  lpg_gas = mq2.readLPG();
  co_gas = mq2.readCO();
  smoke_gas = mq2.readSmoke();

  Serial.println("PENGUKURAN GAS/ASAP");
  Serial.print("LPG :");
  Serial.print(lpg_gas);
  Serial.println(" PPM");
  Serial.print("CO :");
  Serial.print(co_gas);
  Serial.println(" PPM");
  Serial.print("SMOKE:");
  Serial.print(smoke_gas);
  Serial.println(" PPM");

  Blynk.virtualWrite(VPIN_SmokeDetector, smoke_gas);
}

void readdht22()
{
  delay(2000); // Jeda 2 detik sebelum membaca sensor lagi

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
  if (led > 0 || led <= 50)
  {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
    Blynk.virtualWrite(VPIN_DataLevel, "Good");
    Blynk.virtualWrite(VPIN_Green, 1);
    Blynk.virtualWrite(VPIN_Red, 0);
    Blynk.virtualWrite(VPIN_Yellow, 0);
  }
  else if (led >= 51 || led <= 150)
  {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);

    Blynk.virtualWrite(VPIN_Yellow, 1);
    Blynk.virtualWrite(VPIN_Green, 0);
    Blynk.virtualWrite(VPIN_Red, 0);
  }
  else if (led >= 151)
  {

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    Blynk.virtualWrite(VPIN_Red, 1);
    Blynk.virtualWrite(VPIN_Yellow, 0);
    Blynk.virtualWrite(VPIN_Green, 0);
  }
}

void indexquality(int nilai)
{
  if (nilai > 0 || nilai <= 50)
  {
    Blynk.virtualWrite(VPIN_DataLevel, "Good");
  }
  else if (nilai >= 51 || nilai <= 100)
  {
    Blynk.virtualWrite(VPIN_DataLevel, "Moderate");
  }
  else if (nilai >= 101 || nilai <= 150)
  {
    Blynk.virtualWrite(VPIN_DataLevel, "Unhealty for sensitive Groups ");
  }
  else if (nilai >= 151 || nilai <= 200)
  {
    Blynk.virtualWrite(VPIN_DataLevel, "Unhealty");
  }
  else if (nilai >= 201 || nilai <= 300)
  {
    Blynk.virtualWrite(VPIN_DataLevel, "Verry Unhealty");
  }
  else if (nilai > 300)
  {
    Blynk.virtualWrite(VPIN_DataLevel, "Hazardous");
  }
}

void qualityair()
{
  MQ135 gasSensor = MQ135(34);
  float air_quality = gasSensor.getPPM();
  Serial.print("Air Quality: ");
  Serial.print(air_quality);
  Serial.println("  PPM");
  Serial.println();
  lamprgb(air_quality);
  indexquality(air_quality);
}

void setup()
{
  Serial.begin(115200);
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

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT); // Mengatur pin relay sebagai output
  pinMode(MQ135_PIN, INPUT);  // Mengatur pin sensor MQ-135 sebagai input
  pinMode(MQ2_PIN, INPUT);    // Mengatur pin sensor MQ-2 sebagai input
  dht.begin();                // Inisialisasi sensor DHT22
  pinMode(RED_PIN, OUTPUT);   // Mengatur pin merah sebagai output
  pinMode(GREEN_PIN, OUTPUT); // Mengatur pin hijau sebagai output
  pinMode(BLUE_PIN, OUTPUT);  // Mengatur pin biru sebagai output

  timer.setInterval(2000L, checkBlynkStatus);
  timer.setInterval(1000L, readdht22);
  timer.setInterval(1000L, qualityair);
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
}
