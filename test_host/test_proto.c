/*
 * test_proto.c - F0: verifica constantes do contrato v1.1 e que a API linka.
 * Testes de montagem e leitura dos pacotes nas fases F1/F2.
 */
#include "proto.h"
#include "teste.h"

static void constantes_do_contrato(void)
{
    VERIFICA(PROTO_VERSAO == 1);
    VERIFICA(PROTO_JANELA_MS == 17);
    VERIFICA(PROTO_N_PONTOS == 80);
    VERIFICA(PROTO_N_HARM == 4);
}

static void leitura_rejeita_linha_vazia(void)
{
    proto_rx_t rx;
    VERIFICA(proto_le_linha("", 0, &rx) < 0);
    VERIFICA(rx.tipo == PROTO_RX_INVALIDO);
}

int main(void)
{
    RODA(constantes_do_contrato);
    RODA(leitura_rejeita_linha_vazia);
    return RESULTADO();
}
