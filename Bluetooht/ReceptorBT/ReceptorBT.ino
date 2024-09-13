#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#define pin_tira 2

// Pines de motores
#define m1s 26  // PWM Motor 1
#define m1a 25  // Motor 1 A
#define m1b 33  // Motor 1 B

#define m2s 27  // PWM Motor 2
#define m2a 14  // Motor 2 A
#define m2b 12  // Motor 2 B

// Especificacion para control de motores en esp32
#define PWM_FREQ 5000  // Frecuencia del PWM, 5000 Hz es un buen valor
#define PWM_RESOLUTION 8  // Resolución de 8 bits (valores de 0 a 255)

// Canal PWM para el Motor 1
#define M1_PWM_CHANNEL 0  // Canal 0 para el Motor 1
// Canal PWM para el Motor 2
#define M2_PWM_CHANNEL 1  // Canal 1 para el Motor 2


// Configuración de Bluetooth
BluetoothSerial SerialBT;

const int mov = 2;
const int mod = 1;
const int com = 1;
const int luc = 64;

struct RGB {
  int R;
  int G;
  int B;
};

int rec[mov + mod + com];  //recepcion datos para movimiento
struct RGB luces[luc];     //datos de las luces

// Variables para los datos recibidos
int l;  //   servo
int n;  //  motores
int m;  //  modo

int c;  // chequer

//Variables para movimiento
int sAngle = 0;     // dirección del servo (ruedas)
int mSpeed = 0;     // velocidad de los motores
const int dZ = 50;  // deadzone (punto medio que no avanza ni retrocede)

Servo servo;

Adafruit_NeoPixel arriba = Adafruit_NeoPixel(64, pin_tira, NEO_GRB + NEO_KHZ800);  //declaracion de la tira

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
  
// Configuración de PWM para los motores
  ledcSetup(M1_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(m1s, M1_PWM_CHANNEL);

  ledcSetup(M2_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(m2s, M2_PWM_CHANNEL);

  arriba.begin();
  arriba.setBrightness(150);

  //debuger test tira
  for (int i = 0; i < 64; i++) {
    arriba.setPixelColor(i, arriba.Color(255, 0, 0));
    delay(10);
    arriba.show();
  }

  // Inicialización del puerto serial
  Serial.begin(115200);
  Serial.println("Esperando conexión Bluetooth...");

  rec[0]=0;
  rec[1]=0;
  rec[2]=1;
  rec[3]=0;
}

void loop() {
  if (SerialBT.available()) {
    // Leemos los datos enviados por Bluetooth
    comu();

    // Chequeamos el mensaje chequer
    Serial.println(c);

    // Manejo de la dirección y velocidad
    manejo();

    // Cambio en el modo de luces
    luces();
  }
  else{
    Serial.println("Bluetooth not available");
  }
}

void comu() {
  String data = SerialBT.readStringUntil('\n');
  int commaIndex = 0;
  int previousIndex = 1;  // desde donde empieza el string (=1 por ser que el primero es de que es el dato)

  if (data[0] == 'M') {
    for (int i = 0; i < (mov + mod + com); i++) {
      // Encontramos la siguiente coma
      commaIndex = data.indexOf(',', previousIndex);
      // Si no se encuentra más comas, se procesa el último valor
      if (commaIndex == -1) {
        commaIndex = data.length();
      }
      // Extraemos el valor entre las comas
      rec[i] = data.substring(previousIndex, commaIndex).toInt();
      // Actualizamos el índice previo al nuevo comienzo
      previousIndex = commaIndex + 1;
    }
    l = rec[0];
    n = rec[1];
    m = rec[2];
    c = rec[3];
  } else if (data[0] == 'L') {
    String actual = "";
    for (int i = 0; i < luc; i++) {
      // Encontramos la siguiente coma
      commaIndex = data.indexOf(',', previousIndex);
      // Si no se encuentra más comas, se procesa el último valor
      if (commaIndex == -1) {
        commaIndex = data.length();
      }
      actual = data.substring(previousIndex, commaIndex);
      int split1 = 0;
      int inde1 = 0;
      for (int j = 0; j < 3; j++) {
        // Extraemos el valor entre los puntos
        split1 = actual.indexOf('.', inde1);
        if (split1 == -1) {
          split1 = data.length();
        }
        if(j==0){
        luces[i].R = data.substring(inde1, split1).toInt(); // Instanciamos el Red del led I
        }
        if(j==1){
        luces[i].G = data.substring(inde1, split1).toInt(); // Green
        }
        if(j==2){
        luces[i].B = data.substring(inde1, split1).toInt(); // Blue
        }
        inde1 = split1 + 1;
      }
      // Actualizamos el índice previo al nuevo comienzo
      previousIndex = commaIndex + 1;
    }
  }
}


void manejo() {
  // Control del servo (dirección)
  sAngle = map(l, 0, 1023, 0, 180);  // traducimos el dato a ángulos del servo
  servo.write(sAngle);               // mueve el servo al ángulo calculado

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
    ledcWrite(M1_PWM_CHANNEL, mSpeed);
    digitalWrite(m1a, HIGH);
    digitalWrite(m1b, LOW);

    ledcWrite(M2_PWM_CHANNEL, mSpeed);
    digitalWrite(m2a, HIGH);
    digitalWrite(m2b, LOW);
  } else {
    // Movimiento hacia atrás
    ledcWrite(M1_PWM_CHANNEL, abs(mSpeed));
    digitalWrite(m1a, LOW);
    digitalWrite(m1b, HIGH);

    ledcWrite(M2_PWM_CHANNEL, abs(mSpeed));
    digitalWrite(m2a, LOW);
    digitalWrite(m2b, HIGH);
  }
}

void luces() {
   if (m == 1) {
    for (int i = 0; i < luc; i++) {
      arriba.setPixelColor(i, arriba.Color(luces[i].R, luces[i].G, luces[i].B));
      arriba.show();
      delay(10);
    }
  } else if (m == 2) {
    rainbow();
  }
}

void rainbow(){
for (int i = 0; i < arriba.numPixels(); i++) {
    arriba.setPixelColor(i, arriba.ColorHSV((i * 65536 / arriba.numPixels()), 255, 255));
  }
  arriba.show();
}
