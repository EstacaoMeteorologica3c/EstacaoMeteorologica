#define MQ135_PIN 34 // Pino analógico do ESP32 conectado ao AOUT do MQ135

void setup() {
  Serial.begin(115200);
  Serial.println("Teste MQ135 - Qualidade do Ar");
}

void loop() {
  int analogValue = analogRead(MQ135_PIN); // Leitura bruta
  Serial.print("Valor do gás (bruto): ");
  Serial.println(analogValue);

  delay(2000);
}