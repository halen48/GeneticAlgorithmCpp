#include "knapsack.h"
#include "operators/operators.h"
#include <vector>
#include <random>
#include <chrono>

static std::mt19937_64 gen(std::chrono::system_clock::now().time_since_epoch().count());
static uniform_real_distribution<float> val1(2, 15.3);
static weibull_distribution<float> weight(val1(gen), MAX_WEIGHT_BAG);
static weibull_distribution<float> volume(val1(gen), MAX_VOLUME_BAG);
static uniform_real_distribution<float> value(0.1, MAX_WEIGHT_BAG);
static uniform_int_distribution<int> class_(0, MAX_CLASS - 1);

Knapsack::Knapsack() {
    this->items = generateItems(MAX_ITEM);
}

const vector<item>* Knapsack::get_items() {
    return &(this->items);
}

const struct item * Knapsack::operator[](int idx) {
    return &(this->items[idx]);
}

vector<item> Knapsack::generateItems(int max_items, int fix){

    float va_,
          vo_,
          we_,
          ca_;

    vector<item> itens;

    while(max_items-- > 0){
        if(fix==1){

            va_ = MAX_WEIGHT_BAG/(max_items+1);
            vo_ = MAX_VOLUME_BAG/(max_items+3);
            we_ = MAX_WEIGHT_BAG/(max_items+3);
            ca_ = max_items%MAX_CLASS;


        } else if(!fix) {



            va_ = ceil(value(gen)*100)/10;
            vo_ = ceil(volume(gen)*100)/100;
            we_ = ceil(weight(gen)*100)/100;
            ca_ = class_(gen);

        } else continue;

        item i;

        (i.param[I_VALUE])   =   va_;
        (i.param[I_WEIGHT])  =   we_;
        (i.param[I_VOLUME])  =   vo_;
        (i.param[I_CLASS])  =   ca_;

        itens.push_back(i);
    }

    if(fix==2){
        item i;

        (i.param[I_VALUE])   =   9758.8;
        (i.param[I_WEIGHT])  =   358.16;
        (i.param[I_VOLUME])  =   193.79;
        itens.push_back(i);
        (i.param[I_VALUE])   =   1070.6;
        (i.param[I_WEIGHT])  =   1053.94;
        (i.param[I_VOLUME])  =   464.1;
        itens.push_back(i);
        (i.param[I_VALUE])   =   732.6;
        (i.param[I_WEIGHT])  =   704.97;
        (i.param[I_VOLUME])  =   110.52;
        itens.push_back(i);
        (i.param[I_VALUE])   =   8574.4;
        (i.param[I_WEIGHT])  =   435.15;
        (i.param[I_VOLUME])  =   198.17;
        itens.push_back(i);
        (i.param[I_VALUE])   =   4233.5;
        (i.param[I_WEIGHT])  =   295.65;
        (i.param[I_VOLUME])  =   142.91;
        itens.push_back(i);
        (i.param[I_VALUE])   =   8522.7;
        (i.param[I_WEIGHT])  =   671.19;
        (i.param[I_VOLUME])  =   106.44;
        itens.push_back(i);
        (i.param[I_VALUE])   =   9109.2;
        (i.param[I_WEIGHT])  =   476.62;
        (i.param[I_VOLUME])  =   568.89;
        itens.push_back(i);
        (i.param[I_VALUE])   =   7879.4;
        (i.param[I_WEIGHT])  =   303.17;
        (i.param[I_VOLUME])  =   87.89;
        itens.push_back(i);
        (i.param[I_VALUE])   =   1746.8;
        (i.param[I_WEIGHT])  =   965.54;
        (i.param[I_VOLUME])  =   166.53;
        itens.push_back(i);
        (i.param[I_VALUE])   =   952.5;
        (i.param[I_WEIGHT])  =   576.47;
        (i.param[I_VOLUME])  =   72.7;
        itens.push_back(i);

    }

    return itens;

}

/**=======================================================================**/
/**
    bool hard_constraint (chromosome);
**/

/*Se os itens na mochila pesam mais do que a mochila possa carregar*/
hard_constraint_func(overweight) {
    std::vector<item> * items_ptr = (std::vector<item>*)args[0];
#define items (*items_ptr)
    double weight = 0;
    for (int i = 0; i < CHROMO_SIZE; i++) {
        if ((*c[i])) weight += (items[i].param[I_WEIGHT]);
    }
#undef items
    return (weight > MAX_WEIGHT_BAG);
}

/*Se os itens na mochila tem mais volume do que a mochila possa suportar*/
hard_constraint_func(overloaded) {
    std::vector<item>* items_ptr = (std::vector<item>*)args[0];
#define items (*items_ptr)
    double volume = 0;
    for (int i = 0; i < CHROMO_SIZE; i++) {
        if ((*c[i])) volume += (items[i].param[I_VOLUME]);
    }
#undef items
    return (volume > MAX_VOLUME_BAG);
}

hard_constraint_func(empty) {
    return (c.size() == 0);
}
/***************************************************************************/
/**
    double soft_constraint (chromosome);
**/

soft_constraint_func(weightvalue) {
    std::vector<item>* items_ptr = (std::vector<item>*)args[0];
#define items (*items_ptr)
    double value = 0;
    for (int i = 0; i < CHROMO_SIZE; i++) {
        if ((*c[i]))
            value += (items[i].param[I_VALUE]);
    }
#undef items
    return value / MAX_ITEM;  //para evitar valores altos
}

soft_constraint_func(volumevalue) {
    std::vector<item>* items_ptr = (std::vector<item>*)args[0];
#define items (*items_ptr)
    double value = 0;
    for (int i = 0; i < CHROMO_SIZE; i++) {
        if ((*c[i]))
            value += (items[i].param[I_VALUE]);
    }
#undef items
    return value / MAX_ITEM; //para evitar valores altos
}

soft_constraint_func(weightvalue_qttitem) {
    std::vector<item>* items_ptr = (std::vector<item>*)args[0];
#define items (*items_ptr)
    double value = 0;
    int qt_itens = 0;
    for (int i = 0; i < CHROMO_SIZE; i++) {
        if ((*c[i])) {
            value += (items[i].param[I_VALUE]);
            qt_itens++;
        }
    }
#undef items
    if (qt_itens == 0) return 0;
    return value / qt_itens;
}

soft_constraint_func(volumevalue_qttitem) {
    std::vector<item>* items_ptr = (std::vector<item>*)args[0];
#define items (*items_ptr)
    double value = 0;
    int qt_itens = 0;
    for (int i = 0; i < CHROMO_SIZE; i++) {
        if ((*c[i])) {
            value += (items[i].param[I_VALUE]);
            qt_itens++;
        }
    }
#undef items
    if (qt_itens == 0) return 0;
    return value / qt_itens;
}
