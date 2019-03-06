#include <Adafruit_BME280.h>

Adafruit_BME280 bme280; // Crear objeto que controla el sensor

double temperaturaAire; // Crear variable para guardar la temperatura del aire
double humedadAire; // Crear variable para guardar la humedad del aire
double presionBarometrica; // Crear variable para guardar la presión barométrica

void imprimirTemperaturaAire() {
  Serial.print("Temperatura: "); // Imprimir el valor de la temperatura del aire con 1 decimal
  Serial.print(temperaturaAire, 1); 
  Serial.println(" C");
}

void imprimirHumedadAire() {
  Serial.print("Humedad: "); // Imprimir el valor de la humedad relativa del aire con 1 decimal
  Serial.print(humedadAire, 1);
  Serial.println("%");
}

void imprimirPresionBarometrica() {
  Serial.print("Presion barometrica: "); // Imprimir el valor de la presión barométrica sin decimales
  Serial.print(presionBarometrica, 0);
  Serial.println(" mbar");
}

void setup() {
  bme280.begin(0x76); // Inicializar el sensor que tiene la dirección I²C 0x76
  Serial.begin(9600); // Inicializar puerto serie
}

void loop() {
  temperaturaAire = bme280.readTemperature(); // Leer temperatura del aire y guardarla en su variable
  humedadAire = bme280.readHumidity(); // Leer humedad relativa del aire
  presionBarometrica = bme280.readPressure() / 100; // Leer presión barométrica y convertirla a milibares
  
  imprimirTemperaturaAire();
  imprimirHumedadAire();
  imprimirPresionBarometrica();

  Serial.println();

  delay(1000);
}
