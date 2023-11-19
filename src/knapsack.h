#pragma once
#include <vector>
#include "common.h"

/*Numero de parametros para calcular a fitness*/
#define N_PARAM 4
/*============================================*/

#define MAX_WEIGHT_BAG 400
#define MAX_VOLUME_BAG 400
#define MAX_ITEM 20
#define MAX_CLASS 2

using namespace std;

enum item_params {
    I_WEIGHT,
    I_VALUE,
    I_VOLUME,
    I_CLASS,
    MAX_PARAMS = N_PARAM
};

struct item{
    double param[N_PARAM];
};

/*Apelido para compatibilidade com o algoritmo*/
typedef item info;
/*============================================*/

struct bag{

    bool items[MAX_ITEM];

};

class Knapsack {
private:
    vector<item> items;
    vector<item> generateItems(int, int fix = 0);
public:
    Knapsack();
    const item* Knapsack::operator[](int);
    const vector<item> * get_items();
};

/**=======================================================================**/
/**
    bool hard_constraint (chromosome, args);
**/

/*Se os itens na mochila pesam mais do que a mochila possa carregar*/
hard_constraint_func(overweight);

/*Se os itens na mochila tem mais volume do que a mochila possa suportar*/
hard_constraint_func(overloaded);

hard_constraint_func(empty);

/***************************************************************************/
/**
    double soft_constraint (chromosome, args);
**/
soft_constraint_func(weightvalue);
soft_constraint_func(volumevalue);
soft_constraint_func(weightvalue_qttitem);
soft_constraint_func(volumevalue_qttitem);
