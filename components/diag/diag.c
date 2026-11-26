/*
 * diag.c - GPIOs de depuracao, heartbeat das tarefas e supervisor.
 * TODO(F7): estatisticas (pilha minima, uso de CPU, pacotes descartados).
 */
#include "diag.h"

#include <stdatomic.h>
#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const char *TAG = "diag";

#define DIAG_FILA_LOG        32
#define DIAG_PERIODO_MS      1000

static atomic_uint s_batidas[DIAG_N_TAREFAS];
static unsigned    s_ultima[DIAG_N_TAREFAS];
static atomic_bool s_frame_nivel;
static QueueHandle_t s_fila_log;

static const char *const NOMES_TAREFA[DIAG_N_TAREFAS] = {
    [DIAG_T_ACQ]          = "acq_task",
    [DIAG_T_TX_SCHEDULER] = "tx_scheduler",
    [DIAG_T_NET_TX]       = "net_tx_task",
    [DIAG_T_CONN_MANAGER] = "conn_manager",
    [DIAG_T_CMD_SERVER]   = "cmd_server",
};

esp_err_t diag_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << DIAG_GPIO_MEAS) | (1ULL << DIAG_GPIO_FRAME),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io));
    gpio_set_level(DIAG_GPIO_MEAS, 0);
    gpio_set_level(DIAG_GPIO_FRAME, 0);

    s_fila_log = xQueueCreate(DIAG_FILA_LOG, sizeof(mu_transicao_t));
    return (s_fila_log != NULL) ? ESP_OK : ESP_ERR_NO_MEM;
}

void diag_meas_sobe(void)
{
    gpio_set_level(DIAG_GPIO_MEAS, 1);
}

void diag_meas_desce(void)
{
    gpio_set_level(DIAG_GPIO_MEAS, 0);
}

void diag_frame_alterna(void)
{
    bool nivel = !atomic_load(&s_frame_nivel);
    atomic_store(&s_frame_nivel, nivel);
    gpio_set_level(DIAG_GPIO_FRAME, nivel ? 1 : 0);
}

void diag_heartbeat(diag_tarefa_t t)
{
    if ((unsigned)t < DIAG_N_TAREFAS) {
        atomic_fetch_add_explicit(&s_batidas[t], 1u, memory_order_relaxed);
    }
}

void diag_log_transicao(const mu_transicao_t *t, void *ctx)
{
    (void)ctx;
    if (s_fila_log != NULL) {
        /* Fila cheia: perde a entrada de log, nunca bloqueia o Core 1 */
        (void)xQueueSend(s_fila_log, t, 0);
    }
}

static void esvazia_log(void)
{
    mu_transicao_t t;
    while (xQueueReceive(s_fila_log, &t, 0) == pdTRUE) {
        ESP_LOGI(TAG, "FSM t=%lld %s --%s--> %s", (long long)t.t_ms,
                 mu_estado_nome(t.anterior), mu_evento_nome(t.evento), mu_estado_nome(t.novo));
    }
}

void supervisor(void *arg)
{
    (void)arg;
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    TickType_t ultimo = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(DIAG_PERIODO_MS));
        esvazia_log();

        bool todas_vivas = true;
        for (int i = 0; i < DIAG_N_TAREFAS; i++) {
            unsigned b = atomic_load_explicit(&s_batidas[i], memory_order_relaxed);
            if (b == s_ultima[i]) {
                ESP_LOGE(TAG, "tarefa %s sem heartbeat", NOMES_TAREFA[i]);
                todas_vivas = false;
            }
            s_ultima[i] = b;
        }

        /* So alimenta o watchdog se todas as tarefas estao vivas */
        if (todas_vivas) {
            esp_task_wdt_reset();
        }
    }
}
