/*
 * mu_fsm.c - maquina de estados da MU.
 * F0: enums, nomes e log prontos; tabela de transicoes nas fases F2 (rede) e F4 (medicao).
 */
#include "mu_fsm.h"

#include <stddef.h>

static const char *const NOMES_ESTADO[MU_N_ESTADOS] = {
    [MU_E1_INICIALIZACAO] = "E1_INICIALIZACAO",
    [MU_E2_ERRO_REDE]     = "E2_ERRO_REDE",
    [MU_E3_CONEXOES_TCP]  = "E3_CONEXOES_TCP",
    [MU_E4_MEDIDAS]       = "E4_MEDIDAS",
    [MU_E5_AVALIA_EVENTO] = "E5_AVALIA_EVENTO",
    [MU_E6_NORMAL]        = "E6_NORMAL",
    [MU_E7_TRANSITORIO]   = "E7_TRANSITORIO",
    [MU_E8_EVENTO]        = "E8_EVENTO",
    [MU_E9_DESABILITADO]  = "E9_DESABILITADO",
    [MU_E10_ENVIO]        = "E10_ENVIO",
};

static const char *const NOMES_EVENTO[MU_N_EVENTOS] = {
    [MU_EV_LINK_OK]           = "LINK_OK",
    [MU_EV_SEM_LINK]          = "SEM_LINK",
    [MU_EV_LINK_CAIU]         = "LINK_CAIU",
    [MU_EV_CONEXOES_ABERTAS]  = "CONEXOES_ABERTAS",
    [MU_EV_QUADRO_PROCESSADO] = "QUADRO_PROCESSADO",
    [MU_EV_DESABILITADO]      = "DESABILITADO",
    [MU_EV_HABILITADO]        = "HABILITADO",
    [MU_EV_DELTA_NORMAL]      = "DELTA_NORMAL",
    [MU_EV_DELTA_ACIMA]       = "DELTA_ACIMA",
    [MU_EV_DELTA_CONFIRMADO]  = "DELTA_CONFIRMADO",
    [MU_EV_ESTABILIZOU]       = "ESTABILIZOU",
    [MU_EV_M_ESTAVEIS]        = "M_ESTAVEIS",
    [MU_EV_RECONHECIDO]       = "RECONHECIDO",
    [MU_EV_TIMEOUT_EVENTO]    = "TIMEOUT_EVENTO",
    [MU_EV_PACOTE_PRONTO]     = "PACOTE_PRONTO",
    [MU_EV_ENVIO_CONCLUIDO]   = "ENVIO_CONCLUIDO",
};

void mu_fsm_init(mu_fsm_t *fsm, mu_fsm_log_fn log, void *log_ctx)
{
    fsm->estado = MU_E1_INICIALIZACAO;
    fsm->log = log;
    fsm->log_ctx = log_ctx;
}

/* Tabela de transicoes. F0: nenhuma transicao definida (retorna o proprio estado). */
static mu_estado_t proximo_estado(mu_estado_t atual, mu_evento_t ev)
{
    /* TODO(F2/F4): implementar a tabela da secao 5 do CLAUDE.md */
    (void)ev;
    return atual;
}

mu_estado_t mu_fsm_processa(mu_fsm_t *fsm, mu_evento_t ev, int64_t agora_ms)
{
    if ((unsigned)ev >= MU_N_EVENTOS) {
        return fsm->estado;
    }
    mu_estado_t anterior = fsm->estado;
    mu_estado_t novo = proximo_estado(anterior, ev);
    if (novo != anterior) {
        fsm->estado = novo;
        if (fsm->log != NULL) {
            mu_transicao_t t = { .t_ms = agora_ms, .anterior = anterior, .evento = ev, .novo = novo };
            fsm->log(&t, fsm->log_ctx);
        }
    }
    return fsm->estado;
}

const char *mu_estado_nome(mu_estado_t e)
{
    return ((unsigned)e < MU_N_ESTADOS) ? NOMES_ESTADO[e] : "?";
}

const char *mu_evento_nome(mu_evento_t ev)
{
    return ((unsigned)ev < MU_N_EVENTOS) ? NOMES_EVENTO[ev] : "?";
}
