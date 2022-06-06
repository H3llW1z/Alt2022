#pragma once
#include "tree.h"
#include "main.h"
#include "expressionFunctions.h"
void postOrderTravers(node* node);
void incBitSet(bitset<NEW_ALGO_MAX_VARS>& bitset);
vector<bitset<NEW_ALGO_MAX_VARS>> rpnCalcutator(node* node, int flag);