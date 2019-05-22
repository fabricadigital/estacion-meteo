#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_ST77xx.h>
#include <Adafruit_ST7735.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#include <Adafruit_BME280.h>

// Controlador de la pantalla
Adafruit_ST7735 pantalla = Adafruit_ST7735(10, 8, 9);

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

// Variables del botón
const int BOTON_PIN = 5;
int estadoActualBoton;
int estadoAnteriorBoton;

// Variables del estado de la pantalla
const int PANTALLA_TEMPERATURA = 0;
const int PANTALLA_HUMEDAD = 1;
const int PANTALLA_PRESION = 2;
const int PANTALLA_VELOCIDADVIENTO = 3;
int estadoPantalla = PANTALLA_TEMPERATURA;

// Variables para medir el tiempo y refrescar la pantalla
unsigned long int tiempoUltimaActualizacion;
const unsigned long int TIEMPO_ENTRE_ACTUALIZACIONES = 20000;

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

void actualizarDatos() {
  switch(estadoPantalla) {
    case 0:
      temperaturaAire = bme280.readTemperature(); // Leer temperatura del aire y guardarla en su variable
      imprimirTemperaturaAire();
      pantalla.fillScreen(ST7735_YELLOW); // Limpiar la pantalla con fondo amarillo
      pantalla.setTextColor(ST7735_BLACK);// Seleccionar el color del texto
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.setCursor(20, 20);
      pantalla.print("Temperatura");
      pantalla.setFont(&FreeSans24pt7b);  // Seleccionar la fuente grande
      pantalla.setCursor(10, 100);
      pantalla.print(temperaturaAire, 1);
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.print(" C");
      break;
    case 1:
      humedadAire = bme280.readHumidity(); // Leer humedad relativa del aire
      imprimirHumedadAire();
      pantalla.fillScreen(ST7735_BLUE); // Limpiar la pantalla con fondo amarillo
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.setTextColor(ST7735_WHITE);// Seleccionar el color del texto
      pantalla.setCursor(20, 50);
      pantalla.print("Humedad");
      pantalla.setFont(&FreeSans24pt7b);  // Seleccionar la fuente grande
      pantalla.setCursor(10, 100);
      pantalla.print(humedadAire, 0);
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.print(" %");
      break;
    case 2:
      presionBarometrica = bme280.readPressure() / 100; // Leer presión barométrica y convertirla a milibares
      imprimirPresionBarometrica();
      pantalla.fillScreen(ST7735_RED); // Limpiar la pantalla con fondo amarillo
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.setTextColor(ST7735_WHITE);// Seleccionar el color del texto
      pantalla.setCursor(20, 50);
      pantalla.print("Presion");
      pantalla.setFont(&FreeSans24pt7b);  // Seleccionar la fuente grande
      pantalla.setCursor(10, 100);
      pantalla.print(presionBarometrica, 0);
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.print(" mbar"); // Es lo mismo que hPa
      break;
    case 3:
      imprimirVelocidadDelViento();
      pantalla.fillScreen(ST7735_CYAN); // Limpiar la pantalla con fondo amarillo
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.setTextColor(ST7735_BLACK);// Seleccionar el color del texto
      pantalla.setCursor(20, 50);
      pantalla.print("Viento");
      pantalla.setFont(&FreeSans24pt7b);  // Seleccionar la fuente grande
      pantalla.setCursor(10, 100);
      pantalla.print(velocidadDelViento, DEC);
      pantalla.setFont(&FreeSans9pt7b);  // Seleccionar la fuente pequeña
      pantalla.print(" km/h"); // Es lo mismo que hPa
      break;
  }

  tiempoUltimaActualizacion = millis(); // Actualiza el tiempo del último refresco de pantalla.
}

void setup() {
  bme280.begin(0x76); // Inicializar el sensor que tiene la dirección I²C 0x76
  Serial.begin(9600); // Inicializar puerto serie

  pinMode(BOTON_PIN, INPUT_PULLUP); // Configurar pin del botón como entrada con pullup
  estadoActualBoton = digitalRead(BOTON_PIN); // Leer el estado inicial del botón
  estadoAnteriorBoton = estadoActualBoton;

  // Inicializar pantalla
  pantalla.initR(INITR_BLACKTAB);
  // Rotar pantalla para ponerla apaisada
  pantalla.setRotation(1);
  // Limpiar pantalla (rellenar de negro)
  pantalla.fillScreen(ST7735_BLACK);

  pantalla.setFont(&FreeSans9pt7b);
  pantalla.setTextWrap(false);
  pantalla.setCursor(17, 50);
  pantalla.print("Estacion Meteo");
  pantalla.setCursor(14, 70);
  pantalla.print("por Pablo Bacho");

  delay(2000);
  actualizarDatos();
}

void loop() {
  anemometroMedir();

  estadoActualBoton = digitalRead(BOTON_PIN);
  if(estadoActualBoton == LOW && estadoAnteriorBoton == HIGH) {
    estadoPantalla++;
    if(estadoPantalla > 3) {
      estadoPantalla = 0;
    }
    actualizarDatos();
  }
  estadoAnteriorBoton = estadoActualBoton; // Guardar el estado del botón para la próxima vuelta

  if(millis() - tiempoUltimaActualizacion >= TIEMPO_ENTRE_ACTUALIZACIONES) {
    actualizarDatos();
  }
  
}
