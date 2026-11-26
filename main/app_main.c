/*
 * app_main.c - inicializacao e criacao das tarefas da Merging Unit (Modulo 1a).
 * Regra central: Core 1 mede e decide; Core 0 cuida da rede.
 */
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "acq.h"
#include "diag.h"
#include "mu_config.h"
#include "net.h"

static const char *TAG = "main";

#define CORE_REDE    0
#define CORE_MEDIDA  1

/* Prioridades (configMAX_PRIORITIES = 25; Wi-Fi = 23 e lwIP = 18 ficam no Core 0) */
#define PRIO_ACQ           22   /* mais alta da aplicacao */
#define PRIO_TX_SCHEDULER  21
#define PRIO_NET_TX        10
#define PRIO_CONN_MANAGER  10
#define PRIO_CMD_SERVER    10
#define PRIO_SUPERVISOR    2

#define PILHA_ACQ           4096
#define PILHA_TX_SCHEDULER  4096
#define PILHA_NET_TX        4096
#define PILHA_CONN_MANAGER  4096
#define PILHA_CMD_SERVER    6144   /* cJSON */
#define PILHA_SUPERVISOR    3072

static void cria_tarefa(TaskFunction_t fn, const char *nome, uint32_t pilha,
                        UBaseType_t prio, BaseType_t core)
{
    BaseType_t ok = xTaskCreatePinnedToCore(fn, nome, pilha, NULL, prio, NULL, core);
    if (ok != pdPASS) {
        ESP_LOGE(TAG, "falha ao criar %s", nome);
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(mu_config_init());
    ESP_ERROR_CHECK(diag_init());
    ESP_ERROR_CHECK(net_init());
    ESP_ERROR_CHECK(acq_init());
    ESP_ERROR_CHECK(net_wifi_init());
    ESP_ERROR_CHECK(net_sntp_init());

    /* Core 1: medicao e decisao */
    cria_tarefa(acq_task, "acq_task", PILHA_ACQ, PRIO_ACQ, CORE_MEDIDA);
    cria_tarefa(tx_scheduler, "tx_scheduler", PILHA_TX_SCHEDULER, PRIO_TX_SCHEDULER, CORE_MEDIDA);

    /* Core 0: rede e supervisao */
    cria_tarefa(net_tx_task, "net_tx_task", PILHA_NET_TX, PRIO_NET_TX, CORE_REDE);
    cria_tarefa(conn_manager, "conn_manager", PILHA_CONN_MANAGER, PRIO_CONN_MANAGER, CORE_REDE);
    cria_tarefa(cmd_server, "cmd_server", PILHA_CMD_SERVER, PRIO_CMD_SERVER, CORE_REDE);
    cria_tarefa(supervisor, "supervisor", PILHA_SUPERVISOR, PRIO_SUPERVISOR, CORE_REDE);

    ESP_ERROR_CHECK(acq_start());
    ESP_LOGI(TAG, "MU iniciada");
}
