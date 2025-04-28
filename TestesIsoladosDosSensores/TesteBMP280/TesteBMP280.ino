#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Cria o objeto BMP280
Adafruit_BMP280 bmp; // usa I2C

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicia o BMP280 no endereço padrão 0x76
  if (!bmp.begin(0x77)) {
    Serial.println("Erro ao encontrar o BMP280! Verifique as conexões.");
    while (1);
  }

  Serial.println("BMP280 iniciado com sucesso!");
}

void loop() {
  float temperatura = bmp.readTemperature();
  float pressao = bmp.readPressure() / 100.0F; // Converte Pa para hPa
  float altitude = bmp.readAltitude(1013.25);  // 1013.25 = pressão ao nível do mar (ajuste conforme necessário)

  Serial.println("Leitura do BMP280:");
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Pressão: ");
  Serial.print(pressao);
  Serial.println(" hPa");

  Serial.print("Altitude estimada: ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.println("------------------------");
  delay(2000);
}