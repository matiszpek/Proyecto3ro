#include <esp_now.h>
#include <WiFi.h>

// Estructura para el envío de datos por ESP-NOW (control de motores)
typedef struct struct_message {
  int motor1Speed;
  int motor2Speed;
  bool motor1Direction;  // 0: Adelante, 1: Atrás
  bool motor2Direction;
} struct_message;

struct_message outgoingMessage;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback cuando el mensaje se envía exitosamente
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del envío: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito" : "Fallo");
}

// Función para escanear y encontrar el receptor basado en su SSID
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();

  for (int i = 0; i < scanResults; ++i) {
    String SSID = WiFi.SSID(i);
    String BSSIDstr = WiFi.BSSIDstr(i);

    if (SSID.indexOf("RX") == 0) {  // Asume que el receptor tiene un SSID que empieza con "RX"
      int mac[6];
      if (6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
        for (int ii = 0; ii < 6; ++ii) {
          peerInfo.peer_addr[ii] = (uint8_t) mac[ii];
        }
        peerInfo.channel = 0; // Ajusta el canal si es necesario
        peerInfo.encrypt = 0; // Sin encriptación
        break;
      }
    }
  }
}

// Inicializar ESP-NOW
void initESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);  // Callback para envío de datos
}

// Simulación de controles para motores
void simulateMotorControls() {
  // Aquí puedes poner tu código para capturar los valores reales de los controles
  outgoingMessage.motor1Speed = analogRead(34); // Ejemplo de lectura de un pin analógico (GPIO 34)
  outgoingMessage.motor2Speed = analogRead(35); // Ejemplo de lectura de otro pin analógico (GPIO 35)
  
  // Control de dirección (ADELANTE o ATRÁS) dependiendo de condiciones o entradas
  outgoingMessage.motor1Direction = outgoingMessage.motor1Speed > 500;  // Lógica simple de ejemplo
  outgoingMessage.motor2Direction = outgoingMessage.motor2Speed > 500;
}

void setup() {
  Serial.begin(115200);

  // Inicializar ESP-NOW
  initESPNow();
  
  // Escanear para encontrar al receptor
  WiFi.disconnect(); // Desconectar cualquier conexión previa
  ScanForSlave();  // Busca el receptor
  
  // Añadir al receptor encontrado como peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar peer");
    return;
  }
}

void loop() {
  // Simular los controles de motores o capturar entradas reales
  simulateMotorControls();

  // Enviar el mensaje a través de ESP-NOW
  esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));

  if (result == ESP_OK) {
    Serial.println("Mensaje enviado exitosamente");
  } else {
    Serial.println("Error al enviar el mensaje");
  }

  delay(100);  // Ajustar según la frecuencia de actualización deseada
}
