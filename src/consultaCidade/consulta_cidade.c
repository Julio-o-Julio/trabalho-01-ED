#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#define SEED    0x12345678
#include "../../include/consulta_cidade.h"
#include "../../include/cidade.h"

uint32_t hashf(const char* str, uint32_t h){
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

int hash_insere(Thash * h, void * bucket){
    uint32_t hash = hashf(h->get_key(bucket),SEED);
    int pos = hash %(h->max);
    /*se esta cheio*/
    if (h->max == (h->size+1)){
        free(bucket);
        return EXIT_FAILURE;
    }else{  /*fazer a insercao*/
        while(h->table[pos] != 0){
            if (h->table[pos] == h->deleted)
                break;
            pos = (pos+1)%h->max;
        }
        h->table[pos] = (uintptr_t)bucket;
        h->size +=1;
    }
    return EXIT_SUCCESS;
}

int hash_constroi(Thash * h, int nbuckets, char * (*get_key)(void *) ){
    h->table = calloc(sizeof(void *),nbuckets + 1);
    if (h->table == NULL){
        return EXIT_FAILURE;
    }
    h->max = nbuckets + 1;
    h->size = 0;
    h->deleted = (uintptr_t)&(h->size);
    h->get_key = get_key;

    return EXIT_SUCCESS;
}

void * hash_busca(Thash  h, const char * key){
    int pos = hashf(key,SEED) % (h.max);
    void * ret = NULL;
    while(h.table[pos]!=0 && ret == NULL){
        if (strcmp(h.get_key((void*)h.table[pos]),key) == 0){
            ret = (void *) h.table[pos];
        }else{
            pos = (pos+1) % h.max;
        }
    }
    return ret;
}

void hash_apaga(Thash *h){
    int pos;
    for (pos =0; pos < h->max;pos++){
        if (h->table[pos] != 0){
            if (h->table[pos]!=h->deleted){
                free((void*) h->table[pos]);
            }
        }
    }
    free(h->table);
}

int isValidLine(char linha[]) {
    while (*linha != '\0') {
        if (*linha == '{' || *linha == '}') {
            return EXIT_FAILURE;
        } else if (*linha == '"') {
            return EXIT_SUCCESS;
        }
        linha++;
    }
    return EXIT_SUCCESS;
}

Cidade * criar_cidade(char dados[]) {
    Cidade * cidade = malloc(sizeof(Cidade));
    if (cidade == NULL) {
        printf("Erro ao alocar memória para a cidade\n");
        exit(EXIT_FAILURE);
    }

    char *next_field;

    next_field = strstr(dados, "\"codigo_ibge\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%d", &cidade->codigo_ibge);
        }
    }

    next_field = strstr(dados, "\"nome\":");
    if (next_field != NULL) {
        regex_t regex;
        regmatch_t matches[2];
        char pattern[] = "\"nome\":\\s*\"([^\"]+)\"";

        if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
            printf("Erro ao compilar a expressão regular.\n");
            exit(EXIT_FAILURE);
        }

        if (regexec(&regex, next_field, 2, matches, 0) == 0) {
            int start = matches[1].rm_so;
            int end = matches[1].rm_eo;
            int length = end - start;

            strncpy(cidade->nome, next_field + start, length);
            cidade->nome[length] = '\0';
        } else {
            printf("Nome da cidade não encontrado.\n");
            exit(EXIT_FAILURE);
        }

        regfree(&regex);
    }

    next_field = strstr(dados, "\"latitude\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%lf", &cidade->latitude);
        }
    }

    next_field = strstr(dados, "\"longitude\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%lf", &cidade->longitude);
        }
    }

    next_field = strstr(dados, "\"capital\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%d", &cidade->capital);
        }
    }

    next_field = strstr(dados, "\"codigo_uf\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%d", &cidade->codigo_uf);
        }
    }

    next_field = strstr(dados, "\"siafi_id\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%d", &cidade->siafi_id);
        }
    }

    next_field = strstr(dados, "\"ddd\":");
    if (next_field != NULL) {
        next_field = strchr(next_field, ':');
        if (next_field != NULL) {
            sscanf(next_field + 1, "%d", &cidade->ddd);
        }
    }

    next_field = strstr(dados, "\"fuso_horario\":");
    if (next_field != NULL) {
        regex_t regex;
        regmatch_t matches[2];
        char pattern[] = "\"fuso_horario\":\\s*\"([^\"]+)\"";

        if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
            printf("Erro ao compilar a expressão regular.\n");
            exit(EXIT_FAILURE);
        }

        if (regexec(&regex, next_field, 2, matches, 0) == 0) {
            int start = matches[1].rm_so;
            int end = matches[1].rm_eo;
            int length = end - start;

            strncpy(cidade->fuso_horario, next_field + start, length);
            cidade->fuso_horario[length] = '\0';
        } else {
            printf("Fuso horário não encontrado.\n");
            exit(EXIT_FAILURE);
        }

        regfree(&regex);
    }
    
    return cidade;
}

char * get_key_cidade(void * cidade) {
    Cidade *c = (Cidade *)cidade;
    char *key = malloc(sizeof(char) * 12);
    if (key == NULL) {
        printf("Erro ao alocar memoria para a chave\n");
        exit(EXIT_FAILURE);
    }
    sprintf(key, "%d", c->codigo_ibge);
    return key;
}

Cidade *busca_cidade_por_codigo(Thash hash_cidades, int codigo_ibge) {
    char chave[12];
    sprintf(chave, "%d", codigo_ibge);

    Cidade *cidade_encontrada = (Cidade *)hash_busca(hash_cidades, chave);

    return cidade_encontrada;
}

int main(int argc, char *argv[]){
    if (argc < 3) {
        printf("Obrigatorio ter 2 parametros\n");
        return EXIT_FAILURE;
    }

    FILE * cidadesJson = fopen(argv[1], "r");
    if (cidadesJson == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    Thash hash_cidades;
    if (hash_constroi(&hash_cidades, 12000, get_key_cidade) != EXIT_SUCCESS) {
        printf("Erro ao criar a tabela hash\n");
        fclose(cidadesJson);
        return EXIT_FAILURE;
    }

    char linha[128];
    char dados[1024] = "";
    int count = 0;
    while (fgets(linha, sizeof(linha), cidadesJson) != NULL) {
        if (isValidLine(linha)) { continue; }

        strcpy(linha, linha + 8);

        strcat(dados, linha);
        count++;

        if (count == 9) {
            Cidade * cidade = criar_cidade(dados);

            if (hash_insere(&hash_cidades, cidade) != EXIT_SUCCESS) {
                printf("Erro ao inserir a cidade na hash\n");
            }

            dados[0] = '\0';
            count = 0;
        }
    }

    fclose(cidadesJson);

    char *endptr;
    int codigo_ibge = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        printf("O segundo parametro tem que ser int\n");
        return EXIT_FAILURE;
    }
    
    Cidade * cidade_encontrada = busca_cidade_por_codigo(hash_cidades, codigo_ibge);

    if (cidade_encontrada != NULL) {
        printf("Cidade encontrada:\n\n");
        printf("codigo_ibge: %d\n", cidade_encontrada->codigo_ibge);
        printf("nome: %s\n", cidade_encontrada->nome);
        printf("latitude: %lf\n", cidade_encontrada->latitude);
        printf("longitude: %lf\n", cidade_encontrada->longitude);
        printf("capital: %d\n", cidade_encontrada->capital);
        printf("codigo_uf: %d\n", cidade_encontrada->codigo_uf);
        printf("siafi_id: %d\n", cidade_encontrada->siafi_id);
        printf("ddd: %d\n", cidade_encontrada->ddd);
        printf("fuso_horario: %s\n", cidade_encontrada->fuso_horario);
    } else {
        printf("Cidade com o codigo_ibge: %d nao foi encontrada\n", codigo_ibge);
    }

    hash_apaga(&hash_cidades);

    return EXIT_SUCCESS;
}
