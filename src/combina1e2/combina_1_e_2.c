#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#define R 6371
#include <regex.h>
#define SEED    0x12345678
#include "../../include/consulta_cidade.h"
#include "../../include/vizinhos_proximos.h"
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
    char *key = malloc(strlen(c->nome) + 1);
    if (key == NULL) {
        printf("Erro ao alocar memória para a chave\n");
        exit(EXIT_FAILURE);
    }
    snprintf(key, strlen(c->nome) + 1, "%s", c->nome);
    return key;
}

int busca_cidade_por_nome(Thash hash_cidades, char nome[]) {
    Cidade *cidade_encontrada = (Cidade *)hash_busca(hash_cidades, nome);
    if (cidade_encontrada == NULL) return EXIT_FAILURE;

    int codigo = cidade_encontrada->codigo_ibge;
    return codigo;
}

Tnode *criar_no(Cidade *cidade) {
    Tnode *novo_no = (Tnode *)malloc(sizeof(Tnode));
    if (novo_no == NULL) {
        printf("Erro: falha ao alocar memória para o nó\n");
        return NULL;
    }

    novo_no->reg = cidade;
    novo_no->esq = NULL;
    novo_no->dir = NULL;

    return novo_no;
}

int inserir_na_kdtree(Tnode **arvore, void *reg, int nivel) {
    if (*arvore == NULL) {
        *arvore = criar_no(reg);
        if (*arvore == NULL) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    int dimensao = nivel % 2;

    double coord_nova = dimensao == 0 ? ((Cidade *)reg)->latitude : ((Cidade *)reg)->longitude;
    double coord_atual = dimensao == 0 ? ((Cidade *)(*arvore)->reg)->latitude : ((Cidade *)(*arvore)->reg)->longitude;

    if (coord_nova < coord_atual) {
        return inserir_na_kdtree(&((*arvore)->esq), reg, nivel + 1);
    } else {
        return inserir_na_kdtree(&((*arvore)->dir), reg, nivel + 1);
    }
}

Tnode *buscar_linear_codigo_ibge(Tnode *raiz, int codigo_ibge) {
    if (raiz == NULL) {
        return NULL;
    }

    if (((Cidade *)(raiz->reg))->codigo_ibge == codigo_ibge) {
        return raiz;
    }

    Tnode *resultado_esq = buscar_linear_codigo_ibge(raiz->esq, codigo_ibge);
    if (resultado_esq != NULL) {
        return resultado_esq;
    }

    Tnode *resultado_dir = buscar_linear_codigo_ibge(raiz->dir, codigo_ibge);
    if (resultado_dir != NULL) {
        return resultado_dir;
    }

    return NULL;
}

double graus_para_radianos(double graus) {
    return graus * M_PI / 180.00;
}

double calcula_distancia_haversine(double lat1, double lon1, double lat2, double lon2) {
    double d_lat = graus_para_radianos(lat2 - lat1);
    double d_lon = graus_para_radianos(lon2 - lon1);
    
    double a = sin(d_lat / 2) * sin(d_lat / 2) + 
               cos(graus_para_radianos(lat1)) * cos(graus_para_radianos(lat2)) * 
               sin(d_lon / 2) * sin(d_lon / 2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    double distancia = R * c;
    
    return distancia;
}

double calcula_distancia_cidade_a_b(void *a, void *b){
    Cidade *cidade_a = (Cidade *)a;
    Cidade *cidade_b = (Cidade *)b;

    double distancia = calcula_distancia_haversine(cidade_a->latitude, cidade_a->longitude, cidade_b->latitude, cidade_b->longitude);

    return distancia;
}

void encontrar_cidades_proximas_rec(Tnode *raiz, Cidade *cidade_ref, int n, Cidade **cidades_proximas, int *contador) {
    if (raiz == NULL)
        return;

    double distancia = calcula_distancia_cidade_a_b(cidade_ref, ((Cidade *)raiz->reg));

    int i;
    for (i = 0; i < n; i++) {
        if (cidades_proximas[i] == NULL || distancia < calcula_distancia_cidade_a_b(cidade_ref, cidades_proximas[i])) {
            for (int j = n - 1; j > i; j--) {
                cidades_proximas[j] = cidades_proximas[j - 1];
            }
            cidades_proximas[i] = (Cidade *)raiz->reg;
            break;
        }
    }

    encontrar_cidades_proximas_rec(raiz->esq, cidade_ref, n, cidades_proximas, contador);
    encontrar_cidades_proximas_rec(raiz->dir, cidade_ref, n, cidades_proximas, contador);
}

Cidade **encontrar_cidades_proximas(Tnode *raiz, Cidade *cidade_ref, int n) {
    Cidade **cidades_proximas = (Cidade **)malloc(n * sizeof(Cidade *));
    if (cidades_proximas == NULL) {
        printf("Erro: falha ao alocar memória para cidades próximas\n");
        return NULL;
    }

    for (int i = 0; i < n; i++)
        cidades_proximas[i] = NULL;

    int contador = 0;

    encontrar_cidades_proximas_rec(raiz, cidade_ref, n + 1, cidades_proximas, &contador);

    return cidades_proximas;
}

void imprimir_cidades_proximas(Tnode *raiz, Cidade *cidade_ref, int n) {
    Cidade **cidades_proximas = encontrar_cidades_proximas(raiz, cidade_ref, n + 1);

    if (cidades_proximas == NULL) {
        printf("Erro: falha ao encontrar cidades proximas\n");
        return;
    }

    for (int i = 1; i < n + 1 && cidades_proximas[i] != NULL; i++) {
        printf("Cidade %d:\n", i);
        printf("codigo_ibge: %d\n", cidades_proximas[i]->codigo_ibge);
        printf("nome: %s\n", cidades_proximas[i]->nome);
        printf("latitude: %lf\n", cidades_proximas[i]->latitude);
        printf("longitude: %lf\n", cidades_proximas[i]->longitude);
        printf("capital: %d\n", cidades_proximas[i]->capital);
        printf("codigo_uf: %d\n", cidades_proximas[i]->codigo_uf);
        printf("siafid_id: %d\n", cidades_proximas[i]->siafi_id);
        printf("ddd: %d\n", cidades_proximas[i]->ddd);
        printf("fuso_horario: %s\n", cidades_proximas[i]->fuso_horario);
        printf("\n");
    }

    free(cidades_proximas);
}

void destruir_arvore(Tnode *raiz) {
    if (raiz == NULL) {
        return;
    }

    destruir_arvore(raiz->esq);
    destruir_arvore(raiz->dir);

    free(raiz);
}

int main(int argc, char *argv[]){
    if (argc < 4) {
        printf("Obrigatorio ter 3 parametros\n");
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

    Tnode *arvore = NULL;

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

            if (inserir_na_kdtree(&arvore, cidade, 0) == EXIT_FAILURE) {
                printf("Erro ao tentar coloca o ponto na kdtree\n");
                return EXIT_FAILURE;
            }

            dados[0] = '\0';
            count = 0;
        }
    }

    fclose(cidadesJson);

    char nome_cidade[124];
    strcpy(nome_cidade, argv[2]);

    int codigo_ibge = busca_cidade_por_nome(hash_cidades, nome_cidade);

    if (codigo_ibge == EXIT_FAILURE) {
        printf("Cidade com o nome: %s nao foi encontrada\n", nome_cidade);
        return EXIT_FAILURE;
    }
    
    char *endptr;
    int numero_de_cidades = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0') {
        printf("O terceiro parametro tem que ser int\n");
        return EXIT_FAILURE;
    }

    Tnode *c = buscar_linear_codigo_ibge(arvore, codigo_ibge);
    Cidade *cidade_encontrada = NULL;
    if (c != NULL) {
        cidade_encontrada = ((Cidade *)c->reg);
        imprimir_cidades_proximas(arvore, cidade_encontrada, numero_de_cidades);
    } else {
        printf("Cidade com o codigo %d nao encontrada\n", codigo_ibge);
    }

    destruir_arvore(arvore);

    return EXIT_SUCCESS;
}
