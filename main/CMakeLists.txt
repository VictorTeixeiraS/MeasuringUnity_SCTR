idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES acq net config diag detector
    PRIV_REQUIRES nvs_flash esp_timer
)
