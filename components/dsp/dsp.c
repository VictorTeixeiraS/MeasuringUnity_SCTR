/*
 * dsp.c - stubs da fase F0. Implementacao real na fase F4.
 */
#include "dsp.h"

#include <string.h>

size_t dsp_decima(const float *entrada, size_t n_entrada, size_t fator, float *saida)
{
    /* TODO(F4): media de 'fator' amostras */
    (void)entrada;
    (void)saida;
    if (fator == 0) {
        return 0;
    }
    return n_entrada / fator;
}

float dsp_mv_para_amperes(float mv)
{
    /* I_primario = (V_burden / R_burden) * relacao do TC */
    return (mv / 1000.0f) / DSP_R_BURDEN_OHM * DSP_RELACAO_TC;
}

void dsp_janela_init(dsp_janela_t *j)
{
    memset(j, 0, sizeof(*j));
}

void dsp_janela_insere(dsp_janela_t *j, const float *pontos, size_t n)
{
    /* TODO(F4): insercao no buffer circular */
    (void)j;
    (void)pontos;
    (void)n;
}

float dsp_rms(const dsp_janela_t *j)
{
    /* TODO(F4): RMS da janela de 80 pontos */
    (void)j;
    return 0.0f;
}

void dsp_harmonicas(const dsp_janela_t *j, float harm[DSP_N_HARMONICAS])
{
    /* TODO(F4): Goertzel nos bins k = 1..4 */
    (void)j;
    for (size_t k = 0; k < DSP_N_HARMONICAS; k++) {
        harm[k] = 0.0f;
    }
}
