# CLAUDE.md – Firmware da Merging Unit (Módulo 1a)

Este arquivo dá o contexto do projeto ao Claude Code. Leia-o inteiro antes de escrever qualquer código. As decisões abaixo já foram tomadas e estão no relatório técnico; **não as altere sem perguntar**.

## 1. O projeto em uma frase

Firmware ESP-IDF para um ESP32 que mede a corrente de uma fase com um TC, calcula RMS e harmônicas, detecta degraus de corrente e envia pacotes JSON por TCP sobre Wi-Fi, com requisitos de tempo real (primeiro `99/2` em até 17 ms após o degrau, envio a cada 1 ms no modo evento).

Trabalho da disciplina de Sistemas em Tempo Real (UFU). O código é avaliado também pela **arquitetura**: máquina de estados explícita, tarefas do FreeRTOS bem definidas e tempos medidos.

## 2. Plataforma

- **Placa:** ESP32 DevKit V1 (DOIT), 30 pinos, ESP32-WROOM-32. Dois núcleos.
- **Framework:** ESP-IDF (versão estável mais recente), em C. Não use Arduino.
- **Rede:** Wi-Fi do ESP32, modo estação, 2,4 GHz. Não há módulo Ethernet.
- **Um único sensor**, na fase A. Os campos `ib` e `ic` dos pacotes são enviados como `0.0`.

| Sinal | GPIO | Uso |
|---|---|---|
| `IA_ADC` | 36 | ADC1 canal 0, corrente da fase A |
| `VREF_ADC` | 35 | ADC1 canal 7, referência de 1,65 V |
| `DBG_MEAS` | 26 | Sobe quando o detector registra `tsDeteccao`; desce imediatamente antes do `send()` do primeiro `99/2` |
| `DBG_FRAME` | 27 | Alterna a cada quadro do DMA |

Não use os GPIOs 6 a 11 (memória flash) nem os pinos de *strapping* 0, 2, 12 e 15.

## 3. Aquisição e processamento

- `adc_continuous` com **2 canais** (GPIO 36 e 35), **19.200 Hz por canal, 38.400 Hz no total**, atenuação `ADC_ATTEN_DB_12`, calibração do ADC para mV.
  - Motivo: 2 × 9.600 Hz = 19.200 Hz ficaria abaixo do mínimo do driver (~20 kHz). Confira `SOC_ADC_SAMPLE_FREQ_THRES_LOW` em `soc_caps.h` antes de fixar.
- **Decimação por 4** (média de 4 amostras) → 4.800 Hz por canal.
- **Quadro:** 20 pontos decimados por canal = 80 amostras brutas por canal = **4,17 ms**. Tamanho em bytes = 160 conversões × `SOC_ADC_DIGI_RESULT_BYTES`.
- **RMS:** buffer circular de **80 pontos** (1 ciclo de 60 Hz), recalculado a cada quadro, com remoção do offset (subtrai a referência lida no GPIO 35, filtrada da mesma forma) e calibração: `I = ganho × RMS_em_A + offset`.
- Conversão: corrente no primário = (tensão no burden / 33 Ω) × 2000.
- **Harmônicas 1 a 4** (60 a 240 Hz) por Goertzel nos bins k = 1..4 da janela de 80 pontos.

## 4. Detector de eventos

Compara o RMS com uma **referência lenta**, nunca quadro com quadro.

- `ΔI = |I_RMS − I_ref|`; `I_ref += α (I_RMS − I_ref)` **somente em modo normal**, com α = 1/64.
- `L = limiarDeltaI` (padrão 6 A, alterável por `100/11`).
- Normal → Transitório: `ΔI > L` em 1 quadro (registra `tsDeteccao`).
- Transitório → Evento: `ΔI > L` por mais **N = 2** quadros. Transitório → Normal: `ΔI < 0,9 L`.
- Evento → Normal: `ΔI < 0,9 L` por **M = 4** quadros, ou `100/10` com `comando = 0`, ou **2 s** em evento. Ao voltar, `I_ref = I_RMS`.
- O algoritmo de referência está no relatório (Código 2, função `avalia_evento`).

## 5. Máquina de estados (obrigatória)

Explícita no código: `enum` de estados, `enum` de eventos e **uma única função** de transição. As tarefas geram eventos; só a máquina muda o estado. Toda transição vai para o log (tempo, estado anterior, evento, novo estado).

| Estado | Ação | Saídas |
|---|---|---|
| E1 Inicialização | Carrega parâmetros da NVS, verifica ADC e Wi-Fi, espera 3 s pela referência | Link ok → E3; sem link → E2 |
| E2 Erro de rede | Medição continua; envio suspenso; nova tentativa a cada 500 ms | Link ok → E1 |
| E3 Conexões TCP | Abre servidor 5100 e clientes 5200/5300/5400 | → E4 |
| E4 Medidas | Consome quadro, decima, RMS, harmônicas | Link caiu → E2; `habilitado = 0` → E9; senão → E5 |
| E5 Avalia evento | Calcula ΔI e aplica o detector | → E6, E7 ou E8 |
| E6 Normal | Prepara `99/1` | → E10 |
| E7 Transitório | Prepara `99/2`, registra `tsDeteccao` | Confirmado → E8; estabilizou → E6 |
| E8 Evento | Prepara `99/2` com `fasesDiscrepantes` | `100/10`, M quadros estáveis ou 2 s → E6 |
| E9 Desabilitado | Suspende `99/x`; mantém medição, `1/1` e porta 5100 | `habilitado = 1` → E4 |
| E10 Envio | Se o prazo do modo venceu: incrementa `seq`, preenche `ts`, enfileira em cada conexão | → E4 |

## 6. Tarefas (threads do FreeRTOS)

Todas criadas com `xTaskCreatePinnedToCore`. **Regra central: o Core 1 mede e decide; o Core 0 cuida da rede. Nada bloqueante no Core 1.**

| Tarefa | Núcleo | Prioridade | Gatilho | Função |
|---|---|---|---|---|
| `acq_task` | 1 | Mais alta da aplicação | Notificação do DMA | Decimação, RMS, Goertzel, detector, máquina de estados |
| `tx_scheduler` | 1 | Alta | Período de 1 ms | Monta pacotes quando o prazo do modo vence e enfileira |
| `net_tx_task` | 0 | Média | Fila de pacotes | `send()` não bloqueante; fila cheia descarta o `99/1` mais antigo, **nunca** um `99/2` |
| `conn_manager` | 0 | Média | 100 ms | Reconexão a cada 500 ms, `1/1` a cada 1 s, receptor perdido após 3 s |
| `cmd_server` | 0 | Média | Dados na 5100 | Separa linhas, valida, aplica `100/10` e `100/11`, responde `100/12` |
| `supervisor` | 0 | Baixa | 1 s | Verifica se todas as tarefas estão vivas, alimenta o *watchdog*, estatísticas |

Comunicação entre tarefas: filas, `xTaskNotify` e atômicos. Última medida em **buffer duplo**; parâmetros como **cópia completa com troca atômica de ponteiro**, aplicada entre quadros. Sem `malloc` no caminho de 1 ms.

Configuração (`sdkconfig.defaults`): `CONFIG_FREERTOS_HZ=1000`, CPU 240 MHz, Task Watchdog com pânico, tarefas do Wi-Fi e da lwIP no Core 0. No código: `esp_wifi_set_ps(WIFI_PS_NONE)`.

## 7. Contrato de Pacotes v1.1 (não alterar)

TCP, um JSON por linha terminado em `\n`, conexão persistente, `TCP_NODELAY`. O receptor separa pacotes pelo `\n` (TCP é fluxo de bytes).

**Cabeçalho comum em todo pacote:** `tipo` (string), `versao` (1), `modulo` (`"1"`), `idOrigem` (= `idMU`), `seq` (long, crescente por emissor), `ts` (epoch ms UTC, via SNTP).

| Tipo | Direção | Campos além do cabeçalho |
|---|---|---|
| `1/1` | todas as conexões, a cada 1 s | `intervaloMs: 1000` |
| `99/1` | MU → 5200, 5300, 5400 | `idMU`, `idAtivo`, `modo:"NORMAL"`, `janelaMs:17`, `nPontos:80`, `ia`, `ib`, `ic`; opcionais `freq`, `harmA` |
| `99/2` | MU → 5200, 5300, 5400 | Os mesmos de `99/1` com `modo:"EVENTO"`, mais `fasesDiscrepantes` (ex.: `["A"]`) e `tsDeteccao` |
| `100/10` | M2 → MU:5100 | `idMU`, `comando` (0 reconhecido, 1 aumenta taxa, 2 reduz), `parametro1`, `idEvento` |
| `100/11` | M3 → MU:5100 | `idRequisicao`, `idMU`, `parametro`, `valor` |
| `100/12` | MU → 5300 | `idRequisicao`, `idMU`, `aceito`, `motivo` |

Parâmetros válidos no `100/11`: `intervaloEnvioMs` (≤ 100), `habilitado` (0/1), `canalA.ganho`, `canalA.offset`, `limiarDeltaI`. Valor inválido → `aceito:false` com `motivo`, sem alterar nada.

Exemplo de `99/1`:
```json
{"tipo":"99/1","versao":1,"modulo":"1","idOrigem":3,"seq":4529,"ts":1792790360300,"idMU":3,"idAtivo":"ALIM-03","modo":"NORMAL","janelaMs":17,"nPontos":80,"ia":12.4,"ib":0.0,"ic":0.0,"freq":60.0,"harmA":[12.4,0.1,0.4,0.2]}
```

## 8. Regras de código

- **C (C11), ESP-IDF.** Nomes de tarefas e módulos como neste arquivo. Comentários em português, sem acentos (evita problemas de codificação em ferramentas).
- `dsp`, `detector` e `proto` são **C puro**: sem includes do ESP-IDF ou do FreeRTOS. Recebem tempo como parâmetro (`int64_t agora_ms`), não leem relógio.
- Todo código de `dsp`, `detector` e `proto` tem **teste em `test_host/`** antes de ir para a placa.
- Serialização de pacotes com `snprintf` em buffer fixo; leitura com **cJSON** (já vem no ESP-IDF). Nunca `malloc` no caminho de 1 ms.
- Sockets sempre não bloqueantes nas tarefas de envio. Um receptor ausente ou lento nunca bloqueia os outros nem a aquisição.
- Credenciais do Wi-Fi e IPs dos receptores via `Kconfig.projbuild` (menu "MU Configuration") e NVS. **Nunca escreva senhas no código.**
- Use `ESP_LOGx` com uma tag por componente. Nada de `printf` no Core 1 dentro do laço de aquisição.
- Erros de API do ESP-IDF: `ESP_ERROR_CHECK` só na inicialização; em tempo de execução, trate o erro e registre.

## 9. Comandos

```bash
idf.py build                       # compilar
idf.py -p COM3 flash monitor       # gravar e abrir o monitor (sair: Ctrl+])
idf.py menuconfig                  # configurar
cmake -S test_host -B test_host/build && cmake --build test_host/build && ctest --test-dir test_host/build --output-on-failure
python tools/receptores_falsos.py  # simula os módulos 2, 3 e 4
```

Antes de dizer que uma tarefa está pronta: compilar sem avisos novos e rodar os testes de `test_host` que existirem.

## 10. Pendências conhecidas (não decida sozinho)

- Orçamento fim a fim: 42 ms (contrato) ou 32 ms; com 32 ms, o quadro do DMA cai para 10 pontos.
- Valor final de `limiarDeltaI`: definido nos testes com a maleta de relé.
- Limite mínimo de frequência do `adc_continuous`: confirmar na versão instalada do ESP-IDF.

Se uma tarefa exigir mudar o contrato, a máquina de estados, os tempos ou a divisão de núcleos, **pare e pergunte**.