#ifndef __VIZINHOS_PROXIMOS__
#define __VIZINHOS_PROXIMOS__
#include "cidade.h"

typedef struct _tnode{
    void * reg;
    struct _tnode *esq;
    struct _tnode *dir;
}Tnode;

typedef struct {
    Cidade *cidade;
    double distancia;
}CidadeDistancia;

#endif
