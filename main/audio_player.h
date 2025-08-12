#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>

enum TipoAudio {
  AUDIO_CAMINHO_LIVRE,
  AUDIO_OBSTACULO_BAIXO,
  AUDIO_OBSTACULO_FRENTE,
  AUDIO_BEM_VINDO
};

void setupAudioPlayer();
void reproduzirAudio(TipoAudio tipo);
void processarAudioPlayer();

#endif
