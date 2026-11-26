# detector: detector de eventos e maquina de estados, C puro (testado em test_host/)
idf_component_register(
    SRCS "detector.c" "mu_fsm.c"
    INCLUDE_DIRS "include"
)
