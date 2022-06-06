#pragma once
#include "main.h"
#include "expressionFunctions.h"
//структура, определяющая узел дерева выражения
struct node
{

    struct node* left = nullptr;   //указатель на левого потомка
    struct node* right = nullptr;  //указатель на правого потомка
    short value = 0;               //значение узла - номер переменной (положительное целое) или номер оператора (отрицательное целое)
    short leftWeight = 0;          //вес левого ребра
    short rightWeight = 0;         //вес правого ребра
};
int distToLeaf(node* node);
void addnode(string expression, node* tree);