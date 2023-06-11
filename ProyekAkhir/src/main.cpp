#include <Arduino.h>
#include <DHT.h>

#define RELAY_PIN 4    // Pin relay terhubung ke GPIO 12
#define MQ135_PIN 34   // Pin ADC untuk sensor MQ-135
#define MQ2_PIN 35     // Pin ADC untuk sensor MQ-2
#define DHT_PIN 2      // Pin data sensor DHT22 terhubung ke GPIO 4
#define DHT_TYPE DHT22 // Tipe sensor DHT22, ganti menjadi DHT11 jika menggunakan sensor DHT11
#define RED_PIN 21     // Pin GPIO yang terhubung ke kaki merah LED RGB
#define GREEN_PIN 19   // Pin GPIO yang terhubung ke kaki hijau LED RGB
#define BLUE_PIN 18    // Pin GPIO yang terhubung ke kaki biru LED RGB

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
  pinMode(RELAY_PIN, OUTPUT); // Mengatur pin relay sebagai output
  pinMode(MQ135_PIN, INPUT);  // Mengatur pin sensor MQ-135 sebagai input
  pinMode(MQ2_PIN, INPUT);    // Mengatur pin sensor MQ-2 sebagai input
  dht.begin();                // Inisialisasi sensor DHT22
  pinMode(RED_PIN, OUTPUT);   // Mengatur pin merah sebagai output
  pinMode(GREEN_PIN, OUTPUT); // Mengatur pin hijau sebagai output
  pinMode(BLUE_PIN, OUTPUT);  // Mengatur pin biru sebagai output
  digitalWrite(GREEN_PIN, HIGH);
}

void qualityair()
{
  int mq135Value = analogRead(MQ135_PIN); // Membaca nilai dari sensor MQ-135

  float o2Concentration = map(mq135Value, 0, 1023, 0, 100);  // Mengkonversi nilai menjadi konsentrasi O2
  float co2Concentration = map(mq135Value, 0, 1023, 100, 0); // Mengkonversi nilai menjadi konsentrasi CO2

  Serial.print("Konsentrasi O2: ");
  Serial.print(o2Concentration);
  Serial.println(" %");

  Serial.print("Konsentrasi CO2: ");
  Serial.print(co2Concentration);
  Serial.println(" %");

  if (co2Concentration > 1000)
  {
    Serial.println("Peringatan: Konsentrasi CO2 tinggi");
    relay(1);   // menyalakan relay
    lamprgb(1); // led merah
  }
  else
  {
    relay(0);   // mematikan relay
    lamprgb(2); // led hijau
  }

  delay(2000);
}

void readmq2()
{
  int mq2Value = analogRead(MQ2_PIN);
  if (mq2Value > 100)
  {
    Serial.println("Deteksi Asap Rokok");
    lamprgb(1);
    // Lakukan tindakan atau tampilkan notifikasi sesuai kebutuhan Anda
  }
}

void loop()
{
  qualityair();
  readmq2();
  readdht22();
}
