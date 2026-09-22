#include <WiFiS3.h>

const char* ssid = "Totalplay999_2.4G";
const char* password = "huajolote22";
const int ledPin = 13;
bool ledState = false;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 5000) {
    delay(10);
  }

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  while (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Servidor listo en: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String request = "";      // primera línea (GET /algo HTTP/1.1)
  String currentLine = "";
  bool firstLineDone = false;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();

      if (!firstLineDone) {
        request += c;
        if (c == '\n') firstLineDone = true;
      }

      if (c == '\n') {
        if (currentLine.length() == 0) break; // línea en blanco = fin de los headers
        currentLine = "";
      } else if (c != '\r') {
        currentLine += c;
      }
    }
  }

  if (request.indexOf("GET /led/on") >= 0) {
    ledState = true;
    digitalWrite(ledPin, HIGH);
  } else if (request.indexOf("GET /led/off") >= 0) {
    ledState = false;
    digitalWrite(ledPin, LOW);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<style>body{font-family:sans-serif;text-align:center;margin-top:50px;}");
  client.println("button{font-size:20px;padding:15px 30px;margin:10px;}</style></head><body>");
  client.println("<h1>Control de LED</h1>");
  client.print("<p>Estado actual: <b>");
  client.print(ledState ? "ENCENDIDO" : "APAGADO");
  client.println("</b></p>");
  client.println("<a href='/led/on'><button>Encender</button></a>");
  client.println("<a href='/led/off'><button>Apagar</button></a>");
  client.println("</body></html>");

  delay(1);
  client.stop();
}
