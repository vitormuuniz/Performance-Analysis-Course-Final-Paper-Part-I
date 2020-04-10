/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: 2016.1.08.022
 *
 * Created on 26 de Abril de 2019, 16:28
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "fila.h"

typedef struct little_ {
    double tempo_anterior;
    double soma_areas;
    double qtd_pacotes;

} little;

/**
 * 
 * 
 * @return numero aleatorio entre (0,1]
 */
double aleatorio() {
    double u;
    u = rand() % RAND_MAX;
    u = u / RAND_MAX;
    u = 1.0 - u;

    return (u);
}

/**
 * 
 * @param parametro_l parametro da exponencial
 * @return intervalo de tempo, com media tendendo ao intervalo
 * informado pelo usuario
 */
double chegada_pct(double parametro_l) {
    return (-1.0 / parametro_l) * log(aleatorio());
}

/**
 * 
 * @return tamanho do pacote que acabou de chegar,
 * seguindo a proporcao aproximada de:
 * 50% = 550 Bytes
 * 40% = 40 Bytes 
 * 10% = 1500 Bytes
 */
double gera_tam_pct() {
    double a = aleatorio();

    // multiplica por 8 para passar para bits e depois 
    // divide por 1 milhao para passar para megabits (Mb)
    if (a <= 0.5) {
        return ((550.0 * 8.0) / (1000000.0));
    } else if (a <= 0.9) {
        return ((40.0 * 8.0) / (1000000.0));
    } else {
        return ((1500.0 * 8.0) / (1000000.0));
    }
}

/**
 * 
 * @param a
 * @param b
 * @return menor valor entre a e b
 */
double minimo(double a, double b) {
    if (a <= b) return a;
    return b;
}

/**
 * 
 * @param l variavel de little que será iniciada
 */
void inicia_little(little * l) {
    l->qtd_pacotes = 0.0;
    l->soma_areas = 0.0;
    l->tempo_anterior = 0.0;
}

/*
 * 
 */
int main() {
    pacote ** inicio = malloc(sizeof (pacote *));
    pacote ** fim = malloc(sizeof (pacote *));
    *inicio = NULL;
    *fim = NULL;

    //variavel para E[N]
    little en;

    //variavel para E[W] chegada
    little ew_chegada;

    //variavel para E[W] saida
    little ew_saida;

    inicia_little(&en);
    inicia_little(&ew_chegada);
    inicia_little(&ew_saida);

    //iniciando a semente
    //para a geração dos números
    //pseudoaleatórios
    int semente = time(NULL);
    printf("Semente: %d\n", semente);
    srand(semente);

    //tempo atual
    double tempo = 0.0;

    //tempo total
    double tempo_total;
    printf("Informe o tempo total de simulacao: ");
    scanf("%lF", &tempo_total);

    //intervalo medio entre chegadas de pacotes normais
    double intervalo;
    printf("Informe o intervalo medio de tempo(segundos) entre pacotes NORMAIS: ");
    scanf("%lF", &intervalo);

    //ajustando parametro para a exponencial
    intervalo = 1.0 / intervalo;

    //tamanho do pacote gerado
    double tam_pct;

    //tamanho do pacote cbr 
    double tam_pct_cbr = (1200.0 * 8.0) / 1000000.0;

    //tamanho do link de saida do roteador
    double link;
    printf("Informe o tamanho do link (Mbps): ");
    scanf("%lF", &link);

    //tempo de chegada do proximo pacote ao sistema
    double chegada_prox_pct = chegada_pct(intervalo);

    //tempo de chegada do proximo pacote
    //cbr ao sistema, com intervalo de 20ms entre os pacotes
    double chegada_prox_pct_cbr = 0.02;

    //tempo de saida do pacote que esta
    //sendo atendido atualmente
    double saida_pct_atendimento;

    //variavel para o calculo da ocupacao do roteador
    double ocupacao = 0.0;

    while (tempo <= tempo_total) {

        //se roteador vazio avanco o
        //tempo de chegada para o tempo
        //do proximo pacote
        if (*inicio == NULL) tempo = minimo(chegada_prox_pct_cbr, chegada_prox_pct);
        else {
            //há fila!

            //tempo recebe o q ue acontecer primeiro:
            //chegada de um novo pacote ou
            //saida de um pacote na fila
            tempo = minimo(minimo(chegada_prox_pct_cbr, chegada_prox_pct), saida_pct_atendimento);
        }


        if (tempo == chegada_prox_pct) { //chegada de pacote normal

            //            printf("Chegada de pacote WEB no tempo: %lF\n", tempo);

            //descobrir o tamanho do pacote
            tam_pct = gera_tam_pct();

            //se roteador livre
            if (*inicio == NULL) {

                //gerando o tempo de atendimento
                saida_pct_atendimento = tempo + tam_pct / link;

                //calculo da ocupacao
                ocupacao += saida_pct_atendimento - tempo;
            }

            //inserir na fila
            inserir(inicio, fim, tam_pct);

            //gerar o tempo de chegada do proximo pacote
            chegada_prox_pct = tempo + chegada_pct(intervalo);

            //calculo little
            en.soma_areas += en.qtd_pacotes * (tempo - en.tempo_anterior);
            en.qtd_pacotes++;
            en.tempo_anterior = tempo;

            //calculo little E[W] chegada
            ew_chegada.soma_areas += ew_chegada.qtd_pacotes * (tempo - ew_chegada.tempo_anterior);
            ew_chegada.qtd_pacotes++;
            ew_chegada.tempo_anterior = tempo;
        } else if (tempo == chegada_prox_pct_cbr) { //chegada de pacote CBR

            //            printf("Chegada de pacote CBR no tempo: %lF\n", tempo);

            //tamanho pacote cbr = 1200 Bytes
            tam_pct = tam_pct_cbr;

            //se roteador livre
            if (*inicio == NULL) {

                //gerando o tempo de atendimento
                saida_pct_atendimento = tempo + tam_pct / link;

                //calculo da ocupacao
                ocupacao += saida_pct_atendimento - tempo;
            }

            //inserir na fila
            inserir(inicio, fim, tam_pct);

            //gerar o tempo de chegada do proximo pacote
            chegada_prox_pct_cbr += 0.02;

            //calculo little E[N] 
            en.soma_areas += en.qtd_pacotes * (tempo - en.tempo_anterior);
            en.qtd_pacotes++;
            en.tempo_anterior = tempo;

            //calculo little E[W] chegada
            ew_chegada.soma_areas += ew_chegada.qtd_pacotes * (tempo - ew_chegada.tempo_anterior);
            ew_chegada.qtd_pacotes++;
            ew_chegada.tempo_anterior = tempo;
        } else { //saida de pacote

            //remover pacote da fila
            remover(inicio);

            if (*inicio != NULL) {

                //obtem o tamanho do pacote
                tam_pct = (*inicio)->tamanho;

                //                if (tam_pct == tam_pct_cbr) printf("Saida de pacote CBR\n");
                //                else printf("Saida de pacote WEB no tempo: %lF\n", tempo);

                //gerando o tempo de atendimento
                saida_pct_atendimento = tempo + tam_pct / link;

                //calculo da ocupacao
                ocupacao += saida_pct_atendimento - tempo;

            }
            //calculo little E[N]
            en.soma_areas += en.qtd_pacotes * (tempo - en.tempo_anterior);
            en.qtd_pacotes--;
            en.tempo_anterior = tempo;

            //calculo little E[W] saida
            ew_saida.soma_areas += ew_saida.qtd_pacotes * (tempo - ew_saida.tempo_anterior);
            ew_saida.qtd_pacotes++;
            ew_saida.tempo_anterior = tempo;
        }

        //printf("=============================================\n");
        //        getchar();
    }

    ew_saida.soma_areas += ew_saida.qtd_pacotes * (tempo - ew_saida.tempo_anterior);
    ew_chegada.soma_areas += ew_chegada.qtd_pacotes * (tempo - ew_chegada.tempo_anterior);

    double en_final = en.soma_areas / tempo;
    double ew = ew_chegada.soma_areas - ew_saida.soma_areas;
    ew = ew / ew_chegada.qtd_pacotes;

    double lambda = ew_chegada.qtd_pacotes / tempo;
    
    printf("Ocupacao: %lF\n", ocupacao / tempo);
    printf("\n=======Little=======\n");
    printf("E[N] = %lF\n", en_final);
    printf("E[W] = %lF\n", ew);
    printf("Lambda = %lF\n", lambda);
    printf("Validacao Little: %lF\n", en_final - (lambda * ew));

    return (EXIT_SUCCESS);
}

