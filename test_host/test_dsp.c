/*
 * test_dsp.c - F0: apenas o que ja esta implementado. Testes de RMS e Goertzel na F4.
 */
#include "dsp.h"
#include "teste.h"

static void conversao_mv_para_amperes(void)
{
    /* 33 mV no burden de 33 ohm = 1 mA no secundario = 2 A no primario */
    VERIFICA_PERTO(dsp_mv_para_amperes(33.0f), 2.0f, 1e-4);
    VERIFICA_PERTO(dsp_mv_para_amperes(0.0f), 0.0f, 1e-6);
}

static void janela_inicia_vazia(void)
{
    dsp_janela_t j;
    dsp_janela_init(&j);
    VERIFICA(j.cheia == 0);
    VERIFICA(j.idx == 0);
    VERIFICA_PERTO(dsp_rms(&j), 0.0f, 1e-6);
}

static void decimacao_conta_pontos(void)
{
    float entrada[80] = { 0 };
    float saida[20];
    VERIFICA(dsp_decima(entrada, 80, DSP_FATOR_DECIMACAO, saida) == 20);
    VERIFICA(dsp_decima(entrada, 80, 0, saida) == 0);
}

int main(void)
{
    RODA(conversao_mv_para_amperes);
    RODA(janela_inicia_vazia);
    RODA(decimacao_conta_pontos);
    return RESULTADO();
}
