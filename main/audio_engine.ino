#include "BluetoothA2DPSource.h"
extern BluetoothA2DPSource a2dp_source;


// Configurações de áudio
#define SAMPLE_RATE 44100


// Buffer para geração de áudio
int16_t audio_buffer[512];
int buffer_position = 0;

// Variáveis externas para controle de áudio
extern int volume_esquerdo;
extern int volume_direito;
extern int frequencia_esquerda;
extern int frequencia_direita;
extern float distancia_sensor1;
extern float distancia_real_sensor2;

void setupAudio() {
  // Configura o callback para geração de áudio
  a2dp_source.set_data_callback(audioDataCallback);
}

// Callback para gerar dados de áudio
int32_t audioDataCallback(uint8_t *data, int32_t len) {
  // Reseta o watchdog no início do callback para evitar timeout
  esp_task_wdt_reset();

  // Converte o ponteiro para int16_t (formato PCM estéreo)
  int16_t *pcm_data = (int16_t*)data;
  int samples = len / 4; // 2 bytes por amostra, 2 canais
  
  // Limita o número de amostras processadas por vez se for muito grande
  const int MAX_SAMPLES_PER_BATCH = 256;
  if (samples > MAX_SAMPLES_PER_BATCH) samples = MAX_SAMPLES_PER_BATCH;

  // Gera as amostras de áudio
  for (int i = 0; i < samples; i++) {
    // Canal esquerdo (sensor 1 - frente)
    if (distancia_sensor1 < 200) { // Só gera som se estiver a menos de 2 metros
      float t = (float)buffer_position / SAMPLE_RATE;
      pcm_data[i*2] = (sin(2 * PI * frequencia_esquerda * t) * volume_esquerdo * 327); // 32767 / 100
    } else {
      pcm_data[i*2] = 0; // Sem som
    }
    
    // Canal direito (sensor 2 - baixo)
    if (distancia_real_sensor2 < 200) { // Só gera som se estiver a menos de 2 metros
      float t = (float)buffer_position / SAMPLE_RATE;
      pcm_data[i*2+1] = (sin(2 * PI * frequencia_direita * t) * volume_direito * 327); // 32767 / 100
    } else {
      pcm_data[i*2+1] = 0; // Sem som
    }
    
    buffer_position++;
    if (buffer_position >= SAMPLE_RATE) {
      buffer_position = 0; // Reinicia para evitar overflow
    }

        // Reset do watchdog a cada 64 amostras para garantir
    if (i % 64 == 0) {
      esp_task_wdt_reset();
    }
  }
  
  return len;
}

void gerarEarcons(int freq_esquerda, int freq_direita, int vol_esquerdo, int vol_direito) {
  // Atualiza os parâmetros globais para o callback de áudio
  frequencia_esquerda = freq_esquerda;
  frequencia_direita = freq_direita;
  volume_esquerdo = vol_esquerdo;
  volume_direito = vol_direito;
}