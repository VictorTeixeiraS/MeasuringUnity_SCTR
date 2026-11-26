/*
 * detector.c - stubs da fase F0. Implementacao real na fase F4.
 */
#include "detector.h"

#include <string.h>

void detector_init(detector_t *d, float i_inicial)
{
    memset(d, 0, sizeof(*d));
    d->modo = DET_MODO_NORMAL;
    d->i_ref = i_inicial;
}

mu_evento_t avalia_evento(detector_t *d, const det_param_t *p, float i_rms, int64_t agora_ms)
{
    /* TODO(F4): dI = |I_RMS - I_ref|, transicoes N/M, timeout de 2 s */
    (void)d;
    (void)p;
    (void)i_rms;
    (void)agora_ms;
    return MU_EV_DELTA_NORMAL;
}

void detector_reconhece(detector_t *d, float i_rms)
{
    d->modo = DET_MODO_NORMAL;
    d->i_ref = i_rms;
    d->cont_acima = 0;
    d->cont_estavel = 0;
}
