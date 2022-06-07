#pragma once
#include "main.h";
//вспомогательная структура для поиска наименее приоритетного оператора 
struct operators
{
    operators(int ind, char val) {
        value = val;
        index = ind;
    };
    char value;
    int index;
};
bool checkBraces(string str);
void removeOuterBraces(string& str);
int getOperatorsIntForm(char op);
bool isOperator(char ch);
int find_low_priority_operator(string expression);