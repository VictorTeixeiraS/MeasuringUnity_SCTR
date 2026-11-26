/*
 * acq.c - F0: esqueleto da acq_task, buffer duplo e fila de eventos.
 * TODO(F5): adc_continuous com 2 canais, calibracao em mV e callback do DMA.
 */
#include "acq.h"

#include <stdatomic.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "detector.h"
#include "diag.h"
#include "mu_config.h"

static const char *TAG = "acq";

#define ACQ_FILA_EVENTOS     16
#define ACQ_ESPERA_MAX_MS    100   /* sem DMA (F0), acorda assim mesmo para o heartbeat */

static TaskHandle_t  s_tarefa;
static QueueHandle_t s_fila_eventos;

/* Buffer duplo com contador de sequencia (escritor unico: acq_task) */
static acq_medida_t s_medida[2];
static atomic_uint  s_seq;           /* numero de publicacoes; slot ativo = s_seq % 2 */

static mu_fsm_t   s_fsm;
static detector_t s_det;

esp_err_t acq_init(void)
{
    s_fila_eventos = xQueueCreate(ACQ_FILA_EVENTOS, sizeof(mu_evento_t));
    if (s_fila_eventos == NULL) {
        return ESP_ERR_NO_MEM;
    }
    /* TODO(F5): adc_continuous_new_handle, config de 2 canais a ACQ_FS_TOTAL_HZ,
     * ADC_ATTEN_DB_12, frame de ACQ_BYTES_QUADRO, calibracao */
    ESP_LOGI(TAG, "quadro: %d conversoes, %d bytes", ACQ_CONV_QUADRO, ACQ_BYTES_QUADRO);
    return ESP_OK;
}

esp_err_t acq_start(void)
{
    /* TODO(F5): adc_continuous_register_event_callbacks + adc_continuous_start */
    return ESP_OK;
}

/*
 * Escritor: grava no slot inativo e so depois avanca s_seq (troca o slot ativo).
 * Leitor: a copia e valida se nenhuma publicacao terminou durante a leitura.
 * O slot lido so volta a ser escrito depois da proxima publicacao (s_seq == antes + 1),
 * entao s_seq inalterado garante copia integra. Quadro de 4,17 ms >> tempo da copia.
 */
static void publica_medida(const acq_medida_t *m)
{
    unsigned seq = atomic_load_explicit(&s_seq, memory_order_relaxed);
    unsigned inativo = (seq + 1u) % 2u;
    s_medida[inativo] = *m;
    atomic_store_explicit(&s_seq, seq + 1u, memory_order_release);
}

bool acq_le_ultima(acq_medida_t *out)
{
    for (int tentativa = 0; tentativa < 4; tentativa++) {
        unsigned antes = atomic_load_explicit(&s_seq, memory_order_acquire);
        if (antes == 0u) {
            return false;   /* nada publicado ainda */
        }
        *out = s_medida[antes % 2u];
        atomic_thread_fence(memory_order_acquire);
        unsigned depois = atomic_load_explicit(&s_seq, memory_order_relaxed);
        if (depois == antes) {
            return true;
        }
    }
    return false;
}

bool acq_envia_evento(mu_evento_t ev)
{
    if (s_fila_eventos == NULL) {
        return false;
    }
    return xQueueSend(s_fila_eventos, &ev, 0) == pdTRUE;
}

void acq_task(void *arg)
{
    (void)arg;
    s_tarefa = xTaskGetCurrentTaskHandle();
    mu_fsm_init(&s_fsm, diag_log_transicao, NULL);
    detector_init(&s_det, 0.0f);

    for (;;) {
        /* Gatilho: notificacao do callback do DMA */
        uint32_t quadros = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(ACQ_ESPERA_MAX_MS));
        diag_heartbeat(DIAG_T_ACQ);
        int64_t agora_ms = esp_timer_get_time() / 1000;

        /* Eventos externos (rede, comandos) */
        mu_evento_t ev;
        while (xQueueReceive(s_fila_eventos, &ev, 0) == pdTRUE) {
            (void)mu_fsm_processa(&s_fsm, ev, agora_ms);
        }

        if (quadros == 0u) {
            continue;
        }

        /* Parametros lidos uma vez por quadro (troca atomica aplicada entre quadros) */
        const mu_param_t *param = mu_config_param();
        (void)param;
        diag_frame_alterna();

        /* TODO(F4/F5): ler quadro, decimar, RMS, harmonicas, avalia_evento, FSM */
        acq_medida_t m = { 0 };
        m.t_ms = agora_ms;
        m.estado = s_fsm.estado;
        publica_medida(&m);
    }
}
