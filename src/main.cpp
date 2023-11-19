#include "knapsack.h"
#include "ga.h"
#include "operators\operators.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>

using namespace std;

/**=======================================================================**/
int main(){

    vector<t_hard_constraint> hard_constraint;
    vector<t_soft_constraint> soft_constraint;
    vector<void*> args;
    Knapsack k = Knapsack();
	
    /* ====================================== */
    hard_constraint.push_back(&overweight);
    hard_constraint.push_back(&overloaded);
    hard_constraint.push_back(&empty);

    soft_constraint.push_back(&weightvalue);
    soft_constraint.push_back(&volumevalue);
	
    args.push_back((void*)k.get_items());

    /* ====================================== */
    cout << "===========Inicializando===========\n";
    GA ga(&uniformCrossover, &invert, &roulette, hard_constraint, soft_constraint, args);


    do{
        cout << "\r[" << ga.curr_epoch << "/" << IT << "]";
    }while(ga.run(MULTI_THREAD));

    cout << "\n";
    

    double total_kg = 0;
    for(int i = 0; i < CHROMO_SIZE; i++){
        if((*ga.best_chromo[i]))
            (total_kg += (k[i]->param[I_WEIGHT]));
    }

    double total_vol = 0;
    for(int i = 0; i < CHROMO_SIZE; i++){
        if((*ga.best_chromo[i]))
            (total_vol += (k[i]->param[I_VOLUME]));
    }

    double total_val = 0;
    for(int i = 0; i < CHROMO_SIZE; i++){
        if ((*ga.best_chromo[i]))
            (total_val += (k[i]->param[I_VALUE]));
    }

    cout << "Melhor fitness[" << ga.curr_epoch << "]: " << ga.best_chromo.fitness_raw << "\n";
    for(int i = 0; i < CHROMO_SIZE; i++){
        if((*ga.best_chromo[i]))
        cout << "ID: " << (i)
             << "\tR$: " << (k[i]->param[I_VALUE]) << "\t"
             << (k[i]->param[I_WEIGHT]) << "kg\t"
             << (k[i]->param[I_VOLUME]) << "m3\n";
    }
    cout << "==============================================================\n";
    cout << "Total: \tR$" << total_val << "\t\t" << total_kg << "kg\t" << total_vol << "m3\n";

	//====================================
	system("PAUSE");

    return 0;

}
