/*
 * detector.h - detector de degraus de corrente (CLAUDE.md, secao 4).
 * C puro. Compara o RMS com uma referencia lenta, nunca quadro com quadro.
 */
#ifndef DETECTOR_H
#define DETECTOR_H

#include <stdbool.h>
#include <stdint.h>

#include "mu_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DET_ALFA_PADRAO        (1.0f / 64.0f)
#define DET_LIMIAR_PADRAO_A    6.0f
#define DET_HISTERESE          0.9f     /* saida quando dI < 0,9 L */
#define DET_N_CONFIRMA         2        /* quadros extras para confirmar evento */
#define DET_M_ESTAVEIS         4        /* quadros estaveis para sair do evento */
#define DET_TIMEOUT_EVENTO_MS  2000

typedef enum {
    DET_MODO_NORMAL = 0,
    DET_MODO_TRANSITORIO,
    DET_MODO_EVENTO
} det_modo_t;

typedef struct {
    float limiar_a;       /* L = limiarDeltaI */
    float alfa;
} det_param_t;

typedef struct {
    det_modo_t modo;
    float      i_ref;
    int        cont_acima;
    int        cont_estavel;
    int64_t    ts_deteccao_ms;
    int64_t    ts_inicio_evento_ms;
} detector_t;

void detector_init(detector_t *d, float i_inicial);

/*
 * Aplica o detector a um novo RMS (Codigo 2 do relatorio).
 * Retorna o evento para a maquina de estados.
 */
mu_evento_t avalia_evento(detector_t *d, const det_param_t *p, float i_rms, int64_t agora_ms);

/* 100/10 com comando = 0: encerra o evento e faz I_ref = I_RMS */
void detector_reconhece(detector_t *d, float i_rms);

#ifdef __cplusplus
}
#endif

#endif /* DETECTOR_H */
