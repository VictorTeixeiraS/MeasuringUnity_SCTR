# acq: ADC continuo (DMA), decimacao e acq_task (Core 1)
idf_component_register(
    SRCS "acq.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common soc dsp detector
    PRIV_REQUIRES esp_adc esp_timer freertos log config diag
)
