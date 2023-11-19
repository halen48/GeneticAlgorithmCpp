#ifndef OPERATORS_H_
#define OPERATORS_H_

#include <vector>
#include <ctime>
#include <chrono>
#include <random>
#include <algorithm>
#include "../ga.h"

using namespace std;


/**=======================================================================**/

/**
    Todas as funcoes aqui seguem o
    padrao dado no comeco de cada
    secao
**/

/***************************************************************************/

/**
    int * selection(vector<chromosome>);
**/

/* Selecao Aleatoria */
int * random(vector<chromosome>);
/* Selecao Roleta */
int * roulette(vector<chromosome>);

/***************************************************************************/

/**
    chromosome * crossingOver(chromosome, chromosome, int);
**/

/* Cruzamento uniforme */
chromosome * uniformCrossover(chromosome, chromosome, int);
/* Cruzamento em n-pontos */
chromosome * pointCrossover(chromosome, chromosome, int);
/* Cruzamento em n-pontos aleatorios */
chromosome * rndpointCrossover(chromosome, chromosome, int);
#ifdef BINCOD
/* Cruzamento aritimetico */
chromosome * arithmeticCrossover(chromosome, chromosome, int);
#endif

/***************************************************************************/

/**
    void mutation(chromosome* , int);
**/
/* Mutacao embaralhar valores */
void scramble(chromosome *, int);
/* Mutacao permutacao de valores */
void swap_(chromosome *, int);
#ifdef BINCOD
/* Mutacao Inverter bit */
void invert(chromosome *, int);
#endif
#ifdef QUACOD
/*  Mutacao substituicao de valor */
void replace(chromosome * , int);
/*  Mutacao Insercao de valor */
void insert(chromosome * , int);
#endif
/***************************************************************************/

/**
    bool hard_constraint (chromosome);
**/

/***************************************************************************/

/**
    double soft_constraint (chromosome);
**/

/***************************************************************************/
/**
    double f (double x);
**/

/**=======================================================================**/
#endif // OPERATORS_H_
