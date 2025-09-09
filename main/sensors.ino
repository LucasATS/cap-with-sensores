#include "BluetoothA2DPSource.h"
extern BluetoothA2DPSource a2dp_source;


extern float altura_pessoa;
extern float distancia_sensor1;
extern float distancia_real_sensor2;
extern int volume_esquerdo, volume_direito;
extern int frequencia_esquerda, frequencia_direita;


void setupSensors() {
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
}


float lerSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 25000UL); // Timeout 25ms (~4m)
  if (duracao == 0) return -1.0f; // Sem leitura

  return (duracao * 0.0343f) / 2.0f; // Velocidade do som (cm/us)
}

float lerSensor1() { return lerSensor(TRIG_PIN_1, ECHO_PIN_1); }
float lerSensor2() { return lerSensor(TRIG_PIN_2, ECHO_PIN_2); }


float calcularDistanciaReal(float distancia_medida) {
  if (distancia_medida < 0) return -1;

  // Ajuste para ângulo de 45° usando Pitágoras
  float distancia_real = distancia_medida / 1.414f; // sqrt(2) ≈ 1.414

  // Filtro para evitar falsos positivos com o chão
  float limite_chao = altura_pessoa * 0.3f * 100.0f; // altura em cm
  if (distancia_real < limite_chao) {
    return (distancia_real > 30.0f) ? distancia_real : 30.0f;
  }
  return distancia_real;
}


void calcularParametrosAudio() {
  const int vol_max = 50;
  const int vol_min = 10;
  const int freq_max = 2000; // Hz
  const int freq_min = 200;  // Hz

  const float dist_max_frente = 100.0f; // cm
  const float dist_max_baixo  = 60.0f;  // cm
  const float dist_min        = 30.0f;  // cm

  // Canal esquerdo (frente)
  if (distancia_sensor1 > 0 && distancia_sensor1 <= dist_max_frente) {
    volume_esquerdo = map(constrain(distancia_sensor1, dist_min, dist_max_frente), dist_min, dist_max_frente, 50, 10);
    frequencia_esquerda = map(constrain(distancia_sensor1, dist_min, dist_max_frente), dist_min, dist_max_frente, freq_max, freq_min);
  } else {
    volume_esquerdo = 0;
    frequencia_esquerda = 0;
  }

  // Canal direito (baixo)
  if (distancia_real_sensor2 > 0 && distancia_real_sensor2 <= dist_max_baixo) {
    volume_direito = map(constrain(distancia_real_sensor2, dist_min, dist_max_baixo), dist_min, dist_max_baixo, 50, 10);
    frequencia_direita = map(constrain(distancia_real_sensor2, dist_min, dist_max_baixo), dist_min, dist_max_baixo, freq_max, freq_min);
  } else {
    volume_direito = 0;
    frequencia_direita = 0;
  }
}

