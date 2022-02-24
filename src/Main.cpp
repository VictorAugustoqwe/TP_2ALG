#include <iostream>
#include <cmath>

#include "Loja.h"

using namespace std;

//Ordena as partições para ordenar o vetor
void Particao(int esq, int dir, int *pontEsq, int *pontDir, double *vetor){
    double pivo, aux;
    *pontEsq = esq;
    *pontDir = dir;
    pivo = vetor[(*pontEsq + *pontEsq)/2];
    do{
        while(pivo < vetor[*pontEsq]) (*pontEsq)++;
        while(pivo > vetor[*pontDir]) (*pontDir)--;
        if(*pontEsq <= *pontDir){
            aux = vetor[*pontEsq];
            vetor[*pontEsq] = vetor[*pontDir];
            vetor[*pontDir] = aux;
            (*pontEsq)++;
            (*pontDir)--;
        }
    } while(*pontEsq <= *pontDir);
}

//Particiona o vetor recursivamente para ordená-lo 
void Ordena(int esq, int dir, double *vetor){
    int pontEsq;
    int pontDir;
    Particao(esq, dir, &pontEsq, &pontDir, vetor);
    if(esq < pontDir) Ordena(esq, pontDir, vetor);
    if(pontEsq < dir) Ordena(pontEsq, dir, vetor);
}

//Realiza uma ordenação decrescente usando o QuickSort
void QuickSort(double *vetor, int n){
    Ordena(0, n-1, vetor);
}

//Calcula a distância cartesiana entre 2 pontos (x1,y1) e (x2,y2)
double distanciaCartesiana(int x1, int y1, int x2, int y2){
    return(sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2)));
}

//Reduz a distância de um vértice específico ao set quando ele é atualizado, atualizando o heap
void diminuiDistVertice(int *heap, int *pos, double *distanciaAtual, int tamanhoHeap, double distanciaNova, int indiceAlterado){
    int indiceAux, indice;
    if(distanciaNova > distanciaAtual[heap[indiceAlterado]]){
        cout << "Erro aqui";
    }
    indice = indiceAlterado;
    distanciaAtual[heap[indice]] = distanciaNova;
    while(indice > 1 && (distanciaAtual[heap[indice / 2]] > distanciaAtual[heap[indice]])){
        indiceAux = heap[indice / 2];
        heap[indice / 2] = heap[indice];
        pos[heap[indice]] = indice / 2;
        heap[indice] = indiceAux;
        pos[indiceAux] = indice;
        indice = indice / 2;
    }
}

//Refaz o heap para manter a ordenação parcial
void refazHeap(int *heap, int *pos, double *distanciaAtual, int tamanhoHeap){
    int pai, filho, verticePai;
    pai = 1;
    filho = pai*2;
    verticePai = heap[pai];
    while(filho <= tamanhoHeap){
        if(filho < tamanhoHeap)
            if(distanciaAtual[heap[filho]] > distanciaAtual[heap[filho+1]]) filho++;
        if(distanciaAtual[verticePai] <= distanciaAtual[heap[filho]]) break;
        heap[pai] = heap[filho];
        pos[heap[filho]] = pai;
        pai = filho;
        filho = pai*2;       
    }
    heap[pai] = verticePai;
    pos[verticePai] = pai;
}

//Retira e retorna o primeiro elemento do heap mantendo os outros arrays e refaz o heap
int retiraMenorIndiceHeap(int *heap, int *pos, double *distanciaAtual, int &tamanhoHeap){
    if(tamanhoHeap < 1){
        cout << "Heap esta vazio";
        return(-1);
    } 
    int indiceVertice = heap[1];
    heap[1] = heap[tamanhoHeap];
    pos[heap[tamanhoHeap]] = 1;
    tamanhoHeap = tamanhoHeap - 1;
    refazHeap(heap, pos, distanciaAtual, tamanhoHeap);
    return(indiceVertice);
}

//Lê entradas e abre o arquivo executando o algoritmo.
int main(){
    //Declara variáveis
    int nLojas, kmMoto, nDrones;
    int i, j, tamanhoHeap, verticeAtual, percorreAdjacencia;
    double x, y, distanciaNova;
    double **matrizAdjacencia, *distanciaAtual, *distanciasEntreLojas, precoMoto, precoCaminhao, custoMoto, custoCaminhao;
    int *posPrioridade, *antecessor, *verticesNaoExplorados, *verticePrioridade;
    Loja *lojas;

    //Lê entrada
    cin >> nLojas;
    cin >> kmMoto;    
    cin >> nDrones;
    cin >> custoMoto;
    cin >> custoCaminhao;


    //Aloca mememória necessária
    antecessor = new int[nLojas];
    verticePrioridade = new int[nLojas+1];
    posPrioridade = new int[nLojas];
    distanciaAtual = new double[nLojas];
    matrizAdjacencia = new double*[nLojas];
    verticesNaoExplorados = new int[nLojas];
    distanciasEntreLojas = new double[nLojas-1];
    lojas = new Loja[nLojas];

    //Lê posição das lojas
    for(i = 0; i < nLojas; i++){
        cin >> x;
        cin >> y;
        lojas[i].posX = x;
        lojas[i].posY = y;
    }

    //Inicia as variáveis do algoritmo 
    for(i = 0; i < nLojas; i++){
        verticePrioridade[i+1] = i;
        posPrioridade[i] = i + 1;
        antecessor[i] = -1;
        distanciaAtual[i] = 100000;
        matrizAdjacencia[i] = new double[nLojas];
        verticesNaoExplorados[i] = 1;
    }
    tamanhoHeap = nLojas;
    verticePrioridade[1] = 0;
    distanciaAtual[0] = 0;

    //Calcula e armazena a matriz de adjacência do grafo utilizado
    for(i = 0; i < nLojas; i++){
        for(j = 0; j < nLojas; j++){
            matrizAdjacencia[i][j] = distanciaCartesiana(lojas[i].posX, lojas[i].posY, lojas[j].posX, lojas[j].posY);
        }
    }

    //Enquanto houver elementos no heap retira o primeiro e verifica seus adjacentes armazenando a atual distância, com base no algoritmo de prim
    while(tamanhoHeap >= 1){
        verticeAtual = retiraMenorIndiceHeap(verticePrioridade, posPrioridade, distanciaAtual, tamanhoHeap);
        verticesNaoExplorados[verticeAtual] = 0;
        percorreAdjacencia = 0;
        while(percorreAdjacencia < nLojas){
            distanciaNova = matrizAdjacencia[verticeAtual][percorreAdjacencia];
            if(distanciaNova < distanciaAtual[percorreAdjacencia] && verticeAtual != percorreAdjacencia && verticesNaoExplorados[percorreAdjacencia]){
                diminuiDistVertice(verticePrioridade, posPrioridade, distanciaAtual, tamanhoHeap, distanciaNova, posPrioridade[percorreAdjacencia]);                
                distanciaAtual[percorreAdjacencia] = distanciaNova;
                antecessor[percorreAdjacencia] = verticeAtual;
            }
            percorreAdjacencia++;
        }
    }

    //Armazena as distâncias da árvore geradora mínima
    for(i = 1; i < nLojas; i++){
        distanciasEntreLojas[i-1] = matrizAdjacencia[antecessor[i]][i];
    }

    //Ordena o vetor de distâncias 
    QuickSort(distanciasEntreLojas, nLojas-1);
    
    //Implementa lógica para os drones estarem nas maiores distâncias e calcula o preço a ser gasto de moto ou caminhão
    precoMoto = 0.0;
    precoCaminhao = 0.0;
    for(i = nDrones-1; i < nLojas -1; i++){
        if(i >= 0){
            if(distanciasEntreLojas[i] <= kmMoto ){
                precoMoto = precoMoto + distanciasEntreLojas[i] * custoMoto;
            }
            else{
                precoCaminhao = precoCaminhao + distanciasEntreLojas[i] * custoCaminhao;
            }
        }
    }
      
    //Imprime a saída como desejado
    cout.setf(ios::fixed,ios::floatfield);
    cout.precision(3);
    cout << precoMoto << " " << precoCaminhao;

    //Desaloca a memória alocada dinâmicamente
    for(i = 0; i < nLojas; i++){
        delete[] matrizAdjacencia[i];
    }
    delete[] matrizAdjacencia;
    delete[] antecessor;
    delete[] verticesNaoExplorados;
    delete[] posPrioridade;
    delete[] distanciaAtual;
    delete[] verticePrioridade;
    delete[] lojas;

    return 0;
}