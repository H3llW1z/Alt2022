
#include "tree.h"
extern int numOfVars;
//считает расстояние от вершини до ближайшего листа-переменной
int distToLeaf(node* node) {

    if (node->value > 0)   //если попали в переменную - возврат. Переменная имеет вес левого и правого ребра равный 0.
        return 0;

    if (node->value == -1) {        //если попали в отрицание
        node->leftWeight = 999; // отрицание имеет отдельную обработку. Веса на отрицании никогда не проверяются
        node->rightWeight = distToLeaf(node->right) + 1;    //найдём вес правого ребра и прибавим к нему 1 (если справа переменная, то расстояние до неё - 1)
        return node->rightWeight;
    }
    node->leftWeight = distToLeaf(node->left) + 1;          //в случае других операторов считаем вес левого и правого ребра и возвращаем наименьший из них
    node->rightWeight = distToLeaf(node->right) + 1;
    return min(node->leftWeight, node->rightWeight);

}
//добавляет узлы в дерево выражения
void addnode(string expression, node* tree) {

    removeOuterBraces(expression);   //удалим внешние скобки (а+b) -> a+b

    int lpIndex = find_low_priority_operator(expression); //найдём индекс наименее приоритетного оператора

    //если оператора нет, то имеем строку, содержащую только одну переменную, прим. a15. Запишем ее номер в узел дерва
    if (lpIndex == -1) {
        stringstream intVarContainer(expression.substr(1, expression.size() - 1));
        int intVar;
        intVarContainer >> intVar;
        tree->value = intVar;
        if (numOfVars < intVar) numOfVars = intVar;
        return;
    }
    tree->value = getOperatorsIntForm(expression[lpIndex]);   //в противном случае имеем оператор - запишем его номер в узел дерева

    if (tree->value == -1) {        //если попали в отрицание, продолжаем строить только правое поддерево. Отрицание - унарный оператор
        tree->right = new node;
        addnode(expression.substr(lpIndex + 1), tree->right);
    }
    else {                                          //в противном случае строим оба поддерева
        tree->left = new node;
        tree->right = new node;
        addnode(expression.substr(0, lpIndex), tree->left);
        addnode(expression.substr(lpIndex + 1), tree->right);
    }
}
