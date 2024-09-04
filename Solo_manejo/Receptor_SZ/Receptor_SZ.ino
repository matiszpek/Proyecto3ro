#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//Comunicacion
//Declaremos los pines CE y el CSN
#define CE_PIN 9
#define CSN_PIN 10

//Pines de motores
#define m1s 28
#define m1a 30
#define m1b 32

#define m2s 29
#define m2a 31
#define m2b 33

//Variable con la dirección del canal que se va a leer
byte direccion[1] = { 'h' };

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
int datos[3];
//reescritura de datos recibidos
int l;  //servo
int n;  //motores

int sAngle = 0;  //direccion del servo (ruedas)
int mSpeed = 0;  // velocidad de los motores
int dZ = 50;     //deadzone (punto medio que no avanza ni retrocede)

Servo servo;

void setup() {

  servo.attach(7);  // pin del servo

  pinMode(m1s, OUTPUT);  //velocidad 1
  pinMode(m1a, OUTPUT);  //motor1 a
  pinMode(m1b, OUTPUT);  //motor1 b

  pinMode(m2s, OUTPUT);  //velocidad 2
  pinMode(m2a, OUTPUT);  //motor2 a
  pinMode(m2b, OUTPUT);  //motor2 b

  //inicializamos el NRF24L01
  radio.begin();

  //Abrimos el canal de Lectura
  radio.openReadingPipe(1, direccion);

  //empezamos a escuchar por el canal
  radio.startListening();

  pinMode(13, OUTPUT);
  //inicializamos el puerto serie
  Serial.begin(2000000);
  Serial.println("hola");
}
void loop() {
 comu();
 manejo();
}
void ejecutar_paso(byte tipo, byte cant) {
  int speedA = map(281, 512 - dZ, 1023, 0, 255);
  int speedR = map(843, 512 + dZ, 1023, 0, 255);
  switch (tipo) {
    case 0:  //Avanzar
      analogWrite(m1s, speedA);
      digitalWrite(m1a, HIGH);
      digitalWrite(m1b, LOW);

      analogWrite(m2s, speedA);
      digitalWrite(m2a, HIGH);
      digitalWrite(m2b, LOW);

      delay(cant);

      analogWrite(m1s, 0);
      digitalWrite(m1a, LOW);
      digitalWrite(m1b, LOW);

      analogWrite(m2s, 0);
      digitalWrite(m2a, LOW);
      digitalWrite(m2b, LOW);
      break;
    case 1:  //Retroceder
      analogWrite(m1s, -speedR);
      digitalWrite(m1a, LOW);
      digitalWrite(m1b, HIGH);

      analogWrite(m2s, -speedR);
      digitalWrite(m2a, LOW);
      digitalWrite(m2b, HIGH);

      delay(cant);

      analogWrite(m1s, 0);
      digitalWrite(m1a, LOW);
      digitalWrite(m1b, LOW);

      analogWrite(m2s, 0);
      digitalWrite(m2a, LOW);
      digitalWrite(m2b, LOW);
      break;
    case 2:  // Girar a la izquierda
      servo.write(90 + cant);
      delay(100);
      break;
    case 3:  // Girar a la derecha
      servo.write(cant);
      delay(100);
      break;
    case 4:  // Esperar
      delay(cant);
      break;
  }
}
void comu() {
  //uint8_t numero_canal;
  //if ( radio.available(&numero_canal) )
  if (radio.available()) {
    //Leemos los datos y los guardamos en la variable datos[]
    radio.read(&datos, sizeof(datos));

    l = datos[0];  //servo
    n = datos[1];  //motores

    //reportamos por el puerto serial los datos recibidos
    Serial.println(datos[0]);
    Serial.println(datos[1]);
    Serial.println(datos[2]);

  } else {
    Serial.println("No hay datos de radio disponibles");
  }
}

void manejo() {
  sAngle = map(l, 0, 1023, 0, 180); // traducimos el dato a angulos del servo
  servo.write(sAngle);  // mueve el servo el angulo de arriba

  if (n < 512 - dZ) {
    mSpeed = map(n, 0, 512 - dZ, -255, 0);  // adelante
  } else if (n > 512 + dZ) {
    mSpeed = map(n, 512 + dZ, 1023, 0, 255);  // atras
  } else {
    mSpeed = 0;  // nada
  }

  if (mSpeed >= 0) {
    Serial.print("Algo hace ");
    Serial.println(mSpeed);
    analogWrite(m1s, mSpeed);  // adelante1 velocidad (0=no avanzar)
    digitalWrite(m1a, HIGH);   // direccion ade
    digitalWrite(m1b, LOW);

    analogWrite(m2s, mSpeed);  // adelante2 velocidad
    digitalWrite(m2a, HIGH);
    digitalWrite(m2b, LOW);

  } else {
    Serial.print("ALgo hace ");
    Serial.println(mSpeed);
    analogWrite(m1s, -mSpeed);  // atras velociad (0= no atras)
    digitalWrite(m1a, LOW);     // configuracion atras1
    digitalWrite(m1b, HIGH);

    analogWrite(m2s, -mSpeed);  // atras velociad
    digitalWrite(m2a, LOW);     // configuracion atras2
    digitalWrite(m2b, HIGH);
  }
}
