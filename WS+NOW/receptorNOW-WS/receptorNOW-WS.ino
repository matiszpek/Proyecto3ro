#include <WiFi.h>
#include <WebSocketsClient.h>
#include <esp_now.h>

#define NUM_PIXELS 64
#define PIXELS_PER_PACKET 16
#define MESSAGE_LENGTH (PIXELS_PER_PACKET * 3)
#define WEBSOCKET_BUFFER_SIZE 250

uint8_t neoPixelData[NUM_PIXELS * 3];  // Para almacenar los datos de todos los píxeles

WebSocketsClient webSocket;

// Función de callback ESP-NOW
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

// Enviar bloque de datos de NeoPixel en partes
void enviarDatosNeoPixelDividido() {
  for (int i = 0; i < NUM_PIXELS; i += PIXELS_PER_PACKET) {
    uint8_t buffer[MESSAGE_LENGTH];
    memcpy(buffer, &neoPixelData[i * 3], MESSAGE_LENGTH);
    Serial.println(buffer);
    esp_now_send(NULL, buffer, sizeof(buffer));  // Envía al receptor registrado
    delay(10);  // Delay para evitar sobrecargar el canal
  }
}

// Función de recepción de datos WebSocket
void handleWebSocketMessage(String message) {
  // Asumiendo que recibes los datos en formato "index,R,G,B,index,R,G,B,..."
  int index = 0;
  char *token = strtok((char *)message.c_str(), ",");
  while (token != NULL) {
    int pixelIndex = atoi(token);  // Índice del píxel
    token = strtok(NULL, ",");
    if (token == NULL) break;
    int r = atoi(token);
    token = strtok(NULL, ",");
    if (token == NULL) break;
    int g = atoi(token);
    token = strtok(NULL, ",");
    if (token == NULL) break;
    int b = atoi(token);

    // Guardar los valores RGB en el array de píxeles
    neoPixelData[pixelIndex * 3] = r;
    neoPixelData[pixelIndex * 3 + 1] = g;
    neoPixelData[pixelIndex * 3 + 2] = b;

    token = strtok(NULL, ",");
  }

  // Enviar datos a través de ESP-NOW
  enviarDatosNeoPixelDividido();
}

// Función de callback de WebSocket para recibir mensajes
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
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
      handleWebSocketMessage((char*)payload);
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

  // Rellenar con valores iniciales (negros)
  memset(neoPixelData, 0, sizeof(neoPixelData));
}

void loop() {
  webSocket.loop();
}
