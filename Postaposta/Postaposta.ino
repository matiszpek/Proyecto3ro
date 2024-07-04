#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#define pin_tira 7

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


Adafruit_NeoPixel arriba = Adafruit_NeoPixel(128, pin_tira, NEO_GRB + NEO_KHZ800);
struct RGB {
  byte R;
  byte G;
  byte B;
};
struct rainbow {
  bool rainbowing = false;
  int rDelay;
};
rainbow rainbows[2][3];
byte currentWheelPos = 255;
bool apagado[2][3] = { { false, false, false }, { false, false, false } };
byte perf_selec = 0;
const byte max_vals = 7;
String valores[max_vals];
RGB leds[2][192];
uint16_t j;


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
  arriba.begin();
  arriba.setBrightness(255);
  for (int i = 0; i < 64; i++) {
    arriba.setPixelColor(i, arriba.Color(255, 0, 0));
    delay(10);
    arriba.show();
  }
  for (int i = 64; i < 96; i++) {
    arriba.setPixelColor(i, arriba.Color(0, 255, 0));
    delay(10);
    arriba.show();
  }
  for (int i = 96; i < 128; i++) {
    arriba.setPixelColor(i, arriba.Color(0, 0, 255));
    delay(10);
    arriba.show();
  }
}

void loop() {
  while (Serial.available() == 0) {
    comprobarRainbow(); 
    comu(); 
    manejo(); 
    j++; 
    if(j == 256 * 5) {
      j = 0;
    }
  }
  if (Serial.available() > 0) { 
    Gryn();  // Ejecuta la función Gryn cuando hay datos disponibles en el puerto serial
  } 
}

void control_leds() {
  String entrada = Serial.readStringUntil('\n');
  if (entrada != "") {
    Serial.println("recibido: " + entrada);
    if (entrada != "end") {
      leer(entrada);
    } else {
      aplicarPerfiles(perf_selec);
      Serial.println("END");
      arriba.show();
      delay(1000);
    }
  } else {
    comu();
    manejo();
  }
}
void comprobarRainbow() {
  if (rainbows[perf_selec][0].rainbowing) {
    rainbowWallEffect8x8(rainbows[perf_selec][0].rDelay, 0);
    //Serial.println("8x8 rainbow");
  }
  if (rainbows[perf_selec][1].rainbowing) {
    rainbowCycle(rainbows[perf_selec][1].rDelay, 64, 1, j);
    //Serial.println("izq rainbow");
  }
  if (rainbows[perf_selec][2].rainbowing) {
    rainbowCycle(rainbows[perf_selec][2].rDelay, 96, 2, j);
    //Serial.println("der rainbow");
  }
}
void dApagadoRainbow(byte perf, byte num_led, bool aRain, bool aApa) {
  byte ang;
  if (num_led < 64) {
    ang = 0;
  } else if (num_led >= 64 && num_led < 96) {
    ang = 1;
  } else {
    ang = 2;
  }
  if (aApa) {
    apagado[perf][ang] = false;
    Serial.print(ang);
    Serial.println(" apagado cancelado");
  }
  if (aRain) {
    rainbows[perf][ang].rainbowing = false;
    Serial.print(ang);
    Serial.println(" rainbow cancelado");
  }
}
void leer(String input) {
  separador(input);
  //p1:9:151:119:15
  //p1s:1:1:apagado
  //p1s:1:0:rainbow:1
  if (valores[0] == "p1") {
    dApagadoRainbow(0, valores[1].toInt(), true, true);
    leds[0][valores[1].toInt()].R = valores[2].toInt();
    leds[0][valores[1].toInt()].G = valores[3].toInt();
    leds[0][valores[1].toInt()].B = valores[4].toInt();
  } else if (valores[0] == "p2") {
    dApagadoRainbow(1, valores[1].toInt(), true, true);
    leds[1][valores[1].toInt()].R = valores[2].toInt();
    leds[1][valores[1].toInt()].G = valores[3].toInt();
    leds[1][valores[1].toInt()].B = valores[4].toInt();
  } else if (valores[0].length() > 2) {
    byte perf = valores[1].toInt() - 1;
    if (valores[3] == "apagado") {
      apagado[perf][valores[2].toInt()] = true;
      if (valores[2].toInt() == 0) {
        dApagadoRainbow(perf, valores[2].toInt(), true, false);
      } else {
        dApagadoRainbow(perf, 64 + valores[2].toInt() * 16, true, false);
      }
    } else {
      rainbows[perf][valores[2].toInt()].rainbowing = true;
      rainbows[perf][valores[2].toInt()].rDelay = valores[4].toInt();
      if (valores[2].toInt() == 0) {
        dApagadoRainbow(perf, valores[2].toInt(), false, true);
      } else {
        dApagadoRainbow(perf, 64 + valores[2].toInt() * 16, false, true);
      }
    }
  } else if (valores[0].startsWith("m")) {
    int post_delay;
    int cant_pasos;
    cant_pasos = valores[1].toInt();
    post_delay = valores[2].toInt();
    byte pasos[cant_pasos][2];
    for (int i = 0; i < cant_pasos; i++) {
      while (Serial.available() == 0) {}
      String entrada2 = Serial.readStringUntil('\n');
      separador(entrada2);
      pasos[i][0] = valores[0].toInt();
      pasos[i][1] = valores[1].toInt();
    }
    delay(post_delay);
    for (int i = 0; i < cant_pasos; i++) {
      ejecutar_paso(pasos[i][0], pasos[i][1]);
      delay(10);
    }
  }
}
void separador(String input) {
  for (int i = 0; i < max_vals; i++) {
    valores[i] = "";
  }
  String parte;
  byte index = 0;
  for (int i = 0; i < input.length(); i++) {
    char letra = input.charAt(i);
    if (letra != ':') {
      parte = parte + letra;
    } else {
      valores[index] = parte;
      index++;
      parte = "";
    }
  }
  valores[index] = parte;
}
void aplicarPerfiles(byte perf) {
  bool Tapagado0 = apagado[perf][0];
  bool Tapagado1 = apagado[perf][1];
  bool Tapagado2 = apagado[perf][2];
  if (Tapagado0) {
    for (int i = 0; i < 64; i++) {
      arriba.setPixelColor(0 * 64 + i, arriba.Color(0, 0, 0));
    }
    Serial.println("arriba apagado");
  }
  if (Tapagado1) {
    for (int i = 64; i < 96; i++) {
      arriba.setPixelColor(i, arriba.Color(0, 0, 0));
    }
    Serial.println("izquierda apagado");
  }
  if (Tapagado2) {
    for (int i = 96; i < 128; i++) {
      arriba.setPixelColor(i, arriba.Color(0, 0, 0));
    }
    Serial.println("derecha apagado");
  }
  for (int i = 0; i < arriba.numPixels(); i++) {
    if (!(Tapagado0 && i >= 0 && i < 64) && !(Tapagado1 && i >= 64 && i < 96) && !(Tapagado2 && i >= 96 && i < 128)) {
      arriba.setPixelColor(i, arriba.Color(leds[perf][i].R, leds[perf][i].G, leds[perf][i].B));
    }
  }
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
void positionRainbow() {
  if (currentWheelPos > 0) {
    currentWheelPos--;
  } else {
    currentWheelPos = 255;
  }
}
void rainbowCycle(uint8_t wait, byte start, byte ang, uint16_t j) {
  uint16_t i;
  for (i = start; i < start + 16; i++) {
    arriba.setPixelColor(i, Wheel(((i * 256 / arriba.numPixels()) + j) & 255));
  }
  for (i = start + 16; i < start + 32; i++) {
    arriba.setPixelColor(i, Wheel(((i * 256 / arriba.numPixels()) + j) & 255));
  }
  arriba.show();
  delay(wait);
}
void rainbowWallEffect8x8(int wait, byte start) {
  positionRainbow();
  for (int i = start + 7; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos));
  }

  for (int i = start + 6; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 1 * 8));
  }

  for (int i = start + 5; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 2 * 8));
  }

  for (int i = start + 4; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 3 * 8));
  }

  for (int i = start + 3; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 4 * 8));
  }

  for (int i = start + 2; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 5 * 8));
  }

  for (int i = start + 1; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 6 * 8));
  }

  for (int i = start + 0; i < start + 64; i += 8) {
    arriba.setPixelColor(i, Wheel(currentWheelPos - 7 * 8));
  }

  arriba.show();
  delay(wait);
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return arriba.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return arriba.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return arriba.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void comu() {
  uint8_t numero_canal;
  //if ( radio.available(&numero_canal) )
  if (radio.available()) {
    //Leemos los datos y los guardamos en la variable datos[]
    radio.read(&datos, sizeof(datos));

    l = datos[0];  //servo
    n = datos[1];  //motores

    //reportamos por el puerto serial los datos recibidos
    digitalWrite(13, LOW);
    //Serial.println(datos[0]);
    //Serial.println(datos[1]);
    //Serial.println(datos[2]);

  } else {
    digitalWrite(13, HIGH);
    //Serial.println("No hay datos de radio disponibles");
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
    //Serial.print("ALgo hace ");
    //Serial.println(mSpeed);
    analogWrite(m1s, mSpeed);  // adelante1 velocidad (0=no avanzar)
    digitalWrite(m1a, HIGH);   // direccion ade
    digitalWrite(m1b, LOW);

    analogWrite(m2s, mSpeed);  // adelante2 velocidad
    digitalWrite(m2a, HIGH);
    digitalWrite(m2b, LOW);

  } else {
    //Serial.print("ALgo hace ");
    //Serial.println(mSpeed);
    analogWrite(m1s, -mSpeed);  // atras velociad (0= no atras)
    digitalWrite(m1a, LOW);     // configuracion atras1
    digitalWrite(m1b, HIGH);

    analogWrite(m2s, -mSpeed);  // atras velociad
    digitalWrite(m2a, LOW);     // configuracion atras2
    digitalWrite(m2b, HIGH);
  }
}
