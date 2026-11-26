/*
 * conn_manager.c - estado das conexoes TCP (Core 0, periodo 100 ms).
 * TODO(F1/F2): reconexao a cada 500 ms, 1/1 a cada 1 s, receptor perdido apos 3 s,
 * eventos de link para a maquina de estados (acq_envia_evento).
 */
#include "net.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "diag.h"

#define CONN_PERIODO_MS  100

void conn_manager(void *arg)
{
    (void)arg;
    TickType_t ultimo = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&ultimo, pdMS_TO_TICKS(CONN_PERIODO_MS));
        diag_heartbeat(DIAG_T_CONN_MANAGER);
    }
}
