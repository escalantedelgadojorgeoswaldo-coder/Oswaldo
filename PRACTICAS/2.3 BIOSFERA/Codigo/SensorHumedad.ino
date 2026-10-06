/*
  Práctica: Monitor de humedad del suelo
  Desarrollo Sustentable (ACD-0908) - Unidad 2: Escenario natural
  Tecnológico Nacional de México, campus Mazatlán

  Conexiones:
    Sensor VCC -> 5V
    Sensor GND -> GND
    Sensor SIG -> A0
    LED externo -> pin 13 -> resistencia 220 ohm -> LED (pata larga) -> GND
*/

const int PIN_SENSOR = A0;   // Señal del sensor
const int LED_RIEGO  = 13;   // LED de alerta de riego

// Calibración: mide en aire/tierra seca y en agua/tierra mojada
const int VALOR_SECO   = 800;  // lectura con el sensor seco
const int VALOR_MOJADO = 350;  // lectura con el sensor mojado

// Si la humedad baja de este porcentaje, se recomienda regar
const int UMBRAL_PORCENTAJE = 30;

void setup() {
  Serial.begin(9600);
  pinMode(LED_RIEGO, OUTPUT);
  Serial.println("=== Monitor de humedad del suelo ===");
}

void loop() {
  int lectura = analogRead(PIN_SENSOR);
  int porcentaje = map(lectura, VALOR_SECO, VALOR_MOJADO, 0, 100);
  porcentaje = constrain(porcentaje, 0, 100);

  Serial.print("Lectura: ");
  Serial.print(lectura);
  Serial.print("  |  Humedad: ");
  Serial.print(porcentaje);
  Serial.print("%  |  Estado: ");

  if (porcentaje < UMBRAL_PORCENTAJE) {
    Serial.println("TIERRA SECA -> Se recomienda regar");
    digitalWrite(LED_RIEGO, HIGH);
  } else {
    Serial.println("TIERRA HUMEDA -> No necesita riego");
    digitalWrite(LED_RIEGO, LOW);
  }

  delay(1000);
}
