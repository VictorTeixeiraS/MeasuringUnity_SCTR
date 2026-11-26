/*
 * teste.h - mini framework de testes (sem dependencias).
 */
#ifndef TESTE_H
#define TESTE_H

#include <math.h>
#include <stdio.h>

static int g_falhas = 0;
static int g_verificacoes = 0;

#define VERIFICA(cond)                                                        \
    do {                                                                      \
        g_verificacoes++;                                                     \
        if (!(cond)) {                                                        \
            g_falhas++;                                                       \
            printf("FALHA %s:%d: %s\n", __FILE__, __LINE__, #cond);          \
        }                                                                     \
    } while (0)

#define VERIFICA_PERTO(a, b, tol) VERIFICA(fabs((double)(a) - (double)(b)) <= (double)(tol))

#define RODA(fn)                                                              \
    do {                                                                      \
        printf("- %s\n", #fn);                                                \
        fn();                                                                 \
    } while (0)

#define RESULTADO()                                                           \
    (printf("%d verificacoes, %d falhas\n", g_verificacoes, g_falhas), g_falhas == 0 ? 0 : 1)

#endif /* TESTE_H */
