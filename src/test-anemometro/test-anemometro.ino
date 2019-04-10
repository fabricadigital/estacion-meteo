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

void setup() {
  Serial.begin(9600);
}

void loop() {
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
      Serial.print("Media de anemometro_tiempo: ");
      Serial.println(anemometro_mediaTiempoEntreVueltas);
      velocidadDelViento = (3.6 * ANEMOMETRO_PERIMETRO/2) / anemometro_mediaTiempoEntreVueltas; // 3.6 = 3600000 (ms/h) / 1000000 (mm/km)
      Serial.print("Velocidad del viento: ");
      Serial.println(velocidadDelViento);
    }
    Serial.print("Tiempo: ");
    Serial.println(anemometro_tiempoEntreVueltas);
    anemometro_tiempoLaserAnterior = anemometro_tiempoLaserActual; // Guardar la hora para la próxima vuelta
  }
  
  anemometro_nivelLaserAnterior = anemometro_nivelLaser; // Guardar el nivel del láser para compararlo con la próxima lectura
}
