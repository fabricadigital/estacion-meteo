const int PIN_LDR = A2;

const int UMBRAL_LASER_SUP = 480;
const int UMBRAL_LASER_INF = 380;

int lecturaLdr;
int nivelLaser;
int nivelLaserAnterior;

unsigned long int tiempoLaserActual;
unsigned long int tiempoLaserAnterior;
unsigned long int tiempoEntreVueltas;

void setup() {
  Serial.begin(9600);
}

void loop() {
  lecturaLdr = analogRead(PIN_LDR); // Leer el nivel de luz en el LDR

  // Decidir si el láser está incidiendo o no en el LDR según umbrales
  if(lecturaLdr > UMBRAL_LASER_SUP) {
    nivelLaser = HIGH;
  } else if(lecturaLdr < UMBRAL_LASER_INF) {
    nivelLaser = LOW;
  }

  if(nivelLaser == HIGH && nivelLaserAnterior == LOW) {
    tiempoLaserActual = millis(); // Leer la hora actual
    tiempoEntreVueltas = tiempoLaserActual - tiempoLaserAnterior; // Comprobar cuánto tiempo ha pasado desde la última vez
    tiempoLaserAnterior = tiempoLaserActual; // Guardar la hora para la próxima vuelta
    Serial.println("Laser detectado");
    Serial.println(tiempoEntreVueltas);
  }
  
  nivelLaserAnterior = nivelLaser;
}
