#include <Arduino.h>
#include <DHT.h>

#define RELAY_PIN 12   // Pin relay terhubung ke GPIO 12
#define MQ135_PIN A0   // Pin ADC untuk sensor MQ-135
#define MQ2_PIN A1     // Pin ADC untuk sensor MQ-2
#define DHT_PIN 4      // Pin data sensor DHT22 terhubung ke GPIO 4
#define DHT_TYPE DHT22 // Tipe sensor DHT22, ganti menjadi DHT11 jika menggunakan sensor DHT11
#define RED_PIN 12     // Pin GPIO yang terhubung ke kaki merah LED RGB
#define GREEN_PIN 13   // Pin GPIO yang terhubung ke kaki hijau LED RGB
#define BLUE_PIN 14    // Pin GPIO yang terhubung ke kaki biru LED RGB

DHT dht(DHT_PIN, DHT_TYPE);

void readmq()
{
  int mq135Value = analogRead(MQ135_PIN); // Membaca nilai dari sensor MQ-135
  int mq2Value = analogRead(MQ2_PIN);     // Membaca nilai dari sensor MQ-2

  Serial.print("MQ-135: ");
  Serial.println(mq135Value);

  Serial.print("MQ-2: ");
  Serial.println(mq2Value);

  delay(1000); // Jeda 1 detik sebelum membaca nilai sensor lagi
}

void relay()
{
  digitalWrite(RELAY_PIN, HIGH); // Menghidupkan relay
  delay(2000);                   // Menunggu selama 2 detik
  digitalWrite(RELAY_PIN, LOW);  // Mematikan relay
  delay(2000);                   // Menunggu selama 2 detik
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

void lamprgb()
{
  // Merah
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  delay(1000);

  // Hijau
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
  delay(1000);

  // Biru
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH);
  delay(1000);
}

int myFunction(int, int);
void setup()
{

  int result = myFunction(2, 3);
  pinMode(RELAY_PIN, OUTPUT); // Mengatur pin relay sebagai output
  pinMode(MQ135_PIN, INPUT);  // Mengatur pin sensor MQ-135 sebagai input
  pinMode(MQ2_PIN, INPUT);    // Mengatur pin sensor MQ-2 sebagai input
  dht.begin();                // Inisialisasi sensor DHT22
  pinMode(RED_PIN, OUTPUT);   // Mengatur pin merah sebagai output
  pinMode(GREEN_PIN, OUTPUT); // Mengatur pin hijau sebagai output
  pinMode(BLUE_PIN, OUTPUT);  // Mengatur pin biru sebagai output
}

void loop()
{
}

int myFunction(int x, int y)
{
  return x + y;
}