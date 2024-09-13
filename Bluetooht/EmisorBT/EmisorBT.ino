#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// Pines para los potenciómetros y el botón
#define potDireccion 34  // Potenciómetro para la dirección
#define potVelocidad 35  // Potenciómetro para la velocidad
#define botonLuces 32    // Botón para el modo de luces

// Variables para almacenar los valores leídos
int direccion = 0;
int velocidad = 0;
int modoLuces = 0;

int estadoBotonAnterior = LOW;
int estadoBotonActual = LOW;
unsigned long tiempoAnterior = 0;
const unsigned long debounceDelay = 50;  // Retraso para evitar rebotes

void setup() {
  // Inicialización del Bluetooth
  SerialBT.begin("ControlCar");

  // Inicialización de los pines
  pinMode(potDireccion, INPUT);
  pinMode(potVelocidad, INPUT);
  pinMode(botonLuces, INPUT_PULLUP);  // El botón está en modo pull-up

  // Inicialización del puerto serial para depuración
  Serial.begin(115200);
}

void loop() {
  // Leemos los valores de los potenciómetros
  direccion = analogRead(potDireccion);
  velocidad = analogRead(potVelocidad);

  // Leemos el estado actual del botón de luces
  estadoBotonActual = digitalRead(botonLuces);

  // Verificamos si el botón fue presionado con un debounce
  if (estadoBotonActual != estadoBotonAnterior) {
    if (millis() - tiempoAnterior > debounceDelay) {
      if (estadoBotonActual == LOW) {  // Cuando el botón es presionado
        modoLuces = (modoLuces + 1) % 3;  // Cambiamos el modo de luces (0, 1, 2)
        tiempoAnterior = millis();
      }
    }
  }
  estadoBotonAnterior = estadoBotonActual;

  // Creamos el string con los valores para enviar
  String mensaje = "M," + String(direccion) + "," + String(velocidad) + "," + String(modoLuces) + ",1\n";
  SerialBT.print(mensaje);

  // Para fines de depuración, imprimimos los valores en el monitor serial
  Serial.print("Direccion: ");
  Serial.print(direccion);
  Serial.print(" Velocidad: ");
  Serial.print(velocidad);
  Serial.print(" Modo luces: ");
  Serial.println(modoLuces);

  delay(100);  // Pequeño retraso para evitar saturar la conexión Bluetooth
}
