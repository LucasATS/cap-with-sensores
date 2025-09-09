#include "BluetoothA2DPSource.h"

// Instância declarada no bluetooth.ino
extern BluetoothA2DPSource a2dp_source;

// Pinos dos sensores ultrassônicos
#define TRIG_PIN_1 18 // Sensor cima/frente
#define ECHO_PIN_1 19
#define TRIG_PIN_2 22 // Sensor baixo
#define ECHO_PIN_2 23

// Parâmetros fixos
float altura_pessoa = 1.59f; // Altura média da pessoa (m)

// Variáveis de distância
float distancia_sensor1 = 0.0f;
float distancia_sensor2 = 0.0f;
float distancia_real_sensor2 = 0.0f;

// Controle de bipes
const int FREQ_MAX   = 2000;
const int FREQ_MIN   = 200;
int volume_esquerdo = 0;
int volume_direito  = 0;
int frequencia_esquerda = 0;
int frequencia_direita  = 0;

// Nome do dispositivo Bluetooth
const char *device_name = "HZ-BT612"; // Fone gatinho - "HZ-BT612"; // Fone Karina - "FN-B05"; EL-1606

// Controle de tempo de leitura
unsigned long ultimo_tempo_leitura = 0;
const int intervalo_leitura = 100; // ms

// Protótipos
void setupSensors();
float lerSensor1();
float lerSensor2();
float calcularDistanciaReal(float distancia_medida);
void calcularParametrosAudio();
void gerarEarcons(int freq_esq, int freq_dir, int vol_esq, int vol_dir);
void setupBluetooth();
void processarBluetooth();

void setup() {
  Serial.begin(115200);
  Serial.println(F("Inicializando sistema de detecção de obstáculos..."));

  // Inicializa sensores
  setupSensors();
  Serial.println(F("Sensores OK"));

  setupBluetooth();
  Serial.println(F("Bluetooth OK"));

  delay(300);
}

void loop() {
  unsigned long tempo_atual = millis();
  if (tempo_atual - ultimo_tempo_leitura >= intervalo_leitura) {
    ultimo_tempo_leitura = tempo_atual;

    distancia_sensor1 = lerSensor1();
    distancia_sensor2 = lerSensor2();
    distancia_real_sensor2 = calcularDistanciaReal(distancia_sensor2);

    calcularParametrosAudio();

    // Debug
    Serial.printf("Frente: %.1f cm | Baixo: %.1f cm\n", distancia_sensor1, distancia_real_sensor2);
  }

  processarBluetooth();
}
