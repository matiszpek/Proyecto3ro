#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>  // Librería para controlar el servo
#include <ESP32AnalogWrite.h>  // Librería para analogWrite en ESP32

// Pines del motor y luces
#define motorPin1 26
#define motorPin2 27
#define motorDirection1 12
#define motorDirection2 13
#define servoPin 15

// Configuración de NeoPixel
#define pin_tira 18
#define NUM_PIXELS 64
Adafruit_NeoPixel arriba = Adafruit_NeoPixel(NUM_PIXELS, pin_tira, NEO_GRB + NEO_KHZ800);

// Configuración del servo
Servo myServo;

// Deadzone para el control de los encoders
const int deadZone = 50;  // Rango muerto de ±50

// Estructura para las luces
typedef struct struct_luces {
  int ledIndex;
  int r, g, b;
} struct_luces;

// Estructura para los motores
typedef struct struct_motores {
  long encoderValue1;
  long encoderValue2;
} struct_motores;

struct_luces luces;
struct_motores motores;

// Callback para ESP-NOW
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  if (len == sizeof(struct_luces)) {
    // Es un mensaje de luces
    memcpy(&luces, data, sizeof(luces));
    
    // Actualizar NeoPixel
    arriba.setPixelColor(luces.ledIndex, arriba.Color(luces.r, luces.g, luces.b));
    arriba.show();
    
  } else if (len == sizeof(struct_motores)) {
    // Es un mensaje de motores
    memcpy(&motores, data, sizeof(motores));
    
    // Aplicar deadzone para los motores
    int motorPWM1 = 0;
    int motorPWM2 = 0;
    
    if (motores.encoderValue1 > deadZone) {
      motorPWM1 = map(motores.encoderValue1, deadZone, 1000, 0, 255);
      digitalWrite(motorDirection1, HIGH);  // Adelante
    } else if (motores.encoderValue1 < -deadZone) {
      motorPWM1 = map(motores.encoderValue1, -deadZone, -1000, 0, 255);
      digitalWrite(motorDirection1, LOW);  // Atrás
    }
    
    if (motores.encoderValue2 > deadZone) {
      motorPWM2 = map(motores.encoderValue2, deadZone, 1000, 0, 255);
      digitalWrite(motorDirection2, HIGH);  // Adelante
    } else if (motores.encoderValue2 < -deadZone) {
      motorPWM2 = map(motores.encoderValue2, -deadZone, -1000, 0, 255);
      digitalWrite(motorDirection2, LOW);  // Atrás
    }

    // Aplicar PWM a los motores
    analogWrite(motorPin1, motorPWM1);
    analogWrite(motorPin2, motorPWM2);

    // Ajustar la posición del servo
    int servoPos = map(motores.encoderValue1, -1000, 1000, 0, 180);
    myServo.write(servoPos);
    
  } else {
    // Mensaje erróneo
    Serial.println("Error en el mensaje recibido");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configurar NeoPixel
  arriba.begin();
  arriba.show();
  
  // Configurar Servo
  myServo.attach(servoPin);
  
  // Configurar pines de dirección para el puente H
  pinMode(motorDirection1, OUTPUT);
  pinMode(motorDirection2, OUTPUT);

  // Configurar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // No es necesario hacer nada en loop para este receptor
}
