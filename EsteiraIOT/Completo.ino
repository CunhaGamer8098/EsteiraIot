#include <Stepper.h>
// Inicializar motores responsáveis
const int IN1 = 2;
const int IN2 = 3;

// O 28BYJ-48 possui aproximadamente 2048 passos
// por volta na configuração de passo completo.
const int passosPorVolta = 2048;

// Ordem dos pinos para o ULN2003
Stepper motor(passosPorVolta, 8, 10, 9, 11);

// Sensores, infravermelho e ultrassônico
const int sensor = 12;

const int trig1 = 4;
const int echo1 = 5;
const int trig2 = 6;
const int echo2 = 7;

// Variáveis de apoio para os sensores ultrassônicos
const float velocidadeEsteira = 10.0; // cm/s
const float margemErro = 1.5;         // cm
const float alturaReferencia = 14.0; // cm

void setup(){
    Serial.begin(9600);
    // motores
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    motor.setSpeed(15); // RPM

    // Sensores
    pinMode(sensor, INPUT);

    pinMode(trig1, OUTPUT);
    pinMode(echo1, INPUT);
    pinMode(trig2, OUTPUT);
    pinMode(echo2, INPUT);
}

void loop(){
  int estado = digitalRead(sensor);

  if (estado == HIGH){
    pararMotor();
    fecharParede();
  }
  else {
    girarFrente(180);
    separarParede();
  }
}

// Função para movimentar a esteira
void girarFrente(int velocidade) {
    // esteira funcionando de forma padrão
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);


}

// Função para parar a esteira
void pararMotor() {
    // passa a desligar a esteira para que ela pare de se movimentar
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

}
// funções parede
void fecharParede(){
    // Parede, começa a unir para alinhar a caixa
    motor.step(passosPorVolta);

    // colocar uma forma inteligente de se medir a distância
}
void separarParede(){
    // Parede começa a soltar a caixa da parede para andar na esteira
    motor.step(-passosPorVolta);

    // voltar o suficiente para que ele não saia da cremalheira
}

//funções medidas

void calcularLargura(int trig, int echo) {

  float largura = calcularDistancia(trig, echo);

  Serial.print("Largura: ");

  if (largura >= 0) {
    Serial.print(largura);
    Serial.println(" cm");
  } 
  else {
    Serial.println("Sem leitura");
  }
}

void calcularAltura(int trig, int echo) {

  float altura = calcularDistancia(trig, echo);

  Serial.print("Altura: ");

  if (altura >= 0) {
    Serial.print(altura);
    Serial.println(" cm");
  } 
  else {
    Serial.println("Sem leitura");
  }
}

float calcularDistancia(int trig, int echo) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  unsigned long duracao = pulseIn(echo, HIGH, 30000);

  if (duracao == 0) {
    return -1;
  }

  float distancia = duracao * 0.0343 / 2;

  float altura = alturaReferencia - distancia;

  return altura;
}

void calcularComprimento(int trig, int echo) {

  float altura;

  Serial.println("Aguardando objeto...");

  // Aguarda o objeto entrar na região de detecção
  do {
    altura = calcularDistancia(trig, echo);
  } while (altura < 0 || altura <= margemErro);

  // Objeto detectado: inicia a contagem
  unsigned long inicio = millis();

  Serial.println("Objeto detectado!");

  int leiturasAusentes = 0;

  // Aguarda o objeto sair da região de detecção
  while (leiturasAusentes < 2) {

    altura = calcularDistancia(trig, echo);

    if (altura < 0) {
      continue;
    }

    if (altura <= margemErro) {
      leiturasAusentes++;
    } else {
      leiturasAusentes = 0;
    }
  }

  unsigned long fim = millis();

  float segundos = (fim - inicio) / 1000.0;

  float comprimento = velocidadeEsteira * segundos;

  Serial.print("Tempo: ");
  Serial.print(segundos);
  Serial.println(" s");

  Serial.print("Comprimento: ");
  Serial.print(comprimento);
  Serial.println(" cm");
}
