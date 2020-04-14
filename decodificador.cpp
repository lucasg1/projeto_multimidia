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

int gerabit(FILE *entrada, int posicao, byte *aux){
    (posicao % 8 == 0) ? fread(aux, 1, 1, entrada) : NULL == NULL ;
    return !!((*aux) & (1 << (posicao % 8)));
}


void decodificacaoHuffman(const char *arqent, const char *arqsai){
    unsigned listaPixels[N] = {0};
    FILE *entrada = fopen(arqent, "rb");
    if(!entrada){
        printf("Arquivo nao encontrado\n");
        return;
    }
    FILE *saida = fopen(arqsai, "wb");
    if(!saida){
        printf("Arquivo nao encontrado\n");
        return;
    }
    fread(listaPixels, N, sizeof(listaPixels[0]), entrada);
    noArv *raiz = ArvHuffman(listaPixels);
    unsigned tamanho;
    fread(&tamanho, 1, sizeof(tamanho), entrada);
    unsigned posicao = 0;
    byte aux = 0;
    int cont = 1;
    int COL=1, LINE=1, MAXVAL;
    int COLnow = 0, LINEnow=0;
    fprintf(saida, "P2\n");
    noArv *nodeAtual = raiz;
    while ( nodeAtual->esquerda || nodeAtual->direita ){
            nodeAtual = gerabit(entrada, posicao++, &aux) ? nodeAtual->direita : nodeAtual->esquerda;
    }
    COL = nodeAtual->pixel;
    fprintf(saida, "%d ", COL);
    nodeAtual = raiz;
    while ( nodeAtual->esquerda || nodeAtual->direita ){
            nodeAtual = gerabit(entrada, posicao++, &aux) ? nodeAtual->direita : nodeAtual->esquerda;
    }
    LINE = nodeAtual->pixel;
    fprintf(saida, "%d ", LINE);
    nodeAtual = raiz;
    while ( nodeAtual->esquerda || nodeAtual->direita ){
            nodeAtual = gerabit(entrada, posicao++, &aux) ? nodeAtual->direita : nodeAtual->esquerda;
    }
    MAXVAL = nodeAtual->pixel;
    fprintf(saida, "%d\n", MAXVAL);
    LINEnow++;

    while (posicao < tamanho){
        noArv *nodeAtual = raiz;
        while ( nodeAtual->esquerda || nodeAtual->direita ){
            nodeAtual = gerabit(entrada, posicao++, &aux) ? nodeAtual->direita : nodeAtual->esquerda;
        }
        COLnow++;
        if(COLnow==COL){
            fprintf(saida, "%d\n", nodeAtual->pixel);
            COLnow=0;
            LINEnow++;
        }else{
        fprintf(saida, "%d ", nodeAtual->pixel);
        }
    }
    freeArvHuffman(raiz);
    printf("Arquivo de entrada: %s\nArquivo de saida: %s\n", arqent, arqsai);
    fclose(saida);
    fclose(entrada);
}

int main(){
    decodificacaoHuffman("lena_ascii.huff", "lena_ascii.huff.pgm");
    return 0;
}