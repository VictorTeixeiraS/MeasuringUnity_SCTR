/*
 * net.h - Wi-Fi, SNTP, clientes TCP (5200/5300/5400), servidor 5100 e fila de envio.
 * Regra: tudo aqui roda no Core 0, exceto tx_scheduler (Core 1, nao bloqueante).
 */
#ifndef NET_H
#define NET_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "proto.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NET_DEST_M2 = 0,   /* 5200 Protecao */
    NET_DEST_M3,       /* 5300 Supervisorio */
    NET_DEST_M4,       /* 5400 Filtros */
    NET_N_DEST
} net_dest_t;

#define NET_DEST_BIT(d)     (1u << (d))
#define NET_DEST_TODOS      (NET_DEST_BIT(NET_DEST_M2) | NET_DEST_BIT(NET_DEST_M3) | NET_DEST_BIT(NET_DEST_M4))

/* Pacote pronto para envio (buffer fixo, sem malloc) */
typedef struct {
    uint8_t  destinos;      /* mascara de NET_DEST_BIT */
    bool     evento;        /* 99/2: nunca descartado */
    bool     primeiro_99_2; /* baixa DBG_MEAS antes do send() */
    uint16_t len;
    char     dados[PROTO_MAX_LINHA];
} net_pacote_t;

/* Cria filas e estruturas. Chamar antes das tarefas. */
esp_err_t net_init(void);

/* Wi-Fi em modo estacao, WIFI_PS_NONE (F1) */
esp_err_t net_wifi_init(void);
bool      net_link_ok(void);

/* SNTP e relogio UTC em ms (F1) */
esp_err_t net_sntp_init(void);
int64_t   net_agora_ms(void);

/* Enfileira sem bloquear. Fila cheia: descarta o 99/1 mais antigo, nunca um 99/2. */
bool net_enfileira(const net_pacote_t *p);

/* Tarefas */
void tx_scheduler(void *arg);   /* Core 1, periodo 1 ms */
void net_tx_task(void *arg);    /* Core 0, consome a fila */
void conn_manager(void *arg);   /* Core 0, periodo 100 ms */
void cmd_server(void *arg);     /* Core 0, porta 5100 */

#ifdef __cplusplus
}
#endif

#endif /* NET_H */
