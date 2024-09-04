#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Servo.h>

// Configuración de Bluetooth
BluetoothSerial SerialBT;

// Pines de motores
#define m1s 14  // PWM Motor 1
#define m1a 26  // Motor 1 A
#define m1b 27  // Motor 1 B

#define m2s 12  // PWM Motor 2
#define m2a 25  // Motor 2 A
#define m2b 33  // Motor 2 B

// Variables para los datos recibidos
int l;  // servo
int n;  // motores

int sAngle = 0;  // dirección del servo (ruedas)
int mSpeed = 0;  // velocidad de los motores
const int dZ = 50;     // deadzone (punto medio que no avanza ni retrocede)

Servo servo;

void setup() {
  // Inicialización del Bluetooth
  SerialBT.begin("ESP32Car");  // Nombre del dispositivo Bluetooth

  // Inicialización del servo
  servo.attach(13);  // pin del servo

  // Inicialización de los pines de los motores
  pinMode(m1s, OUTPUT);
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);

  pinMode(m2s, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);

  // Inicialización del puerto serial
  Serial.begin(115200);
  Serial.println("Esperando conexión Bluetooth...");
}

void loop() {
  if (SerialBT.available()) {
    // Leemos los datos enviados por Bluetooth
    String data = SerialBT.readStringUntil('\n');
    String dataL = data.substring(0, data.indexOf(','));
    String dataN = data.substring(data.indexOf(',') + 1);

    l = dataL.toInt();
    n = dataN.toInt();

    // Manejo de la dirección y velocidad
    manejo();
  }
}

void manejo() {
  // Control del servo (dirección)
  sAngle = map(l, 0, 1023, 0, 180);  // traducimos el dato a ángulos del servo
  servo.write(sAngle);  // mueve el servo al ángulo calculado

  // Control de los motores (velocidad y dirección)
  if (n < 512 - dZ) {
    mSpeed = map(n, 0, 512 - dZ, -255, 0);  // adelante
  } else if (n > 512 + dZ) {
    mSpeed = map(n, 512 + dZ, 1023, 0, 255);  // atrás
  } else {
    mSpeed = 0;  // detener
  }

  // Configuración de la dirección de los motores
  if (mSpeed >= 0) {
    // Movimiento hacia adelante
    analogWrite(m1s, mSpeed);
    digitalWrite(m1a, HIGH);
    digitalWrite(m1b, LOW);

    analogWrite(m2s, mSpeed);
    digitalWrite(m2a, HIGH);
    digitalWrite(m2b, LOW);
  } else {
    // Movimiento hacia atrás
    analogWrite(m1s, -mSpeed);
    digitalWrite(m1a, LOW);
    digitalWrite(m1b, HIGH);

    analogWrite(m2s, -mSpeed);
    digitalWrite(m2a, LOW);
    digitalWrite(m2b, HIGH);
  }
}
