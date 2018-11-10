#include <ShiftRegister74HC595.h>

int numeroDeShiftRegisters = 1; // Cantidad de shiftRegisters conectados en serie.
int serialDataPin = 11;         // DS
int clockPin = 12;              // SHCP
int latchPin = 8;               // STCP
int buttonPin = 9;              // El boton para cambiar entre sensores
int sensorSeleccionado = 0;     // El sensor actualmente seleccionado
int buttonState = 0;            // Estado del boton
int nivelHumedad = 0;           // Nivel de humedad a mostrar X/3
int valorHumedad = 0;           // Valor analogico sensado
int umbralHumedad = 300;        // Nivel de humedad a mostrar X/3
int bombaAguaPin = 7;           // La bomba de agua

unsigned long masterTiempoInicial;      // Tiempo de inicio del contador Master
unsigned long uiTiempoInicial;          // Tiempo de inicio del contador UI
unsigned long sensorTiempoInicial;      // Tiempo de inicio del contador Sensores
unsigned long parpadearTiempoInicial;   // Tiempo de inicio del contador Parpadeo Leds
unsigned long riegoTiempoInicial;       // Tiempo de inicio del contador de Riego

unsigned long masterTiempoActual;     // Tiempo actual del contador Master
unsigned long uiTiempoActual;         // Tiempo actual del contador UI
unsigned long sensorTiempoActual;     // Tiempo actual del contador Sensores
unsigned long parpadearTiempoActual;  // Tiempo actual del contador Parpadeo Leds
unsigned long riegoTiempoActual;       // Tiempo de actual del contador de Riego

const unsigned long masterTiempoRetardo = 1000;    // Duracion de intervalo Master
const unsigned long uiTiempoRetardo = 1000;       // Duracion de intervalo UI
const unsigned long sensorTiempoRetardo = 1000;   // Duracion de intervalo Sensores
const unsigned long parpadearTiempoRetardo = 500; // Duracion de intervalo Parpadeo Leds
const unsigned long riegoTiempoRetardo = 500;       // Tiempo de actual del contador de Riego


// Inicializamos el/los shiftRegisters
ShiftRegister74HC595 sr (numeroDeShiftRegisters, serialDataPin, clockPin, latchPin);

void setup() {
  Serial.begin(9600);             // Para debuggear
  pinMode(0, INPUT);              // Sensor Humedad N° 1
  pinMode(1, INPUT);              // Sensor Humedad N° 2
  pinMode(2, INPUT);              // Sensor Humedad N° 3
  pinMode(buttonPin, INPUT);      // Boton seleccion sensor
  pinMode(bombaAguaPin, OUTPUT);  // Para activar la bomba de agua.
  #define VALOR_HUMEDAD_MIN 300   // Valor minimo para la lectura de la humedad.
  #define VALOR_HUMEDAD_MAX 800   // Valor maximo para la lectura de la humedad.
}

// dibujarHumedad:
// Recibe el nivel de humedad (x/3)
// Hace un for para apagar los leds del shiftRegister en posicion 0 1 y 2.
// Luego prende el led correspondiente para representar el nivel de humedad.
void dibujarHumedad(int nivelHumedad) {
  for (int i = 0; i <= 2; i++) {
    sr.set(i, LOW);
  }
  sr.set(nivelHumedad, HIGH);
}

// limpiarLedsSensores:
// Apaga los leds 3, 4 y 5, que son los que usamos para mostrar el sensor seleccionado.
// TODO: Hacer que esta funcion sea reutilizable y usarla en dibujarHumedad.
void limpiarLedsSensores() {
  for (int i = 3; i <= 5; i++) {
    sr.set(i, LOW);
  }
}

// parpadearLed: Recibe el N° led
// Guarda el tiempo actual, lo compara contra el tiempo inicial del contador de parpadeo.
// Si el resultado es menor al tiempo de retardo, prendemos/apagamos el led
// Y actualizamos el parpadearTiempoInicial.
void parpadearLed(int led) {
  parpadearTiempoActual = millis();
  if (parpadearTiempoActual - parpadearTiempoInicial >= parpadearTiempoRetardo) {
    sr.set(3+led, !sr.get(3+led));
    parpadearTiempoInicial = parpadearTiempoActual; // Actualizamos el contador de tiempo
  }
}


void activarRiego() {
  riegoTiempoActual = millis();
  if (riegoTiempoActual - riegoTiempoInicial >= riegoTiempoRetardo) {
    digitalWrite(bombaAguaPin, HIGH);
    Serial.println("ACTIVAR RIEGO");     // Logeamos un mensaje
    Serial.println("ACTIVAR RIEGO");     // Logeamos un mensaje
    Serial.println("ACTIVAR RIEGO");     // Logeamos un mensaje
    Serial.println("ACTIVAR RIEGO");     // Logeamos un mensaje
    Serial.println("ACTIVAR RIEGO");     // Logeamos un mensaje
    riegoTiempoInicial = riegoTiempoActual; // Actualizamos el contador de tiempo
  }
}

void controlarUmbralHumedad(int valorHumedad) {
  Serial.println("controlarUmbralHumedad");     // Logeamos un mensaje
  Serial.println(valorHumedad);     // Logeamos un mensaje
  Serial.println("controlarUmbralHumedad");     // Logeamos un mensaje
  if (valorHumedad <= umbralHumedad) {
    activarRiego();
  } else {
    Serial.println("PARAR RIEGO");     // Logeamos un mensaje
    digitalWrite(bombaAguaPin, LOW);
  }
}

void loop() {
  // Guardamos el tiempo actual en milisegundos en cada contador
  uiTiempoActual = millis();
  sensorTiempoActual = millis();
  masterTiempoActual = millis();

  parpadearLed(sensorSeleccionado); // Parpadeamos el led del sensor seleccionado.

  // Si el timpo de retardo Master paso desde la ultima lectura
  if (masterTiempoActual - masterTiempoInicial >= masterTiempoRetardo) {
    buttonState = digitalRead(buttonPin); // Chequeamos el estado del boton
    if (buttonState == HIGH) {            // Si el boton esta presionado
      Serial.print("[BOTOOOON] ");        // Logeamos un mensaje
      sensorSeleccionado = sensorSeleccionado+1;  // Seleccionamos el siguiente sensor.
      if (sensorSeleccionado > 2) {   // Como tenemos 3 sensores (0, 1 y 2) chequeamos que no se pase
        sensorSeleccionado = 0;       // Si se pasa de 2, vuelve a 0.
      }
      Serial.print(sensorSeleccionado); // Logeamos el sensor seleccionado.
      limpiarLedsSensores();       // Limpiamos los LEDS de los sensores.
    }
    masterTiempoInicial = masterTiempoActual; // Actualizamos el contador de tiempo
  }

  // Si el timpo de retardo UI paso desde la ultima lectura
  if (uiTiempoActual - uiTiempoInicial >= uiTiempoRetardo) {
    Serial.println("UI REFRESH");     // Logeamos un mensaje
    dibujarHumedad(nivelHumedad);     // Dibujamos el nivel de humedad
    uiTiempoInicial = uiTiempoActual; // actualizamos el contador de tiempo.
  }

  // Si el timpo de retardo Sensor paso desde la ultima lectura
  if (sensorTiempoActual - sensorTiempoInicial >= sensorTiempoRetardo) {
    Serial.print("[SENSOR REFRESH] Sensor #"); // Logeamos un mensaje
    Serial.print(sensorSeleccionado);          // Mostrando el sensor seleccionado
    Serial.print(" value: ");
    // A continuacion vamos a hacer varias cosas
    // Vamos a hacer a leer el sensor analogico seleccionado.
    // Hacemos un constrain para restringir la lectura del sensor.
    // Le decimos cual es el valor minimo y maximo que queremos obtener
    // Luego mapeamos el valor medido a un rango del 0 al 2
    // 0, 1 y 2 son los leds que indican la medicion del sensor (x/3)
    valorHumedad = analogRead(sensorSeleccionado);
    nivelHumedad = map( constrain(valorHumedad, VALOR_HUMEDAD_MIN, VALOR_HUMEDAD_MAX) , VALOR_HUMEDAD_MIN, VALOR_HUMEDAD_MAX, 0, 2);
    Serial.println(analogRead(sensorSeleccionado));
    controlarUmbralHumedad(valorHumedad); // Chequeamos si hay que activar la bomba de agua.
    sensorTiempoInicial = sensorTiempoActual; // actualizamos el contador de tiempo.
  }
}

