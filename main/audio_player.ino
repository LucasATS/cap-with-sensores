#include "audio_player.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Audio object
Audio audio;

// Enumeração para os diferentes tipos de áudio
enum TipoAudio {
  AUDIO_CAMINHO_LIVRE,
  // AUDIO_GIRE_DIREITA,
  // AUDIO_GIRE_ESQUERDA,
  AUDIO_OBSTACULO_BAIXO,
  AUDIO_OBSTACULO_FRENTE,
  AUDIO_BEM_VINDO
};

// Caminhos para os arquivos de áudio
const char* caminhos_audio[] = {
  "/audios/caminho_livre.wav",
  // "/audios/Gire a direita.wav",
  // "/audios/Gire a esquerda.wav",
  "/audios/obstaculo_baixo.wav",
  "/audios/obstaculo_frente.wav",
  "/audios/bem_vindo.wav"
};

// Variáveis para controle de reprodução
bool audio_playing = false;
unsigned long ultimo_audio = 0;
const int INTERVALO_ENTRE_AUDIOS = 3000; // 3 segundos entre áudios

void setupAudioPlayer() {
  // Inicializar sistema de arquivos
  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao montar SPIFFS");
    return;
  }
  
  // Configurar áudio
  audio.setVolume(30); // 0...21
  
  Serial.println("Sistema de áudio inicializado");
}

void reproduzirAudio(TipoAudio tipo) {
  unsigned long tempo_atual = millis();
  
  // Evita reproduzir áudios muito frequentemente
  if (tempo_atual - ultimo_audio < INTERVALO_ENTRE_AUDIOS) {
    return;
  }
  
  // Verifica se já está reproduzindo
  if (audio_playing) {
    audio.stopSong();
  }
  
  // Inicia a reprodução do áudio
  if (tipo >= 0 && tipo < 5) {
    audio.connecttoFS(SPIFFS, caminhos_audio[tipo]);
    audio_playing = true;
    ultimo_audio = tempo_atual;
    Serial.printf("Reproduzindo áudio: %s\n", caminhos_audio[tipo]);
  }
}

void processarAudioPlayer() {
  esp_task_wdt_reset();
  
  if (audio_playing) {
    if (audio.isRunning()) {
      audio.loop();
    } else {
      audio_playing = false;
    }
  }
  
  esp_task_wdt_reset();
}