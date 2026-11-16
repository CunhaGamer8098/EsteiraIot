
const int trig1 = 4;
const int echo1 = 5;
const int trig2 = 6;
const int echo2 = 7;

const float velocidadeEsteira = 10.0; // cm/s
const float margemErro = 1.5;         // cm
const float alturaReferencia = 14.0; // cm

void setup() {
  Serial.begin(9600);

  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
}

void loop() {

  // Esteira
  calcularAltura(trig1, echo1);
  calcularComprimento(trig1, echo1);

  // Teto
  calcularLargura(trig2, echo2);

  delay(500);
}

void calcularLargura(int trig, int echo) {

  float largura = calcularDistancia(trig, echo);

  Serial.print("Largura: ");

  if (largura >= 0) {
    Serial.print(largura);
    Serial.println(" cm");
  } else {
    Serial.println("Sem leitura");
  }
}

void calcularAltura(int trig, int echo) {

  float altura = calcularDistancia(trig, echo);

  Serial.print("Altura: ");

  if (altura >= 0) {
    Serial.print(altura);
    Serial.println(" cm");
  } else {
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
