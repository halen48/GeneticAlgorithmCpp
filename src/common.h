#pragma once
#include <vector>

class chromosome;
using namespace std;

#define hard_constraint_func(v) bool (v)(chromosome c, vector<void*> args)
#define soft_constraint_func(v) double (v)(chromosome c, vector<void*> args)

typedef hard_constraint_func(*t_hard_constraint);
typedef soft_constraint_func(*t_soft_constraint);