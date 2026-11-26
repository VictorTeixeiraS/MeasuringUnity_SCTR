# config: parametros na NVS
idf_component_register(
    SRCS "mu_config.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common
    PRIV_REQUIRES nvs_flash log
)
