
const int IN1 = 10;
const int IN2 = 9;
const int sensor = 11;

void setup() {

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(sensor, INPUT);

  
}

void loop() {
  int estado = digitalRead(sensor);

  if (estado == HIGH){
    pararMotor();
  }
  else {
    girarFrente(180);
  }

}

// Função para girar em um sentido
void girarFrente(int velocidade) {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);


}

// Função para girar no sentido contrário
void girarReverso(int velocidade) {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);


}

// Função para parar o motor
void pararMotor() {


  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

}
