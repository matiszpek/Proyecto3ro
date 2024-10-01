#include <esp_now.h>
#include <WiFi.h>
// #include <WebSocketsClient.h> // Comentado para no perder la parte del código

// Pines de encoders
#define encoderPinA1 2
#define encoderPinB1 3
#define encoderPinA2 4
#define encoderPinB2 5

// Configuración de ESP-NOW
uint8_t receptorAddress[] = {0x24, 0x6F, 0x28, 0xAE, 0xD4, 0xC8};  // MAC del receptor

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

// Inicializamos los encoders
// Encoder encoder1(encoderPinA1, encoderPinB1);
// Encoder encoder2(encoderPinA2, encoderPinB2);

// Función para enviar datos por ESP-NOW
void enviarESPNow(void* data, size_t dataSize) {
  esp_err_t result = esp_now_send(receptorAddress, (uint8_t *)data, dataSize);
  if (result != ESP_OK) {
    Serial.println("Error al enviar mensaje");
  }
}

// WebSocket callback (comentado)
// void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
//   switch (type) {
//     case WStype_DISCONNECTED:
//       Serial.println("WebSocket desconectado");
//       break;
//     case WStype_CONNECTED:
//       Serial.println("WebSocket conectado");
//       break;
//     case WStype_TEXT:
//       // Parsear los datos de la configuración de luces
//       sscanf((char*)payload, "%d,%d,%d,%d", &luces.ledIndex, &luces.r, &luces.g, &luces.b);
//       // Enviar configuración de luces por ESP-NOW
//       enviarESPNow(&luces, sizeof(luces));
//       break;
//   }
// }

void setup() {
  Serial.begin(115200);
  
  // Conectar a WiFi
   WiFi.begin("Kono", "matiasmayans");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  } 
  
  // Configuración de ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receptorAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el peer");
  }

  // Inicializar valores de encoders (sin usar encoders físicos)
  motores.encoderValue1 = 100;  // Valor fijo para encoder 1
  motores.encoderValue2 = 1000;  // Valor fijo para encoder 2
}

void loop() {
  // Enviar valores de los encoders por ESP-NOW 
  enviarESPNow(&motores, sizeof(motores));

  // Manejar WebSocket (comentado)
  // webSocket.loop();
  
  delay(1000);  // Esperar 1 segundo antes de enviar nuevamente
}
