/*
 * acq.h - aquisicao (adc_continuous + DMA) e tarefa acq_task (Core 1).
 * acq_task: decimacao, RMS, Goertzel, detector e maquina de estados.
 */
#ifndef ACQ_H
#define ACQ_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "dsp.h"
#include "mu_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACQ_GPIO_IA              36   /* ADC1 canal 0 */
#define ACQ_GPIO_VREF            35   /* ADC1 canal 7 */
#define ACQ_N_CANAIS             2
#define ACQ_FS_CANAL_HZ          19200
#define ACQ_FS_TOTAL_HZ          (ACQ_FS_CANAL_HZ * ACQ_N_CANAIS)   /* 38.400 Hz */
#define ACQ_PONTOS_QUADRO        20                                 /* decimados por canal */
#define ACQ_AMOSTRAS_QUADRO      (ACQ_PONTOS_QUADRO * DSP_FATOR_DECIMACAO)  /* 80 por canal */
#define ACQ_CONV_QUADRO          (ACQ_AMOSTRAS_QUADRO * ACQ_N_CANAIS)       /* 160 */
#define ACQ_BYTES_QUADRO         (ACQ_CONV_QUADRO * SOC_ADC_DIGI_RESULT_BYTES)

_Static_assert(ACQ_FS_TOTAL_HZ >= SOC_ADC_SAMPLE_FREQ_THRES_LOW,
               "frequencia do adc_continuous abaixo do minimo do driver");

/* Ultima medida publicada pela acq_task (buffer duplo) */
typedef struct {
    uint32_t    n_quadro;
    int64_t     t_ms;
    float       ia;
    float       freq;
    float       harm_a[DSP_N_HARMONICAS];
    mu_estado_t estado;          /* E6, E7, E8 ou E9 */
    int64_t     ts_deteccao_ms;  /* valido em E7/E8 */
} acq_medida_t;

/* Configura o adc_continuous (F5). Chamar antes de criar a acq_task. */
esp_err_t acq_init(void);

/* Inicia o DMA; o callback notifica a acq_task a cada quadro */
esp_err_t acq_start(void);

/* Copia a ultima medida publicada. Retorna false se ainda nao ha medida. */
bool acq_le_ultima(acq_medida_t *out);

/* Entrega um evento externo (rede, comandos) a maquina de estados. Nao bloqueia. */
bool acq_envia_evento(mu_evento_t ev);

/* Tarefa acq_task (Core 1, prioridade mais alta da aplicacao) */
void acq_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* ACQ_H */
