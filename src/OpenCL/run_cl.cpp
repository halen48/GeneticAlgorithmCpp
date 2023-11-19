#include "../ga.h"
#include "contexto_cl.h"

// Multi thread OpenCL
bool GA::run_cl() {

	/*Nova Geracao*/
	vector<chromosome> new_pop;

	long size_pop = chromo.size() / 2;

	vector<thread> t(size_pop);

	clean_meta();

	for (int i = 0; i < size_pop; i++)
		t[i] = thread(&GA::cycle, this, &new_pop);

	for (int i = 0; i < size_pop; i++)  t[i].join();

#ifdef ELITISM
	//elitismo
	for (int i = 0; i < ELITISM; i++)
		new_pop.push_back(chromo[i]);
#endif

	/*(6) Calcula a Fitness de todos os cromossomos*/
	chromo = new_pop; //Nova Epoca

					  /* Calcula a fitness */
	calc_fitness();

#ifdef ELITISM
	//remove os piores elementos
	for (int i = 0; i < ELITISM; i++)
		chromo.pop_back();
#endif

	curr_epoch++;

#ifdef CONVERGENCE

	/*(7) Verifica a porcentagem de cromossomos
	que tem a mesma fitness.*/
	struct fit_qt {
		double fit;
		int qt;
	};
	vector<fit_qt> fitpop;

	fit_qt fq;
	fq.fit = chromo[0].fitness_raw;
	fq.qt = 0;
	fitpop.push_back(fq);

	for (int i = 1; i < chromo.size(); i++) {                 //Verifica em cada cromossomo
		for (int j = 0; j < fitpop.size(); j++) {             //Se seu valor de fitness existe
			if (chromo[i].fitness_raw != fitpop[j].fit) {     //se nao existe
				fq.fit = chromo[i].fitness_raw;             //
				fq.qt = 0;                                  //
				fitpop.push_back(fq);                       //add no vetor
				break;
			}
			else fitpop[j].qt++;
		}
	}


	int max_qt = 0;
	for (int i = 0; i < fitpop.size(); i++) {
		if (fitpop[max_qt].qt < fitpop[i].qt) max_qt = i;
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