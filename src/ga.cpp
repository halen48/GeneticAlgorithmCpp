#include "ga.h"
#include <iostream>
#include <cstdlib>
#include <thread>

using namespace std;

/** ============================================== **/
//Para problemas de gene unico
chromosome::chromosome(bool big_chromo){

    if(!big_chromo){
		
        //Cria o cromossomo
        for(int index_gene = 0; index_gene < CHROMO_SIZE; index_gene++)
            (*(*this)[index_gene]) = coin(seed);          //Joga a moeda, se true, entao hah esse item

    } else {    //Comeca de uma populacao relativamente media

        int qt_chromo_init = (PCT_GEN*CHROMO_SIZE/100)+1;

        //Para tornarmais caotico
        uniform_int_distribution<int> variance(0,CHROMO_SIZE);
        qt_chromo_init = (int)variance(seed);

        //Inicializa com x% do valor total dos cromossomos
        for(int index_gene = 0; index_gene < CHROMO_SIZE; index_gene++){
			
            if(qt_chromo_init > 0 && coin(seed)){
                (*(*this)[index_gene]) = 1;
                qt_chromo_init--;
            } else {
                (*(*this)[index_gene]) = 0;
            }
        }
    }

    this->fitness_raw = 0;
    this->fitness_nor = 0;
    this->mut         = false;
    this->cross       = false;


};

//Para problemas de permutacao
chromosome::chromosome(vector<gene> genes){
    this->fitness_raw = 0;
    this->fitness_nor = 0;

    for(int i = 0; i < CHROMO_SIZE; i++){
        //Incializa o cromossomo com valores "nulos"
        //que sempre serão referenciados na ultima
        //posicao do vetor de genes
        (*(*this)[i]) = genes[index(seed)];
    }

    this->mut         = false;
    this->cross       = false;
}

//Copia genes
chromosome chromosome::operator=(chromosome c){
    for(int i = 0; i < CHROMO_SIZE; i++){
        (*(*this)[i]) = (*c[i]);
    }

    this->fitness_raw = c.fitness_raw;
    this->fitness_nor = c.fitness_nor;

    this->cross       = c.cross;
    this->mut         = c.mut;
    this->valido      = c.valido;

    return (*this);
}

/** ======================GA====================== **/

//Para problemas de permutacao
GA::GA(vector<gene> genes,
       chromosome * (*crossingOver)(chromosome, chromosome, int),
       void(*mutation)(chromosome* , int),
       int * (*selection)(vector<chromosome>),
       vector<t_hard_constraint> hard_constraint,
       vector<t_soft_constraint> soft_constraint,
       vector<void*> args,
       vector<int> cross_index){

    chromo.clear();

    /* Vetor de posicoes que ocorrera o crossing over*/
    crossOver_index = cross_index;

    /*(2) Inicia a primeira popuçacao*/

    vector<chromosome> vc;

    this->crossingOver      = &(*crossingOver);
    this->mutation          = &(*mutation);
    this->selection         = &(*selection);

    this->hard_constraint   = hard_constraint;
    this->soft_constraint   = soft_constraint;
    this->args              = args;

    //Cria a epoca atual
    for(int index_chromo = 0; index_chromo < N_CHROMO_I; index_chromo++)
        vc.emplace_back(genes);

    //Gera a populacao No 0
    chromo = vc; //Nova Epoca
    curr_epoch = 0;

    calc_fitness();

}

//Para problemas de gene unico
GA::GA(chromosome * (*crossingOver)(chromosome, chromosome, int),
       void(*mutation)(chromosome* , int),
       int * (*selection)(vector<chromosome>),
       vector<t_hard_constraint> hard_constraint,
       vector<t_soft_constraint> soft_constraint,
       vector<void*> args,
       vector<int> cross_index){

    chromo.clear();

    /* Vetor de posicoes que ocorrera o crossing over*/
    crossOver_index = cross_index;

    /*(2) Inicia a primeira popuçacao*/

    vector<chromosome> vc;

    this->crossingOver      = &(*crossingOver);
    this->mutation          = &(*mutation);
    this->selection         = &(*selection);

    this->hard_constraint   = hard_constraint;
    this->soft_constraint   = soft_constraint;
    this->args              = args;


    //Cria a epoca atual
    for(int index_chromo = 0; index_chromo < N_CHROMO_I; index_chromo++)
        vc.emplace_back(true);

    //Gera a populacao No 0
    chromo = vc; //Nova Epoca
    curr_epoch = 0;

    calc_fitness();

}

//=====================================================================================

bool GA::run(enum run_type type) {

    switch (type) {
        case SINGLE_THREAD:
            return this->run_n();
        case MULTI_THREAD:
            return this->run_t();
        case MULTI_THREAD_OPENCL:
            return this->run_cl();
    }

    return false;
}

// Single thread
bool GA::run_n(){


    /*Nova Geracao*/
    vector<chromosome> new_pop;

    clean_meta();

    do{

        /* *(4) Escolhe dois cromossomos da pop e cruza*/

        int * index = selection(chromo);

        chromosome * offspring = nullptr;

        /* Caso exista pontos de cruzamento, faca-os */
        if(crossOver_index.size() > 0){

            for(int cross_index = 0; cross_index < crossOver_index.size(); cross_index++){

                if(crossover_roll(seed) < CROSSOVER_RATE){

                    offspring = crossingOver(chromo[index[0]],
                                             chromo[index[1]],
                                             crossOver_index[cross_index]);
                }

            }

        } else {

            if(crossover_roll(seed) < CROSSOVER_RATE){

                offspring = crossingOver(chromo[index[0]],
                                     chromo[index[1]], 0);
            }

        }

        if(!offspring){

            offspring = new chromosome[2];
            /* If no crossover takes place, form two offspring
            that are exact copies of their respective parents
            http://www.boente.eti.br/fuzzy/ebook-fuzzy-mitchell.pdf */
            offspring[0] = chromo[index[0]];
            offspring[1] = chromo[index[1]];
        }

        /* *(5) Faz a mutacao nos cromossomos prole
           * Ir para '-Sim:' */

        for(int i = 0; i < CHROMO_SIZE; i++){
            if(mutation_roll(seed) < MUTATION_RATE){  mutation(&offspring[0],i); }
            if(mutation_roll(seed) < MUTATION_RATE){  mutation(&offspring[1],i); }
        }

        /*Adiciona os novos cromossomos a populacao*/
        new_pop.push_back(offspring[0]);
        new_pop.push_back(offspring[1]);
                          /* n cromossomos+alguns cromossomos que
                             serao removidos por conta do elitismo */

    }while(new_pop.size() < chromo.size());

    #ifdef ELITISM
     //elitismo
     for(int i = 0; i < ELITISM; i++)
        new_pop.push_back(chromo[i]);
    #endif

    /*(6) Calcula a Fitness de todos os cromossomos*/
    chromo = new_pop; //Nova Epoca

    /* Calcula a fitness */
    calc_fitness();

    #ifdef ELITISM
     //remove os piores elementos
     for(int i = 0; i < ELITISM; i++)
        chromo.pop_back();
    #endif



    curr_epoch++;

    #ifdef CONVERGENCE


    /*(7) Verifica a porcentagem de cromossomos
      que tem a mesma fitness.*/
    struct fit_qt{
        double fit;
        int qt;
    };
    vector<fit_qt> fitpop;

    fit_qt fq;
    fq.fit = chromo[0].fitness_raw;
    fq.qt = 0;
    fitpop.push_back(fq);

    for(int i = 1; i < chromo.size(); i++){                 //Verifica em cada cromossomo
        for(int j = 0; j < fitpop.size(); j++){             //Se seu valor de fitness existe
            if(chromo[i].fitness_raw != fitpop[j].fit){     //se nao existe
                fq.fit = chromo[i].fitness_raw;             //
                fq.qt = 0;                                  //
                fitpop.push_back(fq);                       //add no vetor
                break;
            } else fitpop[j].qt++;
        }
    }


    int max_qt = 0;
    for(int i = 0; i < fitpop.size(); i++){
        if(fitpop[max_qt].qt < fitpop[i].qt) max_qt = i;
    }

    //                                                     90% da populacao tem fitness igual e valido?

    bool fitness_convergence = (fitpop[max_qt].fit > WORST_VALUE(chromo) && fitpop[max_qt].qt > chromo.size()*CONVERGENCE);


    /* (7) acabou() -> 90% tem o mesmo fitness &&
        n de geracoes > limite? */

    /*  --Nao: Volta pro (4)
       */
        return (!fitness_convergence && curr_epoch < IT);
    #else
        return (curr_epoch < IT);
    #endif

}

// Multi thread
bool GA::run_t(){

    /*Nova Geracao*/
    vector<chromosome> new_pop;

    long size_pop = chromo.size()/2;

    vector<thread> t(size_pop);

    clean_meta();

    for(int i = 0; i < size_pop; i++)
        t[i] = thread(&GA::cycle, this, &new_pop);

    for(int i = 0; i < size_pop; i++)  t[i].join();

    #ifdef ELITISM
     //elitismo
     for(int i = 0; i < ELITISM; i++)
        new_pop.push_back(chromo[i]);
    #endif

    /*(6) Calcula a Fitness de todos os cromossomos*/
    chromo = new_pop; //Nova Epoca

    /* Calcula a fitness */
    calc_fitness();

    #ifdef ELITISM
     //remove os piores elementos
     for(int i = 0; i < ELITISM; i++)
        chromo.pop_back();
    #endif

    curr_epoch++;

    #ifdef CONVERGENCE

    /*(7) Verifica a porcentagem de cromossomos
      que tem a mesma fitness.*/
    struct fit_qt{
        double fit;
        int qt;
    };
    vector<fit_qt> fitpop;

    fit_qt fq;
    fq.fit = chromo[0].fitness_raw;
    fq.qt = 0;
    fitpop.push_back(fq);

    for(int i = 1; i < chromo.size(); i++){                 //Verifica em cada cromossomo
        for(int j = 0; j < fitpop.size(); j++){             //Se seu valor de fitness existe
            if(chromo[i].fitness_raw != fitpop[j].fit){     //se nao existe
                fq.fit = chromo[i].fitness_raw;             //
                fq.qt = 0;                                  //
                fitpop.push_back(fq);                       //add no vetor
                break;
            } else fitpop[j].qt++;
        }
    }


    int max_qt = 0;
    for(int i = 0; i < fitpop.size(); i++){
        if(fitpop[max_qt].qt < fitpop[i].qt) max_qt = i;
    }

    //                                                     90% da populacao tem fitness igual e valido?

    bool fitness_convergence = (fitpop[max_qt].fit > WORST_VALUE(chromo) && fitpop[max_qt].qt > chromo.size()*CONVERGENCE);


    /* (7) acabou() -> 90% tem o mesmo fitness &&
        n de geracoes > limite? */

    /*  --Nao: Volta pro (4)
       */
        return (!fitness_convergence && curr_epoch < IT);
    #elifdef DO_UNTIL_FIND
        return (chromo[0].valido && curr_epoch < IT);
    #else
        return (curr_epoch < IT);
    #endif
}

void GA::cycle(vector<chromosome> * new_pop) {

	/* *(4) Escolhe dois cromossomos da pop e cruza*/

	int * index = selection(chromo);

	chromosome * offspring = nullptr;
	/* Caso exista pontos de cruzamento, faca-os */
	if (crossOver_index.size() > 0) {

		for (int cross_index = 0; cross_index < crossOver_index.size(); cross_index++) {

			if (crossover_roll(seed) < CROSSOVER_RATE) {

				offspring = crossingOver(chromo[index[0]],
					chromo[index[1]],
					crossOver_index[cross_index]);
			}

		}

	}
	else {

		if (crossover_roll(seed) < CROSSOVER_RATE) {

			offspring = crossingOver(chromo[index[0]],
				chromo[index[1]], 0);
		}

	}

	if (!offspring) {

		offspring = new chromosome[2];
		/* If no crossover takes place, form two offspring
		that are exact copies of their respective parents
		http://www.boente.eti.br/fuzzy/ebook-fuzzy-mitchell.pdf */
		offspring[0] = chromo[index[0]];
		offspring[1] = chromo[index[1]];

	}

	/* *(5) Faz a mutacao nos cromossomos prole
	* Ir para '-Sim:' */

	for (int i = 0; i < CHROMO_SIZE; i++) {
		if (mutation_roll(seed) < MUTATION_RATE) { mutation(&offspring[0], i); }
		if (mutation_roll(seed) < MUTATION_RATE) { mutation(&offspring[1], i); }
	}

	lock_guard<mutex> lock_add_vector(m);
	/*Adiciona os novos cromossomos a populacao*/
	new_pop->push_back(offspring[0]);
	new_pop->push_back(offspring[1]);
	/* n cromossomos+alguns cromossomos que
	serao removidos por conta do elitismo */

}

//=====================================================================================

void GA::calc_fitness_i(int index){

    chromo[index].fitness_raw = 0;

    //Para cada soft constraint, incrementa o valor da fitness
    for(int sc = 0; sc < soft_constraint.size(); sc++)
        chromo[index].fitness_raw += (soft_constraint[sc]( chromo[index], args ));

    chromo[index].valido = true;

    //Para cada hard constraint, verifica se a solucao eh valida
    for(int hc = 0; hc < hard_constraint.size(); hc++){
        if(  hard_constraint[hc]( chromo[index], args ) ){
            chromo[index].fitness_raw /= 10;
            chromo[index].valido = false;
        }
    }
}

bool GA::isValid(chromosome c){
    for(int hc = 0; hc < hard_constraint.size(); hc++){
        if(  hard_constraint[hc]( c, args) ){
            return false;
        }
    }
    return true;
}

void GA::calc_fitness(){

    vector<thread> t(chromo.size());

    for(int index = 0; index < chromo.size(); index++)
        t[index] = thread(&GA::calc_fitness_i, this, index);

    for(int index = 0; index < chromo.size(); index++) t[index].join();

    //Ordena por ordem decrescente de fitness
    sort_dec();

    calc_max_fitness();
    calc_avg_fitness();
    calc_min_fitness();
    #ifdef NORMALIZE
    normalize();
    #endif

}


void GA::normalize(){

    /* ===================================== */
    for(int i = 0; i < chromo.size(); i++)
        chromo[i].fitness_nor = chromo[i].fitness_raw;

    while(min_fitness < 0 && max_fitness > 0){

        for (int i = 0; i < chromo.size(); i++){
            chromo[i].fitness_nor += max_fitness;
        }

        min_fitness += max_fitness;
        avg_fitness += max_fitness;
        max_fitness += max_fitness;

    }
    /* ===================================== */

    for(int i = 0; i < chromo.size(); i++){
        chromo[i].fitness_nor =
        (max_fitness > 0) ?
        chromo[i].fitness_nor/max_fitness:
        max_fitness/chromo[i].fitness_nor;

    }
    /* ===================================== */
}


void GA::calc_max_fitness(){
    max_fitness = chromo[0].fitness_raw;
    if(best_chromo.fitness_raw < chromo[0].fitness_raw)
        best_chromo = chromo[0];
}


void GA::calc_avg_fitness(){

    avg_fitness = 0;
    for(int i = 0; i < chromo.size(); i++)
        avg_fitness += chromo[i].fitness_raw;

    avg_fitness /= chromo.size();

}

void GA::calc_min_fitness(){

    min_fitness = chromo[chromo.size()-1].fitness_raw;

}

void GA::clean_meta(){

    for(int i = 0; i < chromo.size(); i++){
        chromo[i].cross = false;
        chromo[i].mut   = false;
    }

}

void GA::sort_dec(){

    qsort(&chromo[0], chromo.size(), sizeof(chromosome),[](const void * a, const void * b){
    if ( ( (chromosome*)a )->fitness_raw <  ( (chromosome*)b )->fitness_raw ) return 1;
    if ( ( (chromosome*)a )->fitness_raw >  ( (chromosome*)b )->fitness_raw ) return -1;
    return 0;});
}
