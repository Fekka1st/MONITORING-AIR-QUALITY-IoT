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
#define BUZZER_PIN 22  // pin Buzzer

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
  int sensorValue = analogRead(MQ135_PIN); // Membaca nilai dari sensor MQ-135

  // Menghitung konsentrasi O2 dan CO2 dalam ppm berdasarkan tegangan yang diberikan sensor
  float voltage = sensorValue * (5.0 / 1023.0);        // Mengubah nilai ADC menjadi tegangan (0-5V)
  float O2_ppm = 20.9 - ((voltage - 0.1) * 9.0 / 0.8); // Menghitung konsentrasi O2 dalam ppm (asumsi respons linier)
  float CO2_ppm = 5000 * (voltage / 5.0);              // Menghitung konsentrasi CO2 dalam ppm (asumsi respons linier)

  Serial.print("(MQ-135)Konsentrasi O2 (ppm): ");
  Serial.println(O2_ppm);
  Serial.print("(MQ-135)Konsentrasi CO2 (ppm): ");
  Serial.println(CO2_ppm);

  delay(2000);
}

void readmq2()
{

  int sensorValue = analogRead(MQ2_PIN); // Membaca nilai dari sensor MQ-2

  // Menghitung estimasi konsentrasi gas dalam ppm
  float RS_air = 10000.0;                              // Resistansi sensor di udara bersih (ohm)
  float sensorVoltage = sensorValue * (5.0 / 1023.0);  // Mengubah nilai ADC menjadi tegangan (0-5V)
  float RS_gas = ((5.0 / sensorVoltage) - 1) * RS_air; // Resistansi sensor saat terkena gas (ohm)
  float ratio = RS_gas / RS_air;                       // Rasio resistansi
  float ppm = pow(10, ((log10(ratio) - 2.63) / 1.27)); // Menghitung ppm menggunakan kurva kalibrasi

  Serial.print("(MQ-2)Konsentrasi Gas: ");
  Serial.print(ppm);
  Serial.println(" ppm");
}

void loop()
{
  qualityair();
  readmq2();
  readdht22();

  Serial.println(" ");
  Serial.println("=================================");
}
