#pragma once

#include "parameters.h"
#include <mutex>
#include <random>
#include <chrono>
#include <ctime>
#include <CL/opencl.h>
#include "common.h"

using namespace std;

static mt19937_64                           seed(std::chrono::system_clock::now().time_since_epoch().count());
static uniform_int_distribution<int>        index(0,CHROMO_SIZE-1);
static uniform_real_distribution<float>     crossover_roll(0,100);
static uniform_real_distribution<float>     mutation_roll(0,100);
static bernoulli_distribution               coin(TAILS);

enum run_type {
    SINGLE_THREAD,
    MULTI_THREAD,
    MULTI_THREAD_OPENCL,
};

class chromosome{

    gene g[CHROMO_SIZE];

    public:

        /*Inicializa o cromossomo o valor 'vazio', que vem a ser a ultima posicao do gene*/
        chromosome(vector<gene>);               //Para problemas de permutacao de gene
        chromosome(bool big_chromo = false);         //Para problemas de gene unico
        /*===========================*/

        /* fitness */
        double fitness_raw;
        double fitness_nor;
        bool cross, mut, valido;
        /*===========================*/

        /* apelido para variaveis */
        inline int size(){return CHROMO_SIZE;}

        double fitness() {
            #ifdef NORMALIZE
            return this->fitness_nor;
            #else
            return this->fitness_raw;
            #endif
        }

        gene * operator[](int index){return &g[index];}
        /*===========================*/

        /* operadores */
        chromosome operator=(chromosome);
        /*===========================*/

};

class GA{

    private:

        mutex m;

        /* regras de avalicao */
        vector<t_hard_constraint> hard_constraint;
        vector<t_soft_constraint> soft_constraint;

        vector<void*> args;

        /* ponteiro para funcoes */
        chromosome *    (*crossingOver) (chromosome, chromosome, int);
        void            (*mutation)     (chromosome* , int);
        int *           (*selection)    (vector<chromosome>);

        void           sort_dec();
        void           calc_fitness_i(int);
        bool           isValid(chromosome);

        bool           run_n(); //Single Thread
        bool           run_t(); //Multi Thread
        bool	       run_cl();//Multi Thread with OpenCL

    public:
        vector<chromosome>  chromo;
        vector<int>         crossOver_index;
        long long           curr_epoch;
        double              max_fitness;
        double              avg_fitness;
        double              min_fitness;

        chromosome          best_chromo;

    public:
        //Para problemas de gene unico
        GA(chromosome * (*)(chromosome,chromosome, int),    //tipo de crossing over
           void(*)(chromosome* ,int),                       //tipo de mutacao
           int * (*selection)(vector<chromosome>),          //tipo de selecao
           vector<t_hard_constraint>,                       //Hard Constraint
           vector<t_soft_constraint>,                       //Soft Constraint
           std::vector<void*>,                              //argumentos adicionais para as constraints
           vector<int> cross_index = vector<int>());        //indices do crossing over (nao obrigatorio)

        //Para problemas de permutacao
        GA(vector<gene>,                                    //lista de genes contendo os valores do problema
           chromosome * (*)(chromosome,chromosome, int),    //tipo de crossing over
           void(*)(chromosome* ,int),                       //tipo de mutacao
           int * (*selection)(vector<chromosome>),          //tipo de selecao
           vector<t_hard_constraint>,                       //Hard Constraint
           vector<t_soft_constraint>,                       //Soft Constraint
           std::vector<void*>,                              //argumentos adicionais para as constraints
           vector<int> cross_index = vector<int>());        //indices do crossing over (nao obrigatorio)

        void    calc_fitness();
        void    normalize();
        int     curr_size(){return chromo.size();}
        bool    run(enum run_type);
        void    cycle(vector<chromosome> *);
        void    calc_max_fitness();
        void    calc_avg_fitness();
        void    calc_min_fitness();
        void    clean_meta();

};