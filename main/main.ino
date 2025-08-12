#include "esp_task_wdt.h"
#include "BluetoothA2DPSource.h"
extern BluetoothA2DPSource a2dp_source;

#include "audio_player.h"

#define WATCHDOG_TIMEOUT_SECONDS 5 // Timeout de 5 segundos

// Pinos dos sensores ultrassônicos
#define TRIG_PIN_1 12 // CIMA
#define ECHO_PIN_1 13
#define TRIG_PIN_2 22 // BAIXO
#define ECHO_PIN_2 23

// Parâmetros para cálculo da distância
float altura_pessoa = 1.59; // Altura média da pessoa em metros

// Variáveis para armazenar as distâncias
float distancia_sensor1 = 0;
float distancia_sensor2 = 0;
float distancia_real_sensor2 = 0;

// Variáveis para controle de áudio
int volume_esquerdo = 0;
int volume_direito = 0;
int frequencia_esquerda = 0;
int frequencia_direita = 0;

// Nome do dispositivo Bluetooth
// const char *device_name = "FN-B05"; // Fone Karina
const char *device_name = "HZ-BT612"; // Fone Gatinho

// Controle de tempo para leituras
unsigned long ultimo_tempo_leitura = 0;
const int intervalo_leitura = 100; // 100ms entre leituras

void setup()
{
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = WATCHDOG_TIMEOUT_SECONDS * 1000,
      .idle_core_mask = (1 << 0), // Monitorar CPU0 (IDLE task)
      .trigger_panic = true};
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);
  // Configura o watchdog para 10 segundos
  // Adiciona a tarefa atual ao watchdog

  Serial.begin(115200);
  Serial.println("Inicializando sistema de detecção de obstáculos");

  // Inicializa os sensores
  Serial.println("Inicializando sensores...");
  setupSensors();
  Serial.println("Sensores ok.");

  // Inicializa o player de áudio
  Serial.println("Inicializando player de áudio...");
  setupAudioPlayer();
  Serial.println("Player de áudio ok.");

  // Inicializa o motor de áudio
  Serial.println("Inicializando audio...");
  setupAudio();
  Serial.println("Audio ok.");

  // Inicializa o Bluetooth A2DP
  Serial.println("Inicializando bluetooth...");
  setupBluetooth();
  Serial.println("Bluetooth ok.");

  delay(1000);
}

void loop()
{
  esp_task_wdt_reset();
  unsigned long tempo_atual = millis();

  // Realiza leituras em intervalos definidos - evita leituras excessivas
  if ((tempo_atual - ultimo_tempo_leitura) >= intervalo_leitura)
  {
    ultimo_tempo_leitura = tempo_atual;

    // Lê os sensores
    distancia_sensor1 = lerSensor1();
    distancia_sensor2 = lerSensor2();

    // Calcula a distância real do sensor 2 (considerando o ângulo de 45°)
    distancia_real_sensor2 = calcularDistanciaReal(distancia_sensor2);

    // Calcula os parâmetros de áudio baseados nas distâncias
    calcularParametrosAudio();

    if (distancia_sensor1 < 50)
    {
      // Obstáculo próximo à frente
      reproduzirAudio(AUDIO_OBSTACULO_FRENTE);
    }
    else if (distancia_real_sensor2 < 50)
    {
      // Obstáculo próximo abaixo
      reproduzirAudio(AUDIO_OBSTACULO_BAIXO);
    }
    else if (distancia_sensor1 > 200 && distancia_real_sensor2 > 200)
    {
      // Caminho livre
      reproduzirAudio(AUDIO_CAMINHO_LIVRE);
    }
    else if (distancia_sensor1 < 100 && distancia_sensor1 > 50)
    {
      // Obstáculo à frente, mas não muito próximo - sugerir virar
      // if (/* lógica para determinar direção */)
      // {
      //   reproduzirAudio(AUDIO_GIRE_DIREITA);
      // }
      // else
      // {
      //   reproduzirAudio(AUDIO_GIRE_ESQUERDA);
      // }
    }

    // Gera os earcons estéreo
    gerarEarcons(frequencia_esquerda, frequencia_direita, volume_esquerdo, volume_direito);

    // Exibe informações no monitor serial
    Serial.print("Sensor 1 (frente): ");
    Serial.print(distancia_sensor1);
    Serial.print(" cm | Sensor 2 (baixo): ");
    Serial.print(distancia_real_sensor2);
    Serial.println(" cm");
  }

  // Processa o player de áudio
  processarAudioPlayer();

  // Processa a conexão Bluetooth
  processarBluetooth();

  esp_task_wdt_reset(); // Reseta o watchdog
}