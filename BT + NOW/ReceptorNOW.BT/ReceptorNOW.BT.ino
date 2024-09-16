#include <esp_now.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <Adafruit_NeoPixel.h>

BluetoothSerial SerialBT;  // Inicializa Bluetooth

#define pin_tira 18  // Pin para la tira de NeoPixel

// Pines de motores
#define m1s 26  // PWM Motor 1
#define m1a 25  // Motor 1 A
#define m1b 33  // Motor 1 B

#define m2s 27  // PWM Motor 2
#define m2a 14  // Motor 2 A
#define m2b 12  // Motor 2 B

// Configuración de NeoPixel
Adafruit_NeoPixel arriba = Adafruit_NeoPixel(64, pin_tira, NEO_GRB + NEO_KHZ800);

// Estructura para el envío de datos por ESP-NOW (control de motores)
typedef struct struct_message {
  int motor1Speed;
  int motor2Speed;
  bool motor1Direction;  // 0: Adelante, 1: Atrás
  bool motor2Direction;
} struct_message;

struct_message incomingMessage;

// Callback cuando recibimos un mensaje por ESP-NOW
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  memcpy(&incomingMessage, data, sizeof(incomingMessage));

  // Controlar Motor 1
  analogWrite(m1s, incomingMessage.motor1Speed);
  digitalWrite(m1a, incomingMessage.motor1Direction ? LOW : HIGH);
  digitalWrite(m1b, incomingMessage.motor1Direction ? HIGH : LOW);

  // Controlar Motor 2
  analogWrite(m2s, incomingMessage.motor2Speed);
  digitalWrite(m2a, incomingMessage.motor2Direction ? LOW : HIGH);
  digitalWrite(m2b, incomingMessage.motor2Direction ? HIGH : LOW);
}

// Inicializar ESP-NOW
void initESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);  // Callback para recepción de datos
}

// Configuración de Bluetooth
void setupBluetooth() {
  SerialBT.begin("ESP32_Luces");  // Nombre Bluetooth
  Serial.println("Bluetooth listo para conectar");
}

// Procesar comandos recibidos por Bluetooth para controlar los NeoPixel
void processBluetoothCommand() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');  // Leer comando
    Serial.print(command);
    // Comandos RGB individuales para cada píxel, por ejemplo: "R,G,B"
    int index = 0;  // Posición del píxel a cambiar
    int r, g, b;
    if (sscanf(command.c_str(), "%d,%d,%d,%d", &index, &r, &g, &b) == 4) {
      if (index >= 0 && index < arriba.numPixels()) {
        arriba.setPixelColor(index, arriba.Color(r, g, b));  // Setear color en el píxel
        arriba.show();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar pines de motores
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);

  // Configurar NeoPixel
  arriba.begin();
  arriba.setBrightness(255);

  // Inicializar ESP-NOW
  initESPNow();

  // Inicializar Bluetooth
  setupBluetooth();
}

void loop() {
  // Procesar comandos por Bluetooth para controlar los NeoPixel
  processBluetoothCommand();

  // Aquí puedes añadir otras lógicas o retrasos si es necesario
  delay(10);
}
