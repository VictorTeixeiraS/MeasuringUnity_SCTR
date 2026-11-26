/*
 * dsp.h - decimacao, RMS de 1 ciclo e harmonicas por Goertzel.
 * C puro: sem includes do ESP-IDF ou do FreeRTOS.
 */
#ifndef DSP_H
#define DSP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DSP_FATOR_DECIMACAO   4     /* 19.200 Hz -> 4.800 Hz por canal */
#define DSP_PONTOS_JANELA     80    /* 1 ciclo de 60 Hz a 4.800 Hz */
#define DSP_N_HARMONICAS      4     /* bins k = 1..4 (60 a 240 Hz) */

#define DSP_R_BURDEN_OHM      33.0f
#define DSP_RELACAO_TC        2000.0f

/* Janela circular de 1 ciclo (sinal ja sem offset, em A no primario) */
typedef struct {
    float  amostra[DSP_PONTOS_JANELA];
    size_t idx;      /* proxima posicao de escrita */
    size_t cheia;    /* numero de pontos validos (satura em DSP_PONTOS_JANELA) */
} dsp_janela_t;

/*
 * Media de 'fator' amostras consecutivas.
 * Retorna o numero de pontos escritos em 'saida' (n_entrada / fator).
 */
size_t dsp_decima(const float *entrada, size_t n_entrada, size_t fator, float *saida);

/* Converte tensao no burden (mV) para corrente no primario (A) */
float dsp_mv_para_amperes(float mv);

void  dsp_janela_init(dsp_janela_t *j);
void  dsp_janela_insere(dsp_janela_t *j, const float *pontos, size_t n);

/* RMS da janela (A); 0 se a janela ainda nao esta cheia */
float dsp_rms(const dsp_janela_t *j);

/* Amplitude RMS dos bins k = 1..DSP_N_HARMONICAS; harm[0] e a fundamental */
void  dsp_harmonicas(const dsp_janela_t *j, float harm[DSP_N_HARMONICAS]);

#ifdef __cplusplus
}
#endif

#endif /* DSP_H */
