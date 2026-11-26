# net: Wi-Fi, SNTP, clientes TCP, servidor 5100, fila de envio e tx_scheduler
idf_component_register(
    SRCS "wifi.c" "net_tx.c" "conn_manager.c" "cmd_server.c" "tx_scheduler.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common proto
    PRIV_REQUIRES esp_wifi esp_netif esp_event lwip freertos log acq config diag
)
