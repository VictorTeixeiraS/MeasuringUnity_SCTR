# proto: montagem e leitura dos pacotes, C puro (testado em test_host/)
# cJSON vem do IDF Component Manager (espressif/cjson, ver idf_component.yml):
# o componente "json" foi removido do ESP-IDF v6.
idf_component_register(
    SRCS "proto.c"
    INCLUDE_DIRS "include"
)
