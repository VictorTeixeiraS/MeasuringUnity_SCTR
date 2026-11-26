/*
 * tx_scheduler.c - monta pacotes quando o prazo do modo vence (Core 1, periodo 1 ms).
 * Nada bloqueante: le a ultima medida (buffer duplo), monta com proto e enfileira.
 * TODO(F2): prazo por modo (intervaloEnvioMs no normal, 1 ms no evento), seq e ts.
 */
#include "net.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "acq.h"
#include "diag.h"

#define TX_PERIODO_MS  1

void tx_scheduler(void *arg)
{
    (void)arg;
    TickType_t ultimo = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(TX_PERIODO_MS));
        diag_heartbeat(DIAG_T_TX_SCHEDULER);

        acq_medida_t m;
        if (!acq_le_ultima(&m)) {
            continue;
        }
        /* TODO(F2): se o prazo venceu, proto_monta_99_x e net_enfileira */
    }
}
