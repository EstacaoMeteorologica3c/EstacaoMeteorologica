#include "DHT.h"

#define DHTPIN 4        // Pino do ESP32 conectado ao DATA do DHT11
#define DHTTYPE DHT11   // <- Troquei aqui de DHT22 para DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Teste de Sensor DHT11 - ESP32");
}

void loop() {
  delay(2000); // DHT11 precisa de intervalo maior entre leituras

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Falha na leitura do sensor DHT11!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" %  |  Temperatura: ");
  Serial.print(t);
  Serial.println(" °C");
}