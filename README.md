# Boné com Sensores Ultrassônicos

![Versão](https://img.shields.io/badge/versão-1.0-blue)
![Plataforma](https://img.shields.io/badge/plataforma-ESP32-green)
![Licença](https://img.shields.io/badge/licença-MIT-orange)

## 📋 Descrição

O **Boné com Sensores** é um dispositivo assistivo que utiliza sensores ultrassônicos para detectar obstáculos no ambiente e alertar o usuário através de sinais sonoros via Bluetooth. Projetado para auxiliar pessoas com deficiência visual, o sistema fornece feedback auditivo sobre obstáculos à frente e abaixo do usuário, permitindo uma navegação mais segura e independente.

## 🔍 Características

- **Detecção de Obstáculos**: Utiliza dois sensores ultrassônicos estrategicamente posicionados:
  - Sensor superior: detecta obstáculos à frente do usuário (altura da cabeça/tronco)
  - Sensor inferior: detecta obstáculos abaixo (altura das pernas/pés)

- **Feedback Sonoro**:
  - Alertas por voz para diferentes situações ("Obstáculo à frente", "Obstáculo abaixo", etc.)
  - Earcons estéreo com frequência e volume variáveis de acordo com a proximidade dos obstáculos
  - Transmissão via Bluetooth para fones de ouvido

- **Processamento Inteligente**:
  - Cálculo de distância real considerando o ângulo dos sensores
  - Ajuste baseado na altura do usuário
  - Filtragem para evitar falsos positivos

## 🛠️ Componentes

- ESP32 (microcontrolador principal)
- 2 sensores ultrassônicos HC-SR04
- Módulo Bluetooth (integrado ao ESP32)
- Bateria portátil
- Boné comum (para montagem)
- Fones de ouvido Bluetooth compatíveis

## 📊 Funcionamento

O sistema opera em um ciclo contínuo:

1. Os sensores ultrassônicos medem a distância até os obstáculos
2. O ESP32 processa essas informações, calculando a distância real
3. Com base nas distâncias, o sistema decide qual alerta sonoro reproduzir
4. Simultaneamente, gera earcons estéreo com frequência e volume proporcionais à proximidade dos obstáculos
5. Os sinais de áudio são transmitidos via Bluetooth para os fones de ouvido

## 🔧 Configuração

### Pré-requisitos

- Arduino IDE
- Bibliotecas:
  - ESP32 Arduino Core
  - BluetoothA2DPSource
  - Audio.h (para reprodução de arquivos WAV)
  - SPIFFS (para armazenamento de arquivos)

### Montagem

1. Posicione o sensor superior na frente do boné, apontando para frente
2. Posicione o sensor inferior na aba do boné, apontando para baixo em um ângulo de 45°
3. Conecte os sensores ao ESP32 conforme o esquema de pinos no código
4. Instale o ESP32 e a bateria em um compartimento no boné

### Configuração do Software

1. Ajuste a variável `altura_pessoa` no código para corresponder à altura do usuário
2. Configure o nome do dispositivo Bluetooth (`device_name`) para corresponder aos seus fones de ouvido
3. Carregue os arquivos de áudio para a memória SPIFFS
4. Compile e carregue o código para o ESP32

## 📱 Uso

1. Ligue o dispositivo
2. Conecte seus fones de ouvido Bluetooth (você ouvirá "Bem-vindo" quando conectado)
3. Coloque o boné e ajuste conforme necessário
4. O sistema começará a detectar obstáculos e fornecer feedback sonoro automaticamente

## 🔄 Manutenção

- Verifique regularmente o nível da bateria
- Mantenha os sensores limpos para leituras precisas
- Atualize o firmware conforme necessário para melhorias e correções

## 🧩 Estrutura do Projeto

- `main.ino`: Arquivo principal com setup e loop
- `sensors.ino`: Funções para leitura e processamento dos sensores
- `audio_engine.ino`: Geração de earcons e processamento de áudio
- `audio_player.ino`: Reprodução de arquivos de áudio pré-gravados
- `bluetooth_a2dp.ino`: Configuração e gerenciamento da conexão Bluetooth
- `audio_player.h`: Definições para o player de áudio
- `audios/`: Pasta com arquivos WAV para alertas por voz

## 🤝 Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou enviar pull requests com melhorias.

## 📜 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo LICENSE para detalhes.

## 📞 Contato

Para mais informações, entre em contato com o desenvolvedor do projeto.

---

⭐ Desenvolvido com ❤️ para tornar a navegação mais acessível para pessoas com deficiência visual.
