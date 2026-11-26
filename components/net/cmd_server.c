/*
 * cmd_server.c - servidor TCP da porta 5100 (Core 0).
 * TODO(F2): accept nao bloqueante, separar linhas por '\n', proto_le_linha,
 * aplicar 100/10 e 100/11 (mu_config_aplica), responder 100/12 para a 5300.
 */
#include "net.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "diag.h"

#define CMD_ESPERA_MS  100

void cmd_server(void *arg)
{
    (void)arg;
    for (;;) {
        /* F2: select() com timeout de CMD_ESPERA_MS no socket de escuta */
        vTaskDelay(pdMS_TO_TICKS(CMD_ESPERA_MS));
        diag_heartbeat(DIAG_T_CMD_SERVER);
    }
}
