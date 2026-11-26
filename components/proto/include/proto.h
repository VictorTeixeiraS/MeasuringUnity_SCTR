/*
 * proto.h - Contrato de Pacotes v1.1 (CLAUDE.md, secao 7). Nao alterar o contrato.
 * C puro. Montagem com snprintf em buffer fixo; leitura com cJSON.
 */
#ifndef PROTO_H
#define PROTO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROTO_VERSAO          1
#define PROTO_MODULO          "1"
#define PROTO_JANELA_MS       17
#define PROTO_N_PONTOS        80
#define PROTO_N_HARM          4
#define PROTO_MAX_LINHA       512     /* bytes, incluindo o '\n' final */
#define PROTO_MAX_PARAMETRO   32
#define PROTO_MAX_MOTIVO      64

/* Cabecalho comum a todo pacote enviado */
typedef struct {
    int     id_origem;     /* = idMU */
    int64_t seq;           /* crescente por emissor */
    int64_t ts_ms;         /* epoch ms UTC */
} proto_cab_t;

/* Campos de medida de 99/1 e 99/2 */
typedef struct {
    int         id_mu;
    const char *id_ativo;
    float       ia;
    float       ib;        /* sempre 0.0 (sensor unico na fase A) */
    float       ic;        /* sempre 0.0 */
    bool        tem_freq;
    float       freq;
    bool        tem_harm;
    float       harm_a[PROTO_N_HARM];
} proto_medida_t;

/*
 * Funcoes de montagem: escrevem um JSON terminado em '\n' em 'buf'.
 * Retornam o numero de bytes escritos (sem o '\0') ou -1 se nao couber.
 */
int proto_monta_1_1(char *buf, size_t cap, const proto_cab_t *cab);
int proto_monta_99_1(char *buf, size_t cap, const proto_cab_t *cab, const proto_medida_t *m);
int proto_monta_99_2(char *buf, size_t cap, const proto_cab_t *cab, const proto_medida_t *m,
                     const char *fases_discrepantes, int64_t ts_deteccao_ms);
int proto_monta_100_12(char *buf, size_t cap, const proto_cab_t *cab, int id_mu,
                       int64_t id_requisicao, bool aceito, const char *motivo);

/* Pacotes recebidos na porta 5100 */
typedef enum {
    PROTO_RX_INVALIDO = 0,
    PROTO_RX_1_1,
    PROTO_RX_100_10,
    PROTO_RX_100_11
} proto_rx_tipo_t;

typedef struct {
    int     id_mu;
    int     comando;       /* 0 reconhecido, 1 aumenta taxa, 2 reduz */
    double  parametro1;
    int64_t id_evento;
} proto_100_10_t;

typedef struct {
    int64_t id_requisicao;
    int     id_mu;
    char    parametro[PROTO_MAX_PARAMETRO];
    double  valor;
} proto_100_11_t;

typedef struct {
    proto_rx_tipo_t tipo;
    int             id_origem;
    int64_t         seq;
    union {
        proto_100_10_t p100_10;
        proto_100_11_t p100_11;
    } u;
} proto_rx_t;

/* Le uma linha JSON (sem o '\n'). Retorna 0 se valida; negativo caso contrario. */
int proto_le_linha(const char *linha, size_t len, proto_rx_t *out);

#ifdef __cplusplus
}
#endif

#endif /* PROTO_H */
