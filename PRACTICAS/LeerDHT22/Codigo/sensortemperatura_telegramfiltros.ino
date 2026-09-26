#include <DHT.h>
#include <WiFiS3.h>
#include "WiFiSSLClient.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Totalplay999_2.4G";
const char* password = "huajolote22";

const char* makeHost = "hook.us2.make.com";
const char* makePath = "/gojgot41fr6toj9e24wtgf793hnjcv6x";

const int ledPin = 7;              // LED de alerta de calor
const int ledCambioPin = 13;       // LED que avisa cambio de temperatura
const float TEMP_ALERTA = 32.0;
const int PASO_REPORTE = 1;

int ultimoBucketReportado = -1000;
bool alertaEnviada = false;

void setup() {
  Serial.begin(9600);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 5000) {
    delay(10);
  }

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(ledCambioPin, OUTPUT);
  digitalWrite(ledCambioPin, LOW);

  dht.begin();

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
  Serial.print("WiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(2000);

  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT22");
    return;
  }

  Serial.print("Temperatura actual: ");
  Serial.print(temperatura);
  Serial.println(" *C");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, intentando reconectar...");
    WiFi.begin(ssid, password);
    unsigned long intentoInicio = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - intentoInicio < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("No se pudo reconectar, se intentará de nuevo en el próximo ciclo.");
      return;
    }
    Serial.println("Reconectado.");
  }

  // --- Reporte cada PASO_REPORTE grados ---
  int bucketActual = (int)(temperatura / PASO_REPORTE) * PASO_REPORTE;
  if (bucketActual != ultimoBucketReportado) {
    ultimoBucketReportado = bucketActual;

    digitalWrite(ledCambioPin, HIGH);
    delay(2000);
    digitalWrite(ledCambioPin, LOW);

    const char* tipoReporte = (temperatura < TEMP_ALERTA) ? "fresco" : "actualizacion";

    Serial.print(">> Cambio de rango (");
    Serial.print(tipoReporte);
    Serial.print("): ");
    Serial.println(temperatura);
    enviarAMake(temperatura, humedad, tipoReporte);
  }

  // --- LED + alerta de calor a los 32 grados, y aviso al bajar de nuevo ---
  if (temperatura >= TEMP_ALERTA) {
    digitalWrite(ledPin, HIGH);
    if (!alertaEnviada) {
      alertaEnviada = true;
      Serial.println(">> ¡Hace calor! Enviando alerta a Telegram");
      enviarAMake(temperatura, humedad, "alerta_calor");
    }
  } else {
    digitalWrite(ledPin, LOW);
    if (alertaEnviada) {
      // Acaba de bajar de 32°: primera lectura ya por debajo del umbral
      Serial.println(">> Ya bajó de 32°, el cuarto está fresco. Avisando a Telegram");
      enviarAMake(temperatura, humedad, "alerta_fresco");
    }
    alertaEnviada = false;
  }
}

void enviarAMake(float temp, float hum, const char* tipo) {
  WiFiSSLClient client;

  if (!client.connect(makeHost, 443)) {
    Serial.println("No se pudo conectar a Make.com");
    return;
  }

  String url = String(makePath) + "?temperatura=" + String(temp) +
               "&humedad=" + String(hum) + "&tipo=" + String(tipo);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + makeHost + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 5000) {
    while (client.available()) {
      client.read();
      timeout = millis();
    }
  }
  client.stop();
}
