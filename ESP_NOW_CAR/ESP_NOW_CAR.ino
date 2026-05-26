// ===== CARRINHO ESPNOW (RECEPTOR) - VERSÃO CORRIGIDA V3.0+ =====

#include <esp_now.h>
#include <WiFi.h>

// Definição dos Pinos do L298N
#define IN1 33
#define IN2 25
#define IN3 26
#define IN4 27
#define ENA 32
#define ENB 14

struct Dados {
  int x;
  int y;
};

Dados dadosRecebidos;

// Funções de Movimento
void configurarVelocidade(int vel) {
  analogWrite(ENA, vel);
  analogWrite(ENB, vel);
}

void frente() {
  configurarVelocidade(255);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void tras() {
  configurarVelocidade(255);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void esquerda() {
  configurarVelocidade(255);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void direita() {
  configurarVelocidade(255);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void parar() {
  configurarVelocidade(0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// FUNÇÃO DE RECEPÇÃO AJUSTADA
// Nas versões novas, o primeiro parâmetro mudou para esp_now_recv_info_t
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&dadosRecebidos, incomingData, sizeof(dadosRecebidos));

  int x = dadosRecebidos.x;
  int y = dadosRecebidos.y;

  // Lógica de Direção
  if (y > 3500)      frente();
  else if (y < 500)  tras();
  else if (x > 3500) direita();
  else if (x < 500)  esquerda();
  else               parar();

  Serial.printf("Recebido -> X: %d | Y: %d\n", x, y);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  parar();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  // O "cast" (esp_now_recv_cb_t) resolve o erro de compilação na v3.0+
  esp_now_register_recv_cb((esp_now_recv_cb_t)onDataRecv);
  
  Serial.println("Carrinho pronto e aguardando comandos...");
}

void loop() {
  // O processamento acontece todo no onDataRecv
}