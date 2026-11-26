# Módulo 1a – Merging Unit (MU) com ESP32

Firmware de uma *Merging Unit* didática para subestações de energia, desenvolvida na disciplina de Sistemas em Tempo Real (STR) da Faculdade de Engenharia Elétrica da UFU.

A MU mede a corrente de uma fase com um transformador de corrente (TC), calcula o valor eficaz (RMS) e as harmônicas, detecta variações bruscas (como um curto-circuito) e envia as medidas pela rede Wi-Fi aos outros módulos do sistema, seguindo o **Contrato de Pacotes v1.1**.

## O que a MU faz

- Amostra o sinal do TC a 19.200 Hz e decima para 4.800 Hz (80 pontos por ciclo de 60 Hz).
- Calcula o RMS de um ciclo, atualizado a cada 4,17 ms, e as harmônicas de 1ª a 4ª ordem.
- Em regime normal, envia o pacote `99/1` a cada `intervaloEnvioMs` (no máximo 100 ms).
- Ao detectar um degrau de corrente, envia o primeiro `99/2` em até 17 ms e repete a cada 1 ms até o evento terminar ou o Módulo 2 reconhecê-lo.
- Aceita parametrização remota (`100/11`) e reconhecimento de evento (`100/10`) na porta TCP 5100.

## Hardware

| Item | Detalhe |
|---|---|
| Placa | ESP32 DevKit V1 (DOIT), 30 pinos, ESP32-WROOM-32 |
| Sensor | 1 TC SCT-013-000 (100 A : 50 mA), fase A |
| Condicionamento | Burden de 33 Ω sobre referência de 1,65 V; filtro RC de 3,3 kΩ e 100 nF; diodos BAT85 de proteção |
| Rede | Wi-Fi do próprio ESP32 (2,4 GHz), roteador dedicado |
| Alimentação | USB (power bank ou carregador de 5 V) |

| Sinal | GPIO | Uso |
|---|---|---|
| `IA_ADC` | 36 | Corrente da fase A (ADC1 canal 0) |
| `VREF_ADC` | 35 | Leitura da referência de 1,65 V (ADC1 canal 7) |
| `DBG_MEAS` | 26 | Sobe na detecção e desce antes do primeiro `99/2` (medição de latência) |
| `DBG_FRAME` | 27 | Alterna a cada quadro do DMA (medição do período) |

## Comunicação

Quem recebe é servidor; quem envia conecta. A MU é **cliente** dos módulos 2, 3 e 4 e **servidora** na porta 5100. Os pacotes são JSON, um por linha (NDJSON), sobre TCP com `TCP_NODELAY`.

| Conexão | Porta | Pacotes |
|---|---|---|
| MU → Módulo 2 (Proteção) | 5200 | `99/1`, `99/2`, `1/1` |
| MU → Módulo 3 (Supervisório) | 5300 | `99/1`, `99/2`, `1/1`, `100/12` |
| MU → Módulo 4 (Filtros) | 5400 | `99/1`, `99/2`, `1/1` |
| Módulos 2 e 3 → MU | 5100 | `100/10`, `100/11`, `1/1` |

## Estrutura do repositório

```text
mu-firmware/
├── main/               # app_main.c: inicialização e criação das tarefas
├── components/
│   ├── acq/            # ADC contínuo (DMA) e decimação
│   ├── dsp/            # RMS e Goertzel (C puro)
│   ├── detector/       # detector de eventos e máquina de estados (C puro)
│   ├── proto/          # montagem e leitura dos pacotes (C puro)
│   ├── net/            # Wi-Fi, SNTP, clientes TCP, servidor 5100
│   ├── config/         # parâmetros na NVS
│   └── diag/           # GPIOs de medição, watchdog, estatísticas
├── test_host/          # testes dos componentes C puro no computador
├── tools/              # receptores simulados e análise, em Python
└── sdkconfig.defaults
```

## Como compilar e gravar

Requisitos: VS Code com a extensão **ESP-IDF** da Espressif (instala o ESP-IDF e o compilador) e o driver USB da placa (CP2102 ou CH340).

```bash
idf.py set-target esp32
idf.py menuconfig      # "MU Configuration": Wi-Fi e IPs dos receptores
idf.py build
idf.py -p COM3 flash monitor
```

Se a gravação não conectar, segure o botão **BOOT** da placa durante o "Connecting...".

As credenciais do Wi-Fi e os IPs ficam no `sdkconfig` local, que **não vai para o Git**.

## Testes no computador

Os componentes `dsp`, `detector` e `proto` não dependem do ESP-IDF e têm testes próprios:

```bash
cmake -S test_host -B test_host/build
cmake --build test_host/build
ctest --test-dir test_host/build --output-on-failure
```

## Ferramentas de bancada

Em `tools/`, executadas no computador conectado ao mesmo roteador:

| Script | Função |
|---|---|
| `receptores_falsos.py` | Simula os módulos 2, 3 e 4 (portas 5200, 5300, 5400) e registra os pacotes |
| `comandos.py` | Envia `100/10` e `100/11` para a porta 5100 da MU |
| `analise.py` | Gera histogramas de intervalo entre pacotes e de latência |
| `sinais.py` | Gera sinais sintéticos para os testes do processamento |

## Andamento

| Fase | Entrega | Situação |
|---|---|---|
| F0 | Ambiente e esqueleto | Em andamento |
| F1 | Wi-Fi, SNTP e primeiro cliente TCP | — |
| F2 | Protocolo completo e máquina de estados (parte de rede) | — |
| F3 | Medição da latência do Wi-Fi | — |
| F4 | Processamento, detector e máquina de estados no PC | — |
| F5 | Aquisição real | — |
| F6 | Integração com a maleta de relé | — |
| F7 | Robustez, threads e análise de escalonabilidade | — |
| F8 | Integração com os outros módulos | — |

## Autor

Victor Jose Teixeira da Silva – Engenharia de Computação, UFU. Orientação: Prof. Alan Petrônio Pinheiro.