/*
 * mu_config.h - parametros da MU (NVS + padroes do menu "MU Configuration").
 * Parametros de execucao: copia completa com troca atomica de ponteiro.
 * Quem le (acq_task) pega o ponteiro uma vez por quadro, entre quadros.
 */
#ifndef MU_CONFIG_H
#define MU_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MU_CFG_MAX_IP       16
#define MU_CFG_MAX_ATIVO    32

/* Parametros alteraveis pelo 100/11 */
typedef struct {
    int   intervalo_envio_ms;   /* <= 100 */
    bool  habilitado;
    float canal_a_ganho;
    float canal_a_offset;
    float limiar_delta_i;       /* A */
} mu_param_t;

/* Identificacao e enderecos dos receptores (fixos apos a inicializacao) */
typedef struct {
    int      id_mu;
    char     id_ativo[MU_CFG_MAX_ATIVO];
    char     ip_m2[MU_CFG_MAX_IP];
    uint16_t porta_m2;
    char     ip_m3[MU_CFG_MAX_IP];
    uint16_t porta_m3;
    char     ip_m4[MU_CFG_MAX_IP];
    uint16_t porta_m4;
    uint16_t porta_cmd;
} mu_rede_cfg_t;

/* Carrega padroes do Kconfig e sobrescreve com a NVS. Chamar apos nvs_flash_init(). */
esp_err_t mu_config_init(void);

/* Ponteiro para a copia ativa dos parametros (leitura atomica, nunca NULL apos init) */
const mu_param_t *mu_config_param(void);

const mu_rede_cfg_t *mu_config_rede(void);

/*
 * Valida e aplica um parametro do 100/11 (copia + troca atomica + NVS).
 * Valor invalido: retorna false, preenche 'motivo' e nao altera nada.
 */
bool mu_config_aplica(const char *parametro, double valor, const char **motivo);

#ifdef __cplusplus
}
#endif

#endif /* MU_CONFIG_H */
