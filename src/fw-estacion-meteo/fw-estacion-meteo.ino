#include <Adafruit_BME280.h>

// Variables sensor de aire

Adafruit_BME280 bme280; // Crear objeto que controla el sensor
double temperaturaAire; // Crear variable para guardar la temperatura del aire
double humedadAire; // Crear variable para guardar la humedad del aire
double presionBarometrica; // Crear variable para guardar la presión barométrica

// Variables del anemómetro

const int ANEMOMETRO_PIN_LDR = A2;
const int ANEMOMETRO_UMBRAL_LASER_SUP = 480;
const int ANEMOMETRO_UMBRAL_LASER_INF = 380;
int anemometro_lecturaLdr;
int anemometro_nivelLaser;
int anemometro_nivelLaserAnterior;
unsigned long int anemometro_tiempoLaserActual;
unsigned long int anemometro_tiempoLaserAnterior;
unsigned long int anemometro_tiempoEntreVueltas;
unsigned long int anemometro_sumaTiempoEntreVueltas;
unsigned long int anemometro_mediaTiempoEntreVueltas;
int anemometro_contadorMediaTiempoEntreVueltas;
const int ANEMOMETRO_N_MEDIA_TIEMPO_ENTRE_VUELTAS = 5;
const int ANEMOMETRO_RADIO = 99; // en milímetros
const double ANEMOMETRO_PERIMETRO = 2 * 3.141592 * ANEMOMETRO_RADIO;
int velocidadDelViento;



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

void imprimirVelocidadDelViento() {
  Serial.print("Velocidad del viento: ");
  Serial.print(velocidadDelViento);
  Serial.println(" km/h");
}

void anemometroMedir() {
  anemometro_lecturaLdr = analogRead(ANEMOMETRO_PIN_LDR); // Leer el nivel de luz en el LDR

  // Decidir si el láser está incidiendo o no en el LDR según umbrales
  if(anemometro_lecturaLdr > ANEMOMETRO_UMBRAL_LASER_SUP) {
    anemometro_nivelLaser = HIGH;
  } else if(anemometro_lecturaLdr < ANEMOMETRO_UMBRAL_LASER_INF) {
    anemometro_nivelLaser = LOW;
  }

  if(anemometro_nivelLaser == HIGH && anemometro_nivelLaserAnterior == LOW) {
    anemometro_tiempoLaserActual = millis(); // Leer la hora actual
    anemometro_tiempoEntreVueltas = anemometro_tiempoLaserActual - anemometro_tiempoLaserAnterior; // Comprobar cuánto anemometro_tiempo ha pasado desde la última vez
    anemometro_sumaTiempoEntreVueltas += anemometro_tiempoEntreVueltas; // Incrementar variable por anemometro_tiempoEntreVueltas (igual a anemometro_mediaTiempoEntreVueltas = anemometro_mediaTiempoEntreVueltas + anemometro_tiempoEntreVueltas;)
    anemometro_contadorMediaTiempoEntreVueltas++; // Incrementar en 1 el contador (igual a anemometro_contadorMediaTiempoEntreVueltas = anemometro_contadorMediaTiempoEntreVueltas + 1;)
    if(anemometro_contadorMediaTiempoEntreVueltas >= ANEMOMETRO_N_MEDIA_TIEMPO_ENTRE_VUELTAS) { // Si hemos sumado el número de veces que queremos para la media
      anemometro_mediaTiempoEntreVueltas = anemometro_sumaTiempoEntreVueltas / ANEMOMETRO_N_MEDIA_TIEMPO_ENTRE_VUELTAS; // Dividir entre el número de adiciones para encontrar la media
      anemometro_sumaTiempoEntreVueltas = 0; // Resetear variables a 0 para empezar la media de nuevo
      anemometro_contadorMediaTiempoEntreVueltas = 0;
      velocidadDelViento = (3.6 * ANEMOMETRO_PERIMETRO/2) / anemometro_mediaTiempoEntreVueltas; // 3.6 = 3600000 (ms/h) / 1000000 (mm/km)
    }
    anemometro_tiempoLaserAnterior = anemometro_tiempoLaserActual; // Guardar la hora para la próxima vuelta
  }
  anemometro_nivelLaserAnterior = anemometro_nivelLaser; // Guardar el nivel del láser para compararlo con la próxima lectura
}

void setup() {
  bme280.begin(0x76); // Inicializar el sensor que tiene la dirección I²C 0x76
  Serial.begin(9600); // Inicializar puerto serie
}

void loop() {
  temperaturaAire = bme280.readTemperature(); // Leer temperatura del aire y guardarla en su variable
  humedadAire = bme280.readHumidity(); // Leer humedad relativa del aire
  presionBarometrica = bme280.readPressure() / 100; // Leer presión barométrica y convertirla a milibares
  anemometroMedir();
  
  imprimirTemperaturaAire();
  imprimirHumedadAire();
  imprimirPresionBarometrica();
  imprimirVelocidadDelViento();

  Serial.println();

}
