/*
 * wifi.c - F0: stubs. TODO(F1): esp_wifi em modo STA, eventos de link, SNTP.
 */
#include "net.h"

#include <stdatomic.h>
#include <sys/time.h>

#include "esp_log.h"

static const char *TAG = "net";

static atomic_bool s_link_ok;

esp_err_t net_wifi_init(void)
{
    /* TODO(F1): esp_netif_init, esp_event_loop_create_default, esp_wifi_init,
     * credenciais de CONFIG_MU_WIFI_SSID/PASSWORD, esp_wifi_set_ps(WIFI_PS_NONE) */
    ESP_LOGW(TAG, "Wi-Fi ainda nao implementado (F1)");
    atomic_store(&s_link_ok, false);
    return ESP_OK;
}

bool net_link_ok(void)
{
    return atomic_load(&s_link_ok);
}

esp_err_t net_sntp_init(void)
{
    /* TODO(F1): esp_sntp com CONFIG_MU_SNTP_SERVER */
    return ESP_OK;
}

int64_t net_agora_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (int64_t)tv.tv_usec / 1000;
}
