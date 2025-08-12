#include "BluetoothA2DPSource.h"
extern BluetoothA2DPSource a2dp_source;

void setupSensors()
{
  // Configura os pinos dos sensores
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
}

float lerSensor1()
{
  // Limpa o pino TRIG
  digitalWrite(TRIG_PIN_1, LOW);
  delayMicroseconds(2);

  // Envia pulso de 10µs
  digitalWrite(TRIG_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_1, LOW);

  // Lê o tempo de retorno do eco
  float duracao = pulseIn(ECHO_PIN_1, HIGH);

  // Calcula a distância (velocidade do som: 343m/s)
  float distancia = (duracao * 0.0343) / 2;

  return distancia;
}

float lerSensor2()
{
  // Limpa o pino TRIG
  digitalWrite(TRIG_PIN_2, LOW);
  delayMicroseconds(2);

  // Envia pulso de 10µs
  digitalWrite(TRIG_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_2, LOW);

  // Lê o tempo de retorno do eco
  float duracao = pulseIn(ECHO_PIN_2, HIGH);

  // Calcula a distância (velocidade do som: 343m/s)
  float distancia = (duracao * 0.0343) / 2;

  return distancia;
}

float calcularDistanciaReal(float distancia_medida)
{
  // Calcula a distância real considerando o ângulo de 45°
  // Usando o teorema de Pitágoras para o triângulo retângulo
  // A hipotenusa é a distância medida pelo sensor
  // Queremos calcular o cateto adjacente (distância horizontal)

  // Para um ângulo de 45°, os catetos são iguais
  // Portanto, cateto = hipotenusa / sqrt(2)
  float distancia_real = distancia_medida / 1.414; // sqrt(2) ≈ 1.414

  // Ajuste baseado na altura da pessoa
  // Se a distância for muito pequena, pode indicar um obstáculo no chão
  if (distancia_real < altura_pessoa * 0.3)
  {
    // Ajuste para evitar falsos positivos com o chão
    return (distancia_real > 30.0f) ? distancia_real : 30.0f; // Mínimo de 30cm para evitar alertas constantes
  }

  return distancia_real;
}

void calcularParametrosAudio()
{
  // Calcula os parâmetros de áudio baseados nas distâncias
  // Sensor 1 (frente) controla o canal esquerdo
  // Sensor 2 (baixo) controla o canal direito

  // Limites de distância para alertas
  const float distancia_maxima = 200.0; // cm
  const float distancia_minima = 30.0;  // cm

  // Limites de frequência
  const int freq_minima = 200;  // Hz
  const int freq_maxima = 2000; // Hz

  // Mapeia a distância para volume (mais perto = mais alto)
  volume_esquerdo = map(constrain(distancia_sensor1, distancia_minima, distancia_maxima), distancia_minima, distancia_maxima, 50, 10);
  volume_direito = map(constrain(distancia_real_sensor2, distancia_minima, distancia_maxima), distancia_minima, distancia_maxima, 50, 10);

  // Mapeia a distância para frequência (mais perto = frequência mais alta)
  frequencia_esquerda = map(constrain(distancia_sensor1, distancia_minima, distancia_maxima), distancia_minima, distancia_maxima, freq_maxima, freq_minima);
  frequencia_direita = map(constrain(distancia_real_sensor2, distancia_minima, distancia_maxima), distancia_minima, distancia_maxima, freq_maxima, freq_minima);
}