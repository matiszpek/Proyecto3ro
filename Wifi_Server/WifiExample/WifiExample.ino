#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_NeoPixel.h>

#define SSID "IOT"
#define PASSWORD "INVENTARONELVAR"
#define NUM_PIXELS 64
#define PIN_TIRA 7

Adafruit_NeoPixel arriba = Adafruit_NeoPixel(NUM_PIXELS, PIN_TIRA, NEO_GRB + NEO_KHZ800);

WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  arriba.begin();
  arriba.setBrightness(255);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("WiFi conectado");

  webSocket.begin("tu_servidor", 3000, "/");  // Dirección del servidor WebSocket
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();  // Mantener la conexión activa
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*) payload);
    Serial.println("Mensaje recibido: " + message);
    
    if (message.indexOf("changeColor") >= 0) {
      // Parseamos el color
      int colorStart = message.indexOf(':') + 1;
      String color = message.substring(colorStart);
      
      if (color == "red") {
        setColor(255, 0, 0);
      } else if (color == "green") {
        setColor(0, 255, 0);
      } else if (color == "blue") {
        setColor(0, 0, 255);
      }
    }
  }
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    arriba.setPixelColor(i, arriba.Color(r, g, b));
  }
  arriba.show();
}
