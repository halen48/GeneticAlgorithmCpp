#include "../ga.h"

/*
Caso os ponteiros das funções não estejam na GPU, não funcionará!
*/

__kernel void cycle(chromosome * chromo, chromosome * new_pop) {

	int id = get_global_id(0);

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

	/*Adiciona os novos cromossomos a populacao*/
	new_pop[id]		= (offspring[0]);
	new_pop[id+1]	= (offspring[1]);
	/* n cromossomos+alguns cromossomos que
	serao removidos por conta do elitismo */

}//
