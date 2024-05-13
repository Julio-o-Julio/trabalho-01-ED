#ifndef __CONSULTA_CIDADE__
#define __CONSULTA_CIDADE__
#include <stdint.h>

typedef struct {
    uintptr_t * table;
    int size;
    int max;
    uintptr_t deleted;
    char * (*get_key)(void *);
}Thash;

uint32_t hashf(const char* str, uint32_t h);
int hash_insere(Thash * h, void * bucket);
int hash_constroi(Thash * h,int nbuckets, char * (*get_key)(void *) );
void * hash_busca(Thash h, const char * key);
void hash_apaga(Thash *h);

#endif
