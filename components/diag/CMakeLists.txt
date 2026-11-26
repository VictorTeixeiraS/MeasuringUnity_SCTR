# diag: GPIOs de medicao, watchdog, estatisticas
idf_component_register(
    SRCS "diag.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common detector
    PRIV_REQUIRES esp_driver_gpio esp_system freertos log
)
