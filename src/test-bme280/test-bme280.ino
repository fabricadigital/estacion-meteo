#include <Adafruit_BME280.h>

Adafruit_BME280 bme280; // Crear objeto que controla el sensor

double temperaturaAire; // Crear variable para guardar la temperatura del aire
double humedadAire; // Crear variable para guardar la humedad del aire
double presionBarometrica; // Crear variable para guardar la presión barométrica

void setup() {
  bme280.begin(0x76); // Inicializar el sensor que tiene la dirección I²C 0x76
  Serial.begin(9600); // Inicializar puerto serie
}

void loop() {
  temperaturaAire = bme280.readTemperature(); // Leer temperatura del aire y guardarla en su variable
  Serial.print("Temperatura: ");
  Serial.print(temperaturaAire, 1); // Imprimir el valor de la temperatura del aire con 1 decimal
  Serial.println(" C");

  humedadAire = bme280.readHumidity();
  Serial.print("Humedad: ");
  Serial.print(humedadAire, 1);
  Serial.println("%");

  presionBarometrica = bme280.readPressure() / 100;
  Serial.print("Presion barometrica: ");
  Serial.print(presionBarometrica, 0);
  Serial.println(" mbar");

  Serial.println();

  delay(1000);
}
