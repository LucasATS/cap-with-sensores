#include <Arduino.h>
#include "BluetoothA2DPSource.h"

// Instância global usada pelo main/sensors
BluetoothA2DPSource a2dp_source;

// Estes vêm do main/sensors
extern const char *device_name;
extern int volume_esquerdo;     // 0..100
extern int volume_direito;      // 0..100
extern int frequencia_esquerda; // Hz (0 = mudo)
extern int frequencia_direita;  // Hz (0 = mudo)

// Configuração de áudio
static constexpr int SAMPLE_RATE = 44100; // A2DP usa PCM 44.1kHz 16-bit estéreo. 
                                          // O encoder SBC da lib parte desse PCM. :contentReference[oaicite:1]{index=1}

static float phaseL = 0.0f;
static float phaseR = 0.0f;
static float envL = 0.0f;
static float envR = 0.0f;

// Callback que a lib chama pedindo amostras PCM
static int32_t audioDataCallback(uint8_t *data, int32_t len) {
  const int vL = volume_esquerdo;
  const int vR = volume_direito;
  const int fL = frequencia_esquerda;
  const int fR = frequencia_direita;

  int16_t *pcm = reinterpret_cast<int16_t*>(data);
  const int frames = len / 4; // 2 bytes * 2 canais

  const float twoPi   = 6.28318530718f;
  const float attack  = 0.004f;  // ~4 ms
  const float release = 0.002f;  // ~2 ms

  const float targetAmpL = (vL > 0 ? (vL / 100.0f) : 0.0f);
  const float targetAmpR = (vR > 0 ? (vR / 100.0f) : 0.0f);
  const float incL = (fL > 0 ? twoPi * (float)fL / SAMPLE_RATE : 0.0f);
  const float incR = (fR > 0 ? twoPi * (float)fR / SAMPLE_RATE : 0.0f);

  for (int i = 0; i < frames; ++i) {
    envL += ( (targetAmpL - envL) * ((targetAmpL > envL) ? attack : release) );
    envR += ( (targetAmpR - envR) * ((targetAmpR > envR) ? attack : release) );

    float sL = (fL > 0) ? sinf(phaseL) * envL : 0.0f;
    float sR = (fR > 0) ? sinf(phaseR) * envR : 0.0f;

    phaseL += incL; if (phaseL >= twoPi) phaseL -= twoPi;
    phaseR += incR; if (phaseR >= twoPi) phaseR -= twoPi;

    // Converte para 16-bi
    pcm[2*i]     = (int16_t)(sL * 32767.0f);
    pcm[2*i + 1] = (int16_t)(sR * 32767.0f);
  }

  return len;
}

// Logs
static void connection_state_changed(esp_a2d_connection_state_t state, void*) {
  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
    Serial.println(F("[BT] Conectado"));
    digitalWrite(02, HIGH);
  } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
    Serial.println(F("[BT] Desconectado"));
    digitalWrite(02, LOW);
  }
}


void setupBluetooth() {
  pinMode(02, OUTPUT);
  digitalWrite(02, LOW);

  a2dp_source.set_auto_reconnect(true);
  a2dp_source.set_on_connection_state_changed(connection_state_changed);
  a2dp_source.set_data_callback(audioDataCallback);
  a2dp_source.start(device_name);
  Serial.println(F("[BT] A2DP Source iniciado"));
}


void processarBluetooth() {
  static unsigned long lastTry = 0;
  if (!a2dp_source.is_connected()) {
    const unsigned long now = millis();
    if (now - lastTry > 3000) {
      lastTry = now;
      a2dp_source.reconnect();
    }
  }
}
