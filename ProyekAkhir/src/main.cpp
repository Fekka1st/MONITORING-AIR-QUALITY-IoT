#include <Arduino.h>
#include <DHT.h>
#include <WiFiManager.h>
#include <MQ2.h>

#define RELAY_PIN 4    // Pin relay terhubung ke GPIO 12
#define MQ135_PIN 34   // Pin ADC untuk sensor MQ-135
#define MQ2_PIN 35     // Pin ADC untuk sensor MQ-2
#define DHT_PIN 2      // Pin data sensor DHT22 terhubung ke GPIO 4
#define DHT_TYPE DHT22 // Tipe sensor DHT22, ganti menjadi DHT11 jika menggunakan sensor DHT11
#define RED_PIN 21     // Pin GPIO yang terhubung ke kaki merah LED RGB
#define GREEN_PIN 19   // Pin GPIO yang terhubung ke kaki hijau LED RGB
#define BLUE_PIN 18    // Pin GPIO yang terhubung ke kaki biru LED RGB
#define BUZZER_PIN 22  // pin Buzzer

#define MQ135_THRESHOLD_1 1000
#define MQ2_Threshold 400

int lpg_gas, co_gas, smoke_gas;
MQ2 mq2(MQ2_PIN);

DHT dht(DHT_PIN, DHT_TYPE);

void relay(int nilai)
{
  if (nilai == 1)
  {
    Serial.println("Kipas Menyala");
    digitalWrite(RELAY_PIN, HIGH); // Menghidupkan relay
    delay(2000);                   // Menunggu selama 2 detik
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW); // Mematikan relay
    delay(2000);                  // Menunggu selama 2 detik
  }
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
}

void lamprgb(int led)
{

  if (led == 1) // status bahaya
  {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
  else if (led == 2) // status aman
  {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  }
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
    Serial.print("connected...yeey : :)");
    Serial.println(wm.localIP());
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT); // Mengatur pin relay sebagai output
  pinMode(MQ135_PIN, INPUT);  // Mengatur pin sensor MQ-135 sebagai input
  pinMode(MQ2_PIN, INPUT);    // Mengatur pin sensor MQ-2 sebagai input
  dht.begin();                // Inisialisasi sensor DHT22
  pinMode(RED_PIN, OUTPUT);   // Mengatur pin merah sebagai output
  pinMode(GREEN_PIN, OUTPUT); // Mengatur pin hijau sebagai output
  pinMode(BLUE_PIN, OUTPUT);  // Mengatur pin biru sebagai output
  digitalWrite(GREEN_PIN, HIGH);
  noTone(BUZZER_PIN);
}

void qualityair()
{
  int MQ135_data = analogRead(MQ135_PIN);
  if (MQ135_data < MQ135_THRESHOLD_1)
  {
    Serial.print(“Fresh Air
                 : “);
  }
  else
  {
    Serial.print(“Poor Air
                 : “);
  }
  Serial.print(MQ135_data); // analog data
  Serial.println(" PPM");   // Unit = part per million
  delay(2000);
}

void readmq2()
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
  delay(1000);

  // Check whether it is greater than the threshold value
  if (analogSensor > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    tone(buzzer, 1000, 200);
  }
  else
  {
    digitalWrite(redLed, LOW);
    noTone(buzzer);
  }
  delay(1000); // Delay 1 second for next reading
}

void loop()
{
  qualityair();
  readmq2();
  readdht22();

  Serial.println(" ");
  Serial.println("=================================");
}
