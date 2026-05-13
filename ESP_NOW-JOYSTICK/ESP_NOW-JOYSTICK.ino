// ===== CONTROLE REMOTO ESPNOW (TRANSMISSOR) - VERSÃO CORRIGIDA V3.0+ =====

#include <esp_now.h>
#include <WiFi.h>

// Pinos do Joystick
#define VRX 34
#define VRY 35

// MAC Address do receptor (CARRINHO)
uint8_t enderecoMAC[] = {0x3C, 0x71, 0xBF, 0x4D, 0xB4, 0x1C};

struct Dados {
  int x;
  int y;
};

Dados dados;
esp_now_peer_info_t peerInfo;

// ESTA É A FUNÇÃO QUE PRECISAVA DE AJUSTE:
// O primeiro parâmetro agora deve ser do tipo 'const wifi_tx_info_t *' nas versões novas
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Status do Envio: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}


// Se a de cima falhar, use esta:
/*
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Mesma lógica
}
*/

void setup() {
  Serial.begin(115200);

  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  // O segredo está aqui: o cast (esp_now_send_cb_t) força o compilador a aceitar a função
  esp_now_register_send_cb((esp_now_send_cb_t)onDataSent);

  memcpy(peerInfo.peer_addr, enderecoMAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Erro ao adicionar peer");
    return;
  }
}

void loop() {
  dados.x = analogRead(VRX);
  dados.y = analogRead(VRY);

  esp_now_send(enderecoMAC, (uint8_t *) &dados, sizeof(dados));

  Serial.printf("X: %d | Y: %d\n", dados.x, dados.y);

  delay(50); 
}