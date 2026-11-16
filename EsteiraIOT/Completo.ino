#include <Stepper.h>
#include <WiFi.h>          // ESP32
#include <PubSubClient.h>

/*
=================================================
CONFIGURACOES WIFI E MQTT
=================================================
*/

const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

/*
=================================================
ESTEIRA
=================================================
*/

const int IN1 = 2;
const int IN2 = 3;

/*
=================================================
MOTOR DE PASSO DA PAREDE
=================================================
*/

const int passosPorVolta = 2048;

Stepper motor(passosPorVolta, 8, 10, 9, 11);

const int CURSO_MAX_PASSOS = 1000;

int posicaoParede = 0;

/*
=================================================
SENSORES
=================================================
*/

const int sensorIR = 12;

const int trigAltura = 4;
const int echoAltura = 5;

const int trigLargura = 6;
const int echoLargura = 7;

/*
=================================================
PARAMETROS DE MEDICAO
=================================================
*/

const float velocidadeEsteira = 10.0; // cm/s
const float margemErro = 1.5;

const float alturaReferencia = 14.0;
const float larguraReferencia = 20.0; // AJUSTAR

/*
=================================================
CONTROLE
=================================================
*/

int estadoAnterior = LOW;

/*
=================================================
WIFI
=================================================
*/

void conectarWifi() {

  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
}

/*
=================================================
MQTT
=================================================
*/

void reconnectMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando MQTT...");

    String clientId = "ClassificadorLEGO-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(
          clientId.c_str(),
          mqtt_username,
          mqtt_password)) {

      Serial.println(" conectado");

    } else {

      Serial.print(" falhou rc=");
      Serial.print(client.state());

      delay(2000);
    }
  }
}

void enviarMQTT(
    float altura,
    float largura,
    float comprimento,
    float volume) {

  String payload = "{";

  payload += "\"altura\":";
  payload += String(altura, 2);

  payload += ",\"largura\":";
  payload += String(largura, 2);

  payload += ",\"comprimento\":";
  payload += String(comprimento, 2);

  payload += ",\"volume\":";
  payload += String(volume, 2);

  payload += "}";

  client.publish(
      "projeto/esteira/medidas",
      payload.c_str());

  Serial.println("Dados enviados MQTT");
}

/*
=================================================
SETUP
=================================================
*/

void setup() {
    
  Serial.begin(115200);

  randomSeed(micros());

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  motor.setSpeed(15);

  pinMode(sensorIR, INPUT);

  pinMode(trigAltura, OUTPUT);
  pinMode(echoAltura, INPUT);

  pinMode(trigLargura, OUTPUT);
  pinMode(echoLargura, INPUT);

  estadoAnterior = digitalRead(sensorIR);

  conectarWifi();

  client.setServer(mqtt_broker, mqtt_port);

  Serial.println("Sistema iniciado");
}

/*
=================================================
LOOP
=================================================
*/

void loop() {

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  int estado = digitalRead(sensorIR);

  /*
    PECA DETECTADA
  */

  if (estado == HIGH && estadoAnterior == LOW) {

    Serial.println();
    Serial.println("PECA DETECTADA");

    /*
      1 - PARA ESTEIRA
    */

    pararMotor();

    /*
      2 - FECHA PAREDE
    */

    fecharParede();

    delay(1000);

    /*
      3 - ABRE PAREDE
    */

    separarParede();

    delay(500);

    /*
      4 - RELIGA A ESTEIRA
    */

    girarFrente();

    /*
      5 - MEDIR COMPRIMENTO
      A PECA ESTA EM MOVIMENTO
    */

    float comprimento =
        calcularComprimento(
            trigAltura,
            echoAltura);

    /*
      6 - MEDIR ALTURA
    */

    float altura =
        calcularAltura(
            trigAltura,
            echoAltura);

    /*
      7 - MEDIR LARGURA
    */

    float largura =
        calcularLargura(
            trigLargura,
            echoLargura);

    /*
      8 - CALCULAR VOLUME
    */

    float volume =
        altura *
        largura *
        comprimento;

    /*
      9 - EXIBIR
    */

    Serial.println();
    Serial.println("===== RESULTADO =====");

    Serial.print("Altura: ");
    Serial.print(altura);
    Serial.println(" cm");

    Serial.print("Largura: ");
    Serial.print(largura);
    Serial.println(" cm");

    Serial.print("Comprimento: ");
    Serial.print(comprimento);
    Serial.println(" cm");

    Serial.print("Volume: ");
    Serial.print(volume);
    Serial.println(" cm3");

    /*
      10 - MQTT
    */

    enviarMQTT(
        altura,
        largura,
        comprimento,
        volume);
  }

  estadoAnterior = estado;
}

/*
=================================================
ESTEIRA
=================================================
*/

void girarFrente() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void pararMotor() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

/*
=================================================
PAREDE
=================================================
*/

void fecharParede() {

  int passos =
      CURSO_MAX_PASSOS - posicaoParede;

  if (passos > 0) {

    motor.step(passos);

    posicaoParede += passos;
  }
}

void separarParede() {

  int passos = posicaoParede;

  if (passos > 0) {

    motor.step(-passos);

    posicaoParede -= passos;
  }
}

/*
=================================================
ULTRASSONICO
=================================================
*/

float calcularDistancia(
    int trig,
    int echo,
    float referencia) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  unsigned long duracao =
      pulseIn(echo, HIGH, 30000);

  if (duracao == 0) {
    return -1;
  }

  float distancia =
      duracao * 0.0343 / 2.0;

  float medida =
      referencia - distancia;

  if (medida < 0) {
    medida = 0;
  }

  return medida;
}

/*
=================================================
ALTURA
=================================================
*/

float calcularAltura(int trig, int echo) {

  float altura =
      calcularDistancia(
          trig,
          echo,
          alturaReferencia);

  return altura;
}

/*
=================================================
LARGURA
=================================================
*/

float calcularLargura(int trig, int echo) {

  float largura =
      calcularDistancia(
          trig,
          echo,
          larguraReferencia);

  return largura;
}

/*
=================================================
COMPRIMENTO
=================================================
*/

float calcularComprimento(
    int trig,
    int echo) {

  float leitura;

  Serial.println(
      "Aguardando passagem da peca...");

  do {

    leitura =
        calcularDistancia(
            trig,
            echo,
            alturaReferencia);

  } while (leitura <= margemErro);

  unsigned long inicio =
      millis();

  int leiturasAusentes = 0;

  while (leiturasAusentes < 2) {

    leitura =
        calcularDistancia(
            trig,
            echo,
            alturaReferencia);

    if (leitura < 0) {
      continue;
    }

    if (leitura <= margemErro) {

      leiturasAusentes++;

    } else {

      leiturasAusentes = 0;
    }
  }

  unsigned long fim =
      millis();

  float segundos =
      (fim - inicio) / 1000.0;

  float comprimento =
      velocidadeEsteira *
      segundos;

  return comprimento;
}
