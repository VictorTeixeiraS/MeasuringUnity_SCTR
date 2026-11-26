/*
 * test_detector.c - F0: estrutura da maquina de estados e do detector.
 * Testes das transicoes (secao 5) e do detector (secao 4) nas fases F2/F4.
 */
#include <string.h>

#include "detector.h"
#include "mu_fsm.h"
#include "teste.h"

static int g_logs = 0;

static void conta_log(const mu_transicao_t *t, void *ctx)
{
    (void)t;
    (void)ctx;
    g_logs++;
}

static void fsm_comeca_em_e1(void)
{
    mu_fsm_t fsm;
    mu_fsm_init(&fsm, conta_log, NULL);
    VERIFICA(fsm.estado == MU_E1_INICIALIZACAO);
}

static void fsm_ignora_evento_invalido(void)
{
    mu_fsm_t fsm;
    mu_fsm_init(&fsm, conta_log, NULL);
    g_logs = 0;
    VERIFICA(mu_fsm_processa(&fsm, MU_N_EVENTOS, 0) == MU_E1_INICIALIZACAO);
    VERIFICA(g_logs == 0);
}

static void nomes_definidos(void)
{
    for (int e = 0; e < MU_N_ESTADOS; e++) {
        const char *n = mu_estado_nome((mu_estado_t)e);
        VERIFICA(n != NULL && strcmp(n, "?") != 0);
    }
    for (int ev = 0; ev < MU_N_EVENTOS; ev++) {
        const char *n = mu_evento_nome((mu_evento_t)ev);
        VERIFICA(n != NULL && strcmp(n, "?") != 0);
    }
    VERIFICA(strcmp(mu_estado_nome(MU_N_ESTADOS), "?") == 0);
}

static void detector_inicia_normal(void)
{
    detector_t d;
    detector_init(&d, 12.4f);
    VERIFICA(d.modo == DET_MODO_NORMAL);
    VERIFICA_PERTO(d.i_ref, 12.4f, 1e-6);
}

static void reconhecimento_volta_ao_normal(void)
{
    detector_t d;
    detector_init(&d, 10.0f);
    d.modo = DET_MODO_EVENTO;
    detector_reconhece(&d, 30.0f);
    VERIFICA(d.modo == DET_MODO_NORMAL);
    VERIFICA_PERTO(d.i_ref, 30.0f, 1e-6);
}

int main(void)
{
    RODA(fsm_comeca_em_e1);
    RODA(fsm_ignora_evento_invalido);
    RODA(nomes_definidos);
    RODA(detector_inicia_normal);
    RODA(reconhecimento_volta_ao_normal);
    return RESULTADO();
}
