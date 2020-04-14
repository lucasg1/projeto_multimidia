#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <iostream>

using namespace std;

#define N 65536

typedef unsigned char byte;

typedef struct noArv{
    int frequencia;
    int pixel;
    struct noArv *esquerda;
    struct noArv *direita;
}noArvore;

typedef struct noLis{
    noArv *n;
    struct noLis *proximo;
}noLis;

typedef struct lista{
    noLis *chave;
    int elementos;
}lista;

noLis *novoNoLis(noArv *nArv){
    noLis *novo;
    novo= new noLis[sizeof(*novo)];
    if(novo==NULL) return NULL;
    novo->n= nArv;
    novo->proximo= NULL;
    return novo;
}

noArv *novoNoArv(int pixel, int f, noArv *esq, noArv *dir){
    noArv *novo;
    novo= new noArv[sizeof(*novo)];
    if(novo==NULL) return NULL;
    novo->pixel= pixel;
    novo->frequencia= f;
    novo->esquerda= esq;
    novo->direita= dir;
    return novo;
}

void insereLis(noLis *n, lista *l){
    if (!l->chave){
        l->chave = n;
    }
    else if (n->n->frequencia < l->chave->n->frequencia){
        n->proximo = l->chave;
        l->chave = n;
    }
    else{
        noLis *aux= l->chave->proximo;
        noLis *aux2= l->chave;
        while (aux && aux->n->frequencia <= n->n->frequencia){
            aux2= aux;
            aux= aux2->proximo;
        }
        aux2->proximo = n;
        n->proximo = aux;
    }
    l->elementos++;
}

noArv *popMinLis(lista *l){
    noLis *aux = l->chave;
    noArv *aux2 = aux->n;
    l->chave = aux->proximo;
    free(aux);
    aux = NULL;
    l->elementos--;
    return aux2;
}

void getfrequenciapixel(FILE *entrada, unsigned int *listaPixels){
    int pixel;
    while (fscanf(entrada, "%d", &pixel)!=EOF){
        listaPixels[pixel]++;
    }
    rewind(entrada);
}

bool pegaCodigo(noArv *n, int pixel, char *buffer, int tamanho){
    if (!(n->esquerda || n->direita) && n->pixel == pixel){
        buffer[tamanho] = '\0';
        return true;
    }
    else{
        bool encontrado = false;
        if (n->esquerda){
            buffer[tamanho] = '0';
            encontrado = pegaCodigo(n->esquerda, pixel, buffer, tamanho + 1);
        }
        if (!encontrado && n->direita){
            buffer[tamanho] = '1';
            encontrado = pegaCodigo(n->direita, pixel, buffer, tamanho + 1);
        }
        if (!encontrado){
            buffer[tamanho] = '\0';
        }
        return encontrado;
    }

}

void transverse(noArv *node, int &frequenciaTotal, int &somaTotal, int bits){
    if(node == NULL) return;
    if(node->pixel >= 0){
        somaTotal += bits*(node->frequencia);
        frequenciaTotal += node->frequencia;
    }
    transverse(node->esquerda, frequenciaTotal, somaTotal, bits+1);
    transverse(node->direita, frequenciaTotal, somaTotal, bits+1);

    return;
}

float getTamanhoMedio(noArv *root){
    int frequenciaTotal = 0;
    int somaTotal = 0;
    int bits = 0;
    noArv* node = root;

    transverse(node, frequenciaTotal, somaTotal, bits);
    float tamanhoMedio = somaTotal*1.0/frequenciaTotal;
    return tamanhoMedio;
}

noArv *ArvHuffman(unsigned *listaPixels){
    lista l ={NULL, 0};
    for (int i = 0; i < N; i++){
        if (listaPixels[i]){
            insereLis(novoNoLis(novoNoArv(i, listaPixels[i], NULL, NULL)), &l);
        }
    }
    while (l.elementos > 1) {
        noArv *nodeEsquerdo = popMinLis(&l);
        noArv *nodeDireito = popMinLis(&l);
        noArv *soma = novoNoArv(-1,nodeEsquerdo->frequencia + nodeDireito->frequencia, nodeEsquerdo, nodeDireito);
        insereLis(novoNoLis(soma), &l);
    }
    return popMinLis(&l);
}

void freeArvHuffman(noArv *n){
    if (!n) return;
    else{
        noArv *esquerda = n->esquerda;
        noArv *direita = n->direita;
        free(n);
        freeArvHuffman(esquerda);
        freeArvHuffman(direita);
    }
}

int calculaLinPulo(FILE *entrada){
    byte aux;
    int pulo = 0;
    fread(&aux, 1, 1, entrada);
    while(aux=='#' || aux=='P'){
        pulo++;
        while(aux!=10) fread(&aux, 1, 1, entrada);
        fread(&aux, 1, 1, entrada);
    }
    rewind(entrada);
    return pulo;
}

void pularlinha(FILE *entrada, int pulo){
    byte aux;
    for(int i=0; i<pulo;i++){
        fread(&aux, 1, 1, entrada);
        while(aux!=10) fread(&aux, 1, 1, entrada);
    }
    return;
}

void codificaoHuffman(const char *arqent, const char *arqsai){
    unsigned listaPixels[N] = {0};
    FILE *saida = fopen(arqsai, "wb");
    if(!saida){
        printf("Arquivo nao encontrado\n");
        return;
    }
    FILE *entrada = fopen(arqent, "rb");
    if(!entrada){
        printf("Arquivo nao encontrado\n");
        return;
    }
    int pulo = calculaLinPulo(entrada);
    pularlinha(entrada, pulo);
    getfrequenciapixel(entrada, listaPixels);
    pularlinha(entrada, pulo);

    noArv *raiz = ArvHuffman(listaPixels);
    float comprimentoMedio = getTamanhoMedio(raiz);
    printf("O comprimento medio do codigo binario eh %f\n", comprimentoMedio);

    fwrite(listaPixels, N, sizeof(listaPixels[0]), saida);
    fseek(saida, sizeof(unsigned int), SEEK_CUR);
    int pixel;
    unsigned tamanho = 0;
    byte aux = 0;
    while (fscanf(entrada, "%d", &pixel)!=EOF){
        char buffer[1024] = {0};
        pegaCodigo(raiz, pixel, buffer, 0);
        for (char *i = buffer; *i; i++){
            if (*i == '1'){
                aux = aux | (1 << (tamanho % 8));
            }
            tamanho++;
            if (tamanho % 8 == 0){
                fwrite(&aux, 1, 1, saida);
                aux = 0;
            }
        }
    }
    fwrite(&aux, 1, 1, saida);
    fseek(saida, N * sizeof(unsigned int), SEEK_SET);
    fwrite(&tamanho, 1, sizeof(unsigned), saida);
    freeArvHuffman(raiz);
    printf("Arquivo de entrada: %s\nArquivo de saida: %s\n", arqent, arqsai);
    fclose(entrada);
    fclose(saida);

}

int main(){
    codificaoHuffman("lena_ascii.pgm", "lena_ascii.huff");
    return 0;
}
