#ifndef __CIDADE__
#define __CIDADE__

typedef struct {
    int codigo_ibge;
    char nome[64];
    double latitude;
    double longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char fuso_horario[64];
}Cidade;

Cidade * criar_cidade(char * dados);

#endif
