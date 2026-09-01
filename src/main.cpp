// --- Configurari BLYNK (trebuie sa fie mereu primele) ---
// Datele private (token Blynk + credentiale Wi-Fi) stau in include/secrets.h,
// fisier ignorat de Git. Vezi include/secrets.h.example pentru sablon.
#include "secrets.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// --- Wi-Fi ---
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

// ---  Pini ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define BATT_PIN 34

// --- Deep Sleep ---
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  300 // 5 minute

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- Sistemul s-a trezit ---");

  // Conectare la WiFi și Blynk
  Serial.println("Ma conectez la WiFi si Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  dht.begin();
  delay(2000); // 2 secunde pentru citire

  // Citire Senzori
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int rawADC = analogRead(BATT_PIN);
  float voltage = (rawADC / 4095.0) * 3.3 * 2.0;

  // Calibrare
  voltage = voltage + 0.35;

  int batteryPercent = map(voltage * 100, 330, 420, 0, 100);
  if (batteryPercent > 100) batteryPercent = 100;
  if (batteryPercent < 0) batteryPercent = 0;

  Serial.println("Date citite. Trimit catre platforma...");

  // Trimitere date pe "datastreams" Blynk
  if (!isnan(temperature) && !isnan(humidity)) {
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
  }
  Blynk.virtualWrite(V2, voltage);
  Blynk.virtualWrite(V3, batteryPercent);

  // conexiune activa scurt timp
  Blynk.run();
  delay(500);

  // Deconectare si somn
  Blynk.disconnect();
  WiFi.disconnect();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Date trimise! Intru in Deep Sleep... Zzz");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // gol
}
