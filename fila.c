/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FILA.c
 * Author: 2016.1.08.022
 *
 * Created on 9 de Maio de 2019, 16:35
 */

#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

pacote * aloca_pct() {
    return malloc(sizeof (pacote));

}

/**
 * 
 * @param inicio - aponta o primeiro da fila
 * @param fim - aponta o ultimo da fila
 * @param tamanho - tamanho do pacote a ser inserido
 * @return 1 caso insira, 0 caso erro
 */
int inserir(pacote ** inicio, pacote ** fim, double tamanho) {

    //se fila vazia
    if ((*inicio) == NULL) { //primeira insercao
        (*inicio) = aloca_pct();

        if (*inicio == NULL) return 0;

        (*fim) = (*inicio);
        (*inicio)->tamanho = tamanho;
        (*inicio)->prox = NULL;
        return 1;
    } else {
        pacote *tmp = aloca_pct();
        if (tmp == NULL) return 0;

        tmp->tamanho = tamanho;
        tmp->prox = NULL;

        (*fim)->prox = tmp;
        (*fim) = tmp;
        return 1;
    }
}

double remover(pacote ** inicio) {
    if (*inicio == NULL) return 0;
    else {
        pacote * tmp = *inicio;
        double tamanho = (*inicio)->tamanho; //*(*(tmp))->tamanho
        
        (*inicio) = (*inicio)->prox;
        free(tmp);
        
        return tamanho;
    }
}