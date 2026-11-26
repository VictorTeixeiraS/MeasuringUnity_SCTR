/*
 * mu_fsm.h - maquina de estados da MU (CLAUDE.md, secao 5).
 * C puro. As tarefas geram eventos; so mu_fsm_processa muda o estado.
 */
#ifndef MU_FSM_H
#define MU_FSM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MU_E1_INICIALIZACAO = 0,
    MU_E2_ERRO_REDE,
    MU_E3_CONEXOES_TCP,
    MU_E4_MEDIDAS,
    MU_E5_AVALIA_EVENTO,
    MU_E6_NORMAL,
    MU_E7_TRANSITORIO,
    MU_E8_EVENTO,
    MU_E9_DESABILITADO,
    MU_E10_ENVIO,
    MU_N_ESTADOS
} mu_estado_t;

typedef enum {
    MU_EV_LINK_OK = 0,          /* Wi-Fi conectado (E1 -> E3, E2 -> E1) */
    MU_EV_SEM_LINK,             /* E1 sem link -> E2 */
    MU_EV_LINK_CAIU,            /* E4 -> E2 */
    MU_EV_CONEXOES_ABERTAS,     /* E3 -> E4 */
    MU_EV_QUADRO_PROCESSADO,    /* E4 -> E5 */
    MU_EV_DESABILITADO,         /* habilitado = 0: E4 -> E9 */
    MU_EV_HABILITADO,           /* habilitado = 1: E9 -> E4 */
    MU_EV_DELTA_NORMAL,         /* E5 -> E6 */
    MU_EV_DELTA_ACIMA,          /* E5 -> E7 (registra tsDeteccao) */
    MU_EV_DELTA_CONFIRMADO,     /* E7 -> E8, apos N quadros */
    MU_EV_ESTABILIZOU,          /* E7 -> E6, dI < 0,9 L */
    MU_EV_M_ESTAVEIS,           /* E8 -> E6, M quadros estaveis */
    MU_EV_RECONHECIDO,          /* E8 -> E6, 100/10 com comando = 0 */
    MU_EV_TIMEOUT_EVENTO,       /* E8 -> E6, 2 s em evento */
    MU_EV_PACOTE_PRONTO,        /* E6/E7/E8 -> E10 */
    MU_EV_ENVIO_CONCLUIDO,      /* E10 -> E4 */
    MU_N_EVENTOS
} mu_evento_t;

/* Registro de uma transicao (vai para o log) */
typedef struct {
    int64_t     t_ms;
    mu_estado_t anterior;
    mu_evento_t evento;
    mu_estado_t novo;
} mu_transicao_t;

/* Callback de log; quem chama decide como registrar (sem printf no Core 1) */
typedef void (*mu_fsm_log_fn)(const mu_transicao_t *t, void *ctx);

typedef struct {
    mu_estado_t   estado;
    mu_fsm_log_fn log;
    void         *log_ctx;
} mu_fsm_t;

void mu_fsm_init(mu_fsm_t *fsm, mu_fsm_log_fn log, void *log_ctx);

/* Unica funcao de transicao. Retorna o novo estado (igual ao atual se o evento nao se aplica). */
mu_estado_t mu_fsm_processa(mu_fsm_t *fsm, mu_evento_t ev, int64_t agora_ms);

const char *mu_estado_nome(mu_estado_t e);
const char *mu_evento_nome(mu_evento_t ev);

#ifdef __cplusplus
}
#endif

#endif /* MU_FSM_H */
