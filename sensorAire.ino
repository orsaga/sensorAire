#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------- CONFIGURACIONES --------
#define MQ135_PIN 34
#define BUZZER_PIN 4

const char* ssid     = "ESP32_AIRE";
const char* password = "12345678";

Adafruit_SSD1306 display(128, 32, &Wire);

// Servidor web
WebServer server(80);

// ------- HTML -------
String htmlPage() {
  int valor = analogRead(MQ135_PIN);
  //String calidad = (valor < 1000) ? "Buena" : (valor < 1250) ? "Regular" : "Mala";
  String calidad = (valor < 1500) ? "Buena" : (valor < 2500) ? "Regular" : "Mala";

  String page = R"(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="utf-8">
    <title>Calidad del Aire</title>
    <style>
      body { font-family: Arial; text-align: center; background:#eef; padding:20px; }
      .card { background:white; padding:20px; border-radius:12px; box-shadow:0 0 10px #999; width:80%; margin:auto; }
      .value { font-size: 2rem; margin-top:10px; }
      .good { color:green; }
      .mid { color:orange; }
      .bad { color:red; }
    </style>
  </head>
  <body>
    <h2>🌬 Sistema Calidad de Aire</h2>
    <div class="card">
      <p>Valor MQ-135:</p>
      <p class="value">)" + String(valor) + R"(</p>
      <p>Calidad:</p>
      <p class="value )";

  if (valor < 1500) page += "good";
  else if (valor < 2500) page += "mid";
  else page += "bad";

  page += R"(">)" + calidad + R"(</p>
    </div>
  </body>
  </html>
  )";

  return page;
}

// ----------- MANEJADORES ----------
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Iniciar OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED no encontrado");
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP iniciada, IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Sistema Aire");
  display.println("Conectado a:");
  display.println(ssid);
  display.display();
  delay(2000);
}

void loop() {
  server.handleClient();

  int valor = analogRead(MQ135_PIN);

  // Mostrar en la pantalla
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Calidad del aire:");

  if (valor < 1500) {
  //if (valor < 1000) {
    display.println("Buena");
    digitalWrite(BUZZER_PIN, LOW);
  } else if (valor < 2500) {
  //} else if (valor < 1240) {
    display.println("Regular");
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    display.println("MALA!");
    digitalWrite(BUZZER_PIN, HIGH); // alarma
  }

  display.setCursor(0,20);
  display.print("ADC: ");
  display.print(valor);
  display.display();

  delay(500);
}
