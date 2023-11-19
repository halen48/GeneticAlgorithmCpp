#include "operators.h"

/* Cruzamento uniforme */
chromosome * uniformCrossover(chromosome c1, chromosome c2, int nao_usado){

    static chromosome offspring[2];

    srand(time(NULL));

    bool g_swap;

    for(int i = 0; i < CHROMO_SIZE; i++){
        if(rand()%2){
            g_swap         =   (*c1[i]);
            (*c1[i])       =   (*c2[i]);
            (*c2[i])       =   g_swap;
        }
    }

    offspring[0] = c1;
    offspring[1] = c2;
    offspring[0].cross = true;
    offspring[1].cross = true;

    return offspring;
}

/* Cruzamento em n-pontos */

chromosome * pointCrossover(chromosome c1, chromosome c2, int index){

    static chromosome offspring[2];

    for(int i = 0; i < index; i++){
        (*offspring[0][i]) = (*c1[i]);
        (*offspring[1][i]) = (*c2[i]);
    }

    for(int i = index; i < CHROMO_SIZE; i++){
        (*offspring[0][i]) = (*c2[i]);
        (*offspring[1][i]) = (*c1[i]);
    }

    offspring[0].cross = true;
    offspring[1].cross = true;
    return offspring;

}

/* Cruzamento em n-pontos aleatorios */

chromosome * rndpointCrossover(chromosome c1, chromosome c2, int qt_cortes){

    srand(time(NULL));
    int index;

    static chromosome offspring[2];

    while(qt_cortes-- > 0 ){

        index = rand()%CHROMO_SIZE;

        for(int i = 0; i < index; i++){
            (*offspring[0][i]) = (*c1[i]);
            (*offspring[1][i]) = (*c2[i]);
        }

        for(int i = index; i < CHROMO_SIZE; i++){
            (*offspring[0][i]) = (*c2[i]);
            (*offspring[1][i]) = (*c1[i]);
        }

    }

    offspring[0].cross = true;
    offspring[1].cross = true;
    return offspring;

}

#ifdef BINCOD

/* Cruzamento aritimetico
   http://www.obitko.com/tutorials/genetic-algorithms/crossover-mutation.php */

chromosome * arithmeticCrossover(chromosome c1, chromosome c2, int nao_usado){

    static chromosome offspring[2];

    for(int i = 0; i < CHROMO_SIZE; i++)
        (*offspring[0][i]) = ( (*c1[i]) && (*c2[i]) );

    offspring[0].cross = true;
    offspring[1] = offspring[0];

    return offspring;
}

#endif
