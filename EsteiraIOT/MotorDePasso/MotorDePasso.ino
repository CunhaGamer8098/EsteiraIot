
#include <Stepper.h>

// O 28BYJ-48 possui aproximadamente 2048 passos
// por volta na configuração de passo completo.
const int passosPorVolta = 2048;

// Ordem dos pinos para o ULN2003
Stepper motor(passosPorVolta, 8, 10, 9, 11);

void setup() {
  motor.setSpeed(15); // RPM
}

void loop() {

  // Gira 360 graus no sentido horário
  motor.step(passosPorVolta);

  delay(2000);

  // Gira 360 graus no sentido anti-horário
  motor.step(-passosPorVolta);

  delay(2000);
}
