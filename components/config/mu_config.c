/*
 * mu_config.c - F0: padroes do Kconfig e troca atomica de ponteiro.
 * TODO(F2): leitura/gravacao na NVS e validacao do 100/11.
 */
#include "mu_config.h"

#include <stdatomic.h>
#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "config";

/* Dois buffers: um ativo, outro para montar a proxima copia */
static mu_param_t s_param[2];
static _Atomic(const mu_param_t *) s_param_ativo;
static mu_rede_cfg_t s_rede;

static void copia_str(char *dst, size_t cap, const char *src)
{
    strncpy(dst, src, cap - 1);
    dst[cap - 1] = '\0';
}

esp_err_t mu_config_init(void)
{
    mu_param_t *p = &s_param[0];
    p->intervalo_envio_ms = CONFIG_MU_INTERVALO_ENVIO_MS;
    p->habilitado = true;
    p->canal_a_ganho = (float)CONFIG_MU_CANAL_A_GANHO_MILLI / 1000.0f;
    p->canal_a_offset = (float)CONFIG_MU_CANAL_A_OFFSET_MA / 1000.0f;
    p->limiar_delta_i = (float)CONFIG_MU_LIMIAR_DELTA_I_DA / 10.0f;
    atomic_store(&s_param_ativo, p);

    s_rede.id_mu = CONFIG_MU_ID;
    copia_str(s_rede.id_ativo, sizeof(s_rede.id_ativo), CONFIG_MU_ID_ATIVO);
    copia_str(s_rede.ip_m2, sizeof(s_rede.ip_m2), CONFIG_MU_M2_IP);
    copia_str(s_rede.ip_m3, sizeof(s_rede.ip_m3), CONFIG_MU_M3_IP);
    copia_str(s_rede.ip_m4, sizeof(s_rede.ip_m4), CONFIG_MU_M4_IP);
    s_rede.porta_m2 = CONFIG_MU_M2_PORT;
    s_rede.porta_m3 = CONFIG_MU_M3_PORT;
    s_rede.porta_m4 = CONFIG_MU_M4_PORT;
    s_rede.porta_cmd = CONFIG_MU_CMD_PORT;

    /* TODO(F2): sobrescrever com valores da NVS */
    ESP_LOGI(TAG, "idMU=%d idAtivo=%s intervalo=%d ms L=%.1f A",
             s_rede.id_mu, s_rede.id_ativo, p->intervalo_envio_ms, (double)p->limiar_delta_i);
    return ESP_OK;
}

const mu_param_t *mu_config_param(void)
{
    return atomic_load(&s_param_ativo);
}

const mu_rede_cfg_t *mu_config_rede(void)
{
    return &s_rede;
}

bool mu_config_aplica(const char *parametro, double valor, const char **motivo)
{
    /* TODO(F2): validar, copiar para o buffer inativo, trocar o ponteiro, gravar na NVS */
    (void)parametro;
    (void)valor;
    if (motivo != NULL) {
        *motivo = "nao implementado";
    }
    return false;
}
