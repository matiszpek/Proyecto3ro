#include <Arduino.h>
#include <BluetoothSerial.h>

#define servo A0
#define motor A1
#define modo 9

// Creamos el objeto BluetoothSerial
BluetoothSerial SerialBT;

// vector con los datos a enviar
int datos[4];

void setup() {
  pinMode(servo, INPUT);
  pinMode(motor, INPUT);
  pinMode(modo,INPUT_PULLUP);

  // Inicializamos el Bluetooth con el nombre del dispositivo
  SerialBT.begin("ESP32Car");  // Nombre del dispositivo Bluetooth

  // Inicializamos el puerto serie para monitoreo
  Serial.begin(115200);
}

void loop() { 
  // Leemos los valores de los potenciómetros (o cualquier otro input)
  datos[0] = analogRead(servo);   // Dirección del servo
  datos[1] = analogRead(motor);  // Velocidad de los motores
  datos[2]=  digitalRead(modo); // Modo luces
  datos[3] = 9999;             // Valor fijo (cheuqer)

  // Convertimos los valores en un formato de cadena
  String mensaje = String(datos[0]) + "," + String(datos[1])+ "," + String(datos[2]) + String(datos[3]) + "\n";

  // Enviamos los datos por Bluetooth
  SerialBT.println(mensaje);

  // Reportamos por el puerto serie los datos enviados para monitoreo
  Serial.print("Servo: "); 
  Serial.print(datos[0]); 
  Serial.print(" | Velocidad: "); 
  Serial.print(datos[1]);
  Serial.print(" | Modo: ");  
  Serial.print(datos[2]); 
  Serial.print(" | Chequer: ");
  Serial.print(datos[3]);
  Serial.println(mensaje); 

  // Pequeña pausa antes del siguiente ciclo
  delay(50);
}
