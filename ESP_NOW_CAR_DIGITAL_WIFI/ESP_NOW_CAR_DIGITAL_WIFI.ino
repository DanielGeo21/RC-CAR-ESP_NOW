#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CarrinhoESP32";
const char* password = "12345678";

WebServer server(80);

// ===== PINOS =====

#define IN1 33
#define IN2 25
#define IN3 26
#define IN4 27

#define ENA 32
#define ENB 14

// ===== HTML =====

String pagina = R"rawliteral(

<!DOCTYPE html>
<html>

<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{
  background:#111;
  text-align:center;
  color:white;
  font-family:Arial;
}

h1{
  margin-top:20px;
}

button{
  width:110px;
  height:110px;
  font-size:35px;
  margin:10px;
  border:none;
  border-radius:20px;
  background:#00aaff;
  color:white;
  cursor:pointer;
  user-select:none;
}

button:active{
  background:#0077aa;
}

</style>

</head>

<body>

<h1>Carrinho ESP32</h1>

<div>
<button id="frente">↑</button>
</div>

<div>
<button id="esquerda">←</button>
<button id="direita">→</button>
</div>

<div>
<button id="tras">↓</button>
</div>

<script>

// função de envio
function send(cmd){
  fetch("/" + cmd).catch(()=>{});
}

// configuração universal (PC + celular)
function bind(btnId, cmd){

  const btn = document.getElementById(btnId);

  // PC
  btn.addEventListener("mousedown", () => send(cmd));
  btn.addEventListener("mouseup", () => send("parar"));

  // celular
  btn.addEventListener("touchstart", (e) => {
    e.preventDefault();
    send(cmd);
  });

  btn.addEventListener("touchend", (e) => {
    e.preventDefault();
    send("parar");
  });
}

bind("frente", "frente");
bind("tras", "tras");
bind("esquerda", "esquerda");
bind("direita", "direita");

</script>

</body>
</html>

)rawliteral";
// ===== VELOCIDADE =====

void velocidade(int vel){

  ledcWrite(ENA, vel);
  ledcWrite(ENB, vel);

}

// ===== MOVIMENTOS =====

void frente(){

  velocidade(128);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void tras(){

  velocidade(128);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void esquerda(){

  velocidade(128);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void direita(){

  velocidade(128);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void parar(){

  velocidade(0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcAttachChannel(ENA, 1000, 8, 0);
ledcAttachChannel(ENB, 1000, 8, 1);

  parar();

  // ===== WIFI =====

  WiFi.softAP(ssid, password);

  Serial.println("");
  Serial.println("WiFi iniciado");
  Serial.println(WiFi.softAPIP());

  // ===== PAGINA =====

  server.on("/", [](){

    server.send(200, "text/html", pagina);

  });

  server.on("/frente", [](){

    frente();
    server.send(200, "text/plain", "ok");

  });

  server.on("/tras", [](){

    tras();
    server.send(200, "text/plain", "ok");

  });

  server.on("/esquerda", [](){

    esquerda();
    server.send(200, "text/plain", "ok");

  });

  server.on("/direita", [](){

    direita();
    server.send(200, "text/plain", "ok");

  });

  server.on("/parar", [](){

    parar();
    server.send(200, "text/plain", "ok");

  });

  server.begin();

  Serial.println("Servidor iniciado");
}

void loop() {

  server.handleClient();

}