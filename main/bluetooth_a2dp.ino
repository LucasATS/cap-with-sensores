#include "BluetoothA2DPSource.h"
BluetoothA2DPSource a2dp_source;

#include "audio_player.h"

bool msg_conectado = true;

// Flags para gerenciar o reinício e desconexão
bool bluetooth_restarting = false;
bool bluetooth_disconnecting = false;
unsigned long bluetooth_restart_time = 0;
unsigned long bluetooth_disconnect_time = 0;

// Parâmetros de conexão ajustáveis
const int MAX_RECONNECT_ATTEMPTS = 8;
const unsigned long RECONNECT_INTERVAL = 3000;   // 3 segundos
const unsigned long CONNECTION_WATCHDOG = 30000; // 30 segundos

// Variáveis para controle de reconexão
int reconnect_attempts = 0;
unsigned long ultimo_tempo_reconexao = 0;
unsigned long ultimo_tempo_conexao_estavel = 0;

// Callback para dados de áudio - corrigido para corresponder à assinatura esperada
int32_t audio_data_callback(uint8_t *data, int32_t len)
{
  // Este callback é chamado quando dados de áudio são processados
  // Pode ser usado para monitorar a atividade de áudio
  ultimo_tempo_conexao_estavel = millis();

  // Retorna o número de bytes processados
  return len;
}

// Callback para mudanças de estado da conexão
void connection_state_changed(esp_a2d_connection_state_t state, void *)
{
  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED)
  {
    Serial.println("Bluetooth conectado com sucesso!");
    reconnect_attempts = 0; // Reseta as tentativas quando conecta
    ultimo_tempo_conexao_estavel = millis();
  }
  else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED)
  {
    Serial.println("Bluetooth desconectado.");
  }
}

void setupBluetooth()
{
  // Configurações avançadas para melhorar a estabilidade
  a2dp_source.set_auto_reconnect(true);

  // Configura o callback com a assinatura correta
  a2dp_source.set_data_callback(audio_data_callback);

  a2dp_source.set_on_connection_state_changed(connection_state_changed);

  // Inicializa o Bluetooth A2DP como fonte de áudio
  a2dp_source.start(device_name);
  Serial.println("Bluetooth A2DP iniciado. Aguardando conexão...");

  // Configura o callback para geração de áudio
  ultimo_tempo_conexao_estavel = millis();
}

void processarBluetooth()
{

    // Reset do watchdog no início da função
  esp_task_wdt_reset();

  unsigned long tempo_atual = millis();

  // Verifica e mantém a conexão Bluetooth
  if (!a2dp_source.is_connected())
  {
    // Tenta reconectar se necessário, com limite de tentativas
    if (tempo_atual - ultimo_tempo_reconexao >= RECONNECT_INTERVAL)
    {
      ultimo_tempo_reconexao = tempo_atual;

      if (reconnect_attempts < MAX_RECONNECT_ATTEMPTS)
      {
        Serial.printf("Tentando reconectar Bluetooth... Tentativa %d/%d\n",
                      reconnect_attempts + 1, MAX_RECONNECT_ATTEMPTS);
        a2dp_source.reconnect();
        reconnect_attempts++;
      }
      else
      {
        // Se excedeu o número de tentativas, reinicia o Bluetooth
        Serial.println("Reiniciando o módulo Bluetooth após várias tentativas...");
        a2dp_source.end();
        // Substitua o delay por uma flag e um timestamp
        bluetooth_restart_time = tempo_atual;
        bluetooth_restarting = true;
      }
    }

    msg_conectado = true;
  }
  else
  {
    // Quando conectado
    if (msg_conectado)
    {
      Serial.print("Bluetooth conectado: ");
      Serial.println(device_name);

      msg_conectado = false;  // Reseta a flag
      reconnect_attempts = 0; // Reseta as tentativas quando conecta
      ultimo_tempo_conexao_estavel = tempo_atual;

      reproduzirAudio(AUDIO_BEM_VINDO);
    }

    // Watchdog para verificar se a conexão está realmente estável
    if (tempo_atual - ultimo_tempo_conexao_estavel >= CONNECTION_WATCHDOG)
    {
      // Envia um ping ou verifica o estado para confirmar que a conexão está ativa
      if (a2dp_source.get_connection_state() == ESP_A2D_CONNECTION_STATE_CONNECTED)
      {
        ultimo_tempo_conexao_estavel = tempo_atual;
      }
      else
      {
        // Se a conexão parece estar em um estado inconsistente, força reconexão
        Serial.println("Conexão em estado inconsistente. Forçando reconexão...");
        a2dp_source.disconnect();
        // Substitua o delay por uma flag e um timestamp
        bluetooth_disconnect_time = tempo_atual;
        bluetooth_disconnecting = true;
      }
    }

    esp_task_wdt_reset();
  }

  // Gerencia os estados de reinício/desconexão sem usar delay
  if (bluetooth_restarting && tempo_atual - bluetooth_restart_time >= 1000)
  {
    bluetooth_restarting = false;
    a2dp_source.start(device_name);
    reconnect_attempts = 0;
  }

  if (bluetooth_disconnecting && tempo_atual - bluetooth_disconnect_time >= 500)
  {
    bluetooth_disconnecting = false;
    ultimo_tempo_reconexao = tempo_atual - RECONNECT_INTERVAL; // Força reconexão imediata
  }
}