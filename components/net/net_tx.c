/*
 * net_tx.c - fila de pacotes e net_tx_task (Core 0).
 * TODO(F2): send() nao bloqueante por conexao; descarte do 99/1 mais antigo com fila cheia.
 */
#include "net.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "diag.h"

static const char *TAG = "net_tx";

#define NET_FILA_PACOTES     16
#define NET_TX_ESPERA_MS     100   /* acorda para o heartbeat mesmo sem pacotes */

static QueueHandle_t s_fila;

esp_err_t net_init(void)
{
    s_fila = xQueueCreate(NET_FILA_PACOTES, sizeof(net_pacote_t));
    if (s_fila == NULL) {
        ESP_LOGE(TAG, "sem memoria para a fila");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

bool net_enfileira(const net_pacote_t *p)
{
    if (s_fila == NULL) {
        return false;
    }
    /* TODO(F2): politica de descarte (99/1 mais antigo sai, 99/2 nunca) */
    return xQueueSend(s_fila, p, 0) == pdTRUE;
}

void net_tx_task(void *arg)
{
    (void)arg;
    static net_pacote_t pacote;   /* static: fora da pilha, sem malloc */

    for (;;) {
        diag_heartbeat(DIAG_T_NET_TX);
        if (xQueueReceive(s_fila, &pacote, pdMS_TO_TICKS(NET_TX_ESPERA_MS)) == pdTRUE) {
            /* TODO(F2): send() em cada conexao de pacote.destinos */
            if (pacote.primeiro_99_2) {
                diag_meas_desce();
            }
        }
    }
}
