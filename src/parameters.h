#pragma once

/* =====================================
[Nota 1  -   100 = 100%]
[Nota 2  -   1 = 100% ]
   ===================================== */

/*===INCLUDES=DO=PROBLEMA===*/
#include "knapsack.h"
/*==========================*/

#define BINCOD
//#define QUACOD

#ifdef BINCOD
/*====CODIFICACAO=BINARIA====*/
/* Tipo da estrutura de informacoes da fitness*/
typedef bool gene;
/* Porcentagem de genes populacao inicial (Nota 1)
   https://www.scss.tcd.ie/publications/tech-reports/reports.97/TCD-CS-1997-08.pdf */
#define PCT_GEN 7
/*===========================*/
#endif

#ifdef QUACOD
/*=CODIFICACAO=QUANTITATIVA=*/
/*Tipo do gene no problema*/
typedef nota gene;
/*ID da representacao de cromossomo nulo*/
#define EMPTY_ITEM_ID 100
/* Quantidade de valores distintos possiveis representaveis */
#define MAX_ITEM_ID 10
/*===========================*/
#endif

/*=======CONFIGURACOES=======*/
/*Quantidade de genes em um cromossomo*/
#define CHROMO_SIZE MAX_ITEM
/* Elitismo */
#define ELITISM 1
/*Quantidade de cromossomos inicial*/
#define N_CHROMO_I 150
/* Taxa de Mutacao (Nota 1)*/
#define MUTATION_RATE 0.1
/* Taxa de Crossover (Nota 1)*/
#define CROSSOVER_RATE 50
/* Menor Pior valor, caso o algoritmo esteja tentando convergir para cá, mudará de direcao */
#define WORST_VALUE(a) { return a[a.size()-1].fitness_raw; }
/* Normalizacao dos Valores */
#define NORMALIZE
/* Porcentagem de 'coroas' quando a moeda for jogada (Nota 1)*/
#define TAILS 0.5
/*----Condicoes de parada----*/
/* Numero limite de iteracoes */
#define IT 1000
/* Limite de convergencia
 (quantos % da populacao deve possuir
 o mesmo valor para ser considerada estavel)
 (Nota 2)*/
//#define CONVERGENCE 0.90
/* O algoritmo deve parar apenas quando alcancar um resultado bom */
#ifndef CONVERGENCE
#define DO_UNTIL_FIND
#endif
/*===========================*/
