#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <math.h>  // Para log()

// WiFi
const char* ssid = "Visitantes";
const char* password = "";

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQ135
#define MQ135_PIN 34

// BMP280
Adafruit_BMP280 bmp;

// Web Server
WebServer server(80);

// Rota para página HTML
void handleRoot() {
  String page = R"rawliteral(
  <!DOCTYPE html>
  <html lang="pt-BR">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Estação Meteorológica</title>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@300;500;700&display=swap" rel="stylesheet">
    <style>
      body {
        margin: 0;
        font-family: 'Poppins', sans-serif;
        background: linear-gradient(to right, #e0f7ff, #ffffff);
        color: #003366;
      }
      header {
        background-color: #0077cc;
        color: white;
        padding: 1rem 2rem;
        text-align: center;
        box-shadow: 0 2px 5px rgba(0,0,0,0.1);
      }
      .container {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
        padding: 2rem;
        gap: 1.5rem;
      }
      .card {
        background-color: white;
        border-radius: 15px;
        box-shadow: 0 4px 10px rgba(0,0,0,0.1);
        padding: 1.5rem;
        width: 250px;
        text-align: center;
        transition: transform 0.2s ease-in-out;
      }
      .card:hover {
        transform: translateY(-5px);
      }
      .card h2 {
        margin: 0.5rem 0;
        color: #0077cc;
      }
      .icon {
        font-size: 2.5rem;
        color: #0077cc;
      }
      footer {
        text-align: center;
        padding: 1rem;
        color: #666;
        margin-top: 2rem;
      }
    </style>
  </head>
  <body>
    <header>
      <h1>Estação Meteorológica</h1>
      <p>Dados atualizados em tempo real</p>
    </header>
    <div class="container">
      <div class="card">
        <div class="icon">🌡️</div>
        <h2>Temperatura</h2>
        <p id="tempDHT">-- °C</p>
      </div>
      <div class="card">
        <div class="icon">💧</div>
        <h2>Umidade</h2>
        <p id="hum">-- %</p>
      </div>
      <div class="card">
        <div class="icon">🧭</div>
        <h2>Pressão</h2>
        <p id="pressure">-- hPa</p>
      </div>
      <div class="card">
        <div class="icon">🌫️</div>
        <h2>Qualidade do Ar</h2>
        <p id="mq135">--</p>
      </div>
      <div class="card">
        <div class="icon">❄️</div>
        <h2>Ponto de Orvalho</h2>
        <p id="dewPoint">-- °C</p>
      </div>
      <div class="card">
        <div class="icon">🔥</div>
        <h2>Índice de Calor</h2>
        <p id="heatIndex">-- °C</p>
      </div>
      <div class="card">
        <div class="icon">📉</div>
        <h2>Gráficos Históricos</h2>
        <p><em>Em breve</em></p>
      </div>
      <div class="card">
        <div class="icon">⚠️</div>
        <h2>Alertas</h2>
        <p><em>Sem alertas</em></p>
      </div>
    </div>
    <footer>
      &copy; 2025 Estação Meteorológica - Todos os direitos reservados
    </footer>

    <script>
      async function updateData() {
        try {
          const response = await fetch('/data');
          if (!response.ok) throw new Error('Erro na rede');
          const data = await response.json();

          document.getElementById('tempDHT').textContent = isNaN(data.tempDHT) ? '-- °C' : data.tempDHT.toFixed(1) + ' °C';
          document.getElementById('hum').textContent = isNaN(data.hum) ? '-- %' : data.hum.toFixed(1) + ' %';
          document.getElementById('pressure').textContent = isNaN(data.pressure) ? '-- hPa' : data.pressure.toFixed(1) + ' hPa';
          document.getElementById('mq135').textContent = data.mq135;
          document.getElementById('dewPoint').textContent = isNaN(data.dewPoint) ? '-- °C' : data.dewPoint.toFixed(1) + ' °C';
          document.getElementById('heatIndex').textContent = isNaN(data.heatIndex) ? '-- °C' : data.heatIndex.toFixed(1) + ' °C';
        } catch (error) {
          console.error('Erro ao atualizar dados:', error);
        }
      }

      setInterval(updateData, 5000);
      window.onload = updateData;
    </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", page);
}

// Rota para enviar dados em JSON
void handleData() {
  float tempDHT = dht.readTemperature();
  float hum = dht.readHumidity();
  float tempBMP = NAN;
  float pressure = NAN;
  float dewPoint = NAN;
  float heatIndex = NAN;

  if (bmp.begin(0x77)) {
    tempBMP = bmp.readTemperature();
    pressure = bmp.readPressure() / 100.0F;
  }

  int mq135Value = analogRead(MQ135_PIN);

  if (!isnan(tempDHT) && !isnan(hum)) {
    double a = 17.27;
    double b = 237.7;
    double alpha = ((a * tempDHT) / (b + tempDHT)) + log(hum / 100.0);
    dewPoint = (b * alpha) / (a - alpha);
    heatIndex = dht.computeHeatIndex(tempDHT, hum, false);
  }

  String json = "{";
  json += "\"tempDHT\":" + String(tempDHT, 1) + ",";
  json += "\"hum\":" + String(hum, 1) + ",";
  json += "\"pressure\":" + String(pressure, 1) + ",";
  json += "\"mq135\":" + String(mq135Value) + ",";
  json += "\"dewPoint\":" + String(dewPoint, 1) + ",";
  json += "\"heatIndex\":" + String(heatIndex, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  if (!bmp.begin(0x77)) {
    Serial.println("Não foi possível iniciar BMP280");
  }

  WiFi.begin(ssid, password);
  Serial.print("Conectando-se à rede WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado! Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
