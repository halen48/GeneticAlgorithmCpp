#include "operators.h"

/* Selecao Aleatoria */

int * random(vector<chromosome> epoch){

    uniform_int_distribution<int> r_index(0,epoch.size()-1);

    static int index[2];

    index[0] = r_index(seed);

    do{
        index[1] = r_index(seed);
    }while(index[1]==index[0]);

    return index;

}

/* Selecao Roleta */

int * roulette(vector<chromosome> epoch){

    static int r_index[2];

    int size = epoch.size();

    vector<float> roulette(size);

    double T = epoch[0].fitness_raw;

    /* Soma das fitness
    1. Sum the total expected value of individuals
       in the population. Call this sum T */
    for(int i = 1; i < size; i++){
        T += (epoch[i].fitness_raw);
    }

    /* Valores da roleta */
    //(valor/total)
    //cout << "=============<Roulette>===========\n";
    for(int i = 0; i < size; i++){
        roulette[i] = (epoch[i].fitness_raw);
        //cout << roulette[i] << " | ";
    }
    //cout << "\n";

    for(int index_b_fit = 0; index_b_fit < 2; index_b_fit++){   //faca duas vezes

            uniform_real_distribution<float> roll (0, T);
            float r;

        do{
            /* Choose a random real r between 0 and T. */
            r = roll(seed);  //escolha um ponto

            int index = 0;
            /* Loop through the individuals in the population,
            summing the expected values, until the sum is greater
            than or equal to r. The individual whose expected value
            puts the sum over this limit is the one selected. */
            for(float part = 0; part < r && index < roulette.size(); index++)
                part += roulette[index];

        r_index[index_b_fit] = (index-1);

        //se o mesmo pai foi selecionado na segunda vez, repete o processo

        }while(index_b_fit && r_index[1] == r_index[0]);

        //cout << "[" << r_index[index_b_fit] << "]: " << roulette[r_index[index_b_fit]] << "%\n";
    }


    return r_index;

}
