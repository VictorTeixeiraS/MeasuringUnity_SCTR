/*
 * diag.h - GPIOs de medicao, supervisor (watchdog) e log de transicoes.
 */
#ifndef DIAG_H
#define DIAG_H

#include <stdint.h>

#include "esp_err.h"
#include "mu_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DIAG_GPIO_MEAS   26   /* sobe em tsDeteccao, desce antes do send() do 1o 99/2 */
#define DIAG_GPIO_FRAME  27   /* alterna a cada quadro do DMA */

/* Tarefas supervisionadas */
typedef enum {
    DIAG_T_ACQ = 0,
    DIAG_T_TX_SCHEDULER,
    DIAG_T_NET_TX,
    DIAG_T_CONN_MANAGER,
    DIAG_T_CMD_SERVER,
    DIAG_N_TAREFAS
} diag_tarefa_t;

esp_err_t diag_init(void);

void diag_meas_sobe(void);
void diag_meas_desce(void);
void diag_frame_alterna(void);

/* Chamada por cada tarefa a cada iteracao do seu laco (atomico, nao bloqueia) */
void diag_heartbeat(diag_tarefa_t t);

/* Enfileira uma transicao para o supervisor registrar (nao bloqueia; usar como mu_fsm_log_fn) */
void diag_log_transicao(const mu_transicao_t *t, void *ctx);

/* Tarefa supervisor (Core 0, prioridade baixa, periodo 1 s) */
void supervisor(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* DIAG_H */
