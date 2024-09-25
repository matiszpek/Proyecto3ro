#include <WiFi.h>
#include <WebSocketsClient.h>
#include <esp_now.h>

// Configuración
#define NUM_PIXELS 64
#define PIXELS_PER_PACKET 16
#define MESSAGE_LENGTH (PIXELS_PER_PACKET * 3)
#define WEBSOCKET_BUFFER_SIZE 250

// WebSocket
WebSocketsClient webSocket;

// Estructura de mensaje para enviar por ESP-NOW
typedef struct struct_message {
  int messageType;  // 0 = Motores, 1 = Luces
  int motor1Speed;
  int motor2Speed;
  bool motor1Direction;  // 0: Adelante, 1: Atrás
  bool motor2Direction;
  int ledIndex;  // Índice del píxel a controlar
  int r, g, b;   // Valores RGB para el píxel
} struct_message;

struct_message outgoingMessage;

// Función de callback de ESP-NOW
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado de envío:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado" : "Fallo");
}

// Configurar ESP-NOW
void setupESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
}

// Función para enviar datos a través de ESP-NOW
void enviarDatosESPNow() {
  esp_now_send(NULL, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));
}

// Procesar mensajes de WebSocket
void handleWebSocketMessage(String message) {
  // Asumiendo que los mensajes de la página web contienen "type,speed1,direction1,speed2,direction2" para motores
  // y "type,index,R,G,B" para luces
  char *token = strtok((char *)message.c_str(), ",");
  int messageType = atoi(token);  // Tipo de mensaje

  if (messageType == 0) {
    // Mensaje de control de motores
    outgoingMessage.messageType = 0;  // Motores
    token = strtok(NULL, ",");
    outgoingMessage.motor1Speed = atoi(token);
    token = strtok(NULL, ",");
    outgoingMessage.motor1Direction = atoi(token);
    token = strtok(NULL, ",");
    outgoingMessage.motor2Speed = atoi(token);
    token = strtok(NULL, ",");
    outgoingMessage.motor2Direction = atoi(token);

    // Enviar datos de motores por ESP-NOW
    enviarDatosESPNow();

  } else if (messageType == 1) {
    // Mensaje de control de luces
    outgoingMessage.messageType = 1;  // Luces
    token = strtok(NULL, ",");
    outgoingMessage.ledIndex = atoi(token);
    token = strtok(NULL, ",");
    outgoingMessage.r = atoi(token);
    token = strtok(NULL, ",");
    outgoingMessage.g = atoi(token);
    token = strtok(NULL, ",");
    outgoingMessage.b = atoi(token);

    // Enviar datos de luces por ESP-NOW
    enviarDatosESPNow();
  }
}

// Callback de eventos de WebSocket
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Desconectado");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Conectado");
      webSocket.sendTXT("Conectado");
      break;
    case WStype_TEXT:
      Serial.printf("Mensaje Recibido: %s\n", payload);
      handleWebSocketMessage(String((char*)payload));
      break;
    case WStype_BIN:
      Serial.println("Mensaje Binario recibido");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Conexión WiFi
  WiFi.begin("IOTB", "inventaronelVAR");  // Configura tu WiFi aquí
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Inicializar WebSocket
  webSocket.begin(".lclx.io", 80, "/ws");  // Cambia el URL y puerto
  webSocket.onEvent(webSocketEvent);

  // Configurar ESP-NOW
  setupESPNow();
}

void loop() {
  webSocket.loop();
}
