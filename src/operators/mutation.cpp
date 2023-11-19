#include "operators.h"

/**
    void mutation(chromosome* , int);
**/

/* Mutacao permutacao de valores */
void swap_(chromosome * c, int index){

    uniform_int_distribution<int> roll (0, CHROMO_SIZE-1);

    swap( (*c[0][index]), (*c[0][roll(seed)]));
    c->mut = true;

}

/* Mutacao embaralhar valores */
void scramble(chromosome * c, int index){

    if(index+1 > CHROMO_SIZE-1) return;

    uniform_int_distribution<int> roll (index+1, CHROMO_SIZE-1);
    int max_range = roll(seed);

    for(int i = index; i < max_range; i++){
        uniform_int_distribution<int> index_s (i, max_range);
        swap( (*c[0][index_s(seed)]), (*c[0][index_s(seed)]) );
    }

}
#ifdef BINCOD
/* Mutacao Inverter bit */
void invert(chromosome * c, int index){
    (*c[0][index]) = !(*c[0][index]);
    c->mut = true;
}
#endif

#ifdef QUACOD
/*  Mutacao substituicao de valor
    Troca de um valor por outro valor aleatorio
    Fonte: HAUPT, Randy L. Partical Genetic Algorithms */
void replace(chromosome * c, int index){

    uniform_int_distribution<int> roll (0, MAX_ITEM_ID-1);

    (*c[0][index]) = roll(seed);
     c->mut = true;

}
/*  Mutacao Insercao de valor */

void insert(chromosome * c, int index){ //Testar

    uniform_int_distribution<int> roll (0, CHROMO_SIZE-1);

    int j = roll(seed),
        i = index+1;

    for(; j > i; j--)
        (*c[0][j]) = (*c[0][j-1]);

    swap( (*c[0][index]), (*c[0][j]) );

    c->mut = true;

}
#endif
