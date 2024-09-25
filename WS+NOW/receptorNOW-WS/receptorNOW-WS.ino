#include <esp_now.h>
#include <Adafruit_NeoPixel.h>

// Pines de motores
#define m1s 26  // PWM Motor 1
#define m1a 25  // Motor 1 A
#define m1b 33  // Motor 1 B
#define m2s 27  // PWM Motor 2
#define m2a 14  // Motor 2 A
#define m2b 12  // Motor 2 B

// Configuración de NeoPixel
#define pin_tira 18
#define NUM_PIXELS 64
Adafruit_NeoPixel arriba = Adafruit_NeoPixel(NUM_PIXELS, pin_tira, NEO_GRB + NEO_KHZ800);

// Estructura de mensaje
typedef struct struct_message {
  int messageType;  // 0 = Motores, 1 = Luces
  int motor1Speed;
  int motor2Speed;
  bool motor1Direction;  // 0: Adelante, 1: Atrás
  bool motor2Direction;
  int ledIndex;  // Índice del píxel a controlar
  int r, g, b;   // Valores RGB para el píxel
} struct_message;

struct_message incomingMessage;

// Callback cuando recibimos un mensaje por ESP-NOW
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int len) {
  memcpy(&incomingMessage, data, sizeof(incomingMessage));

  if (incomingMessage.messageType == 0) {
    // Control de motores
    analogWrite(m1s, incomingMessage.motor1Speed);
    digitalWrite(m1a, incomingMessage.motor1Direction ? LOW : HIGH);
    digitalWrite(m1b, incomingMessage.motor1Direction ? HIGH : LOW);

    analogWrite(m2s, incomingMessage.motor2Speed);
    digitalWrite(m2a, incomingMessage.motor2Direction ? LOW : HIGH);
    digitalWrite(m2b, incomingMessage.motor2Direction ? HIGH : LOW);
  } 
  else if (incomingMessage.messageType == 1) {
    // Control de luces NeoPixel
    if (incomingMessage.ledIndex >= 0 && incomingMessage.ledIndex < arriba.numPixels()) {
      arriba.setPixelColor(incomingMessage.ledIndex, arriba.Color(incomingMessage.r, incomingMessage.g, incomingMessage.b));
      arriba.show();
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }
  
  // Actualiza el callback de recepción de ESP-NOW con el nuevo formato de función
  esp_now_register_recv_cb(OnDataRecv);

  // Configurar NeoPixel
  arriba.begin();
  arriba.setBrightness(255);

  // Configurar pines de motores
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);
}

void loop() {
  // El receptor no necesita hacer nada en el loop, todo se maneja a través de ESP-NOW
}
