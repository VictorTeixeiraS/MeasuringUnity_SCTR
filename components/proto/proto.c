/*
 * proto.c - stubs da fase F0. Implementacao real nas fases F1/F2.
 */
#include "proto.h"

#include <string.h>

int proto_monta_1_1(char *buf, size_t cap, const proto_cab_t *cab)
{
    /* TODO(F1): {"tipo":"1/1",...,"intervaloMs":1000} */
    (void)buf;
    (void)cap;
    (void)cab;
    return -1;
}

int proto_monta_99_1(char *buf, size_t cap, const proto_cab_t *cab, const proto_medida_t *m)
{
    /* TODO(F2) */
    (void)buf;
    (void)cap;
    (void)cab;
    (void)m;
    return -1;
}

int proto_monta_99_2(char *buf, size_t cap, const proto_cab_t *cab, const proto_medida_t *m,
                     const char *fases_discrepantes, int64_t ts_deteccao_ms)
{
    /* TODO(F2) */
    (void)buf;
    (void)cap;
    (void)cab;
    (void)m;
    (void)fases_discrepantes;
    (void)ts_deteccao_ms;
    return -1;
}

int proto_monta_100_12(char *buf, size_t cap, const proto_cab_t *cab, int id_mu,
                       int64_t id_requisicao, bool aceito, const char *motivo)
{
    /* TODO(F2) */
    (void)buf;
    (void)cap;
    (void)cab;
    (void)id_mu;
    (void)id_requisicao;
    (void)aceito;
    (void)motivo;
    return -1;
}

int proto_le_linha(const char *linha, size_t len, proto_rx_t *out)
{
    /* TODO(F2): leitura com cJSON e validacao dos campos */
    (void)linha;
    (void)len;
    memset(out, 0, sizeof(*out));
    out->tipo = PROTO_RX_INVALIDO;
    return -1;
}
