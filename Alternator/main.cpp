﻿#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <cstdlib>
#include <time.h>
#include <random>
#include <iterator>
#include <list>
#include <stack>
#include <bitset>
#include <fstream>

using namespace std;


#define MAX_VARS 15     //максимальное количество переменных в формуле. МОЖНО РЕДАКТИРОВАТЬ (C++ не имеет встроенных динамических битовых шкал, сторонние работают хуже.
//Если удастся найти хорошую реализацию динамических шкал, программа будет доработана с их использованием. Это лишь тонкость реализации.


//Из-за того, что битовые шкалы статические, необходимо задать фиксированный размер. Эти значения можно менять, если Вам нужно больше переменных или более длинные формулы
#define NEW_ALGO_MAX_VARS 200
#define NEW_ALGO_STACK_LIMIT 50000

//Флаги для выбора режима калькулятора на ОПЗ (СКНФ или СДНф)
#define FLAG_SKNF -25 
#define FLAG_SDNF -50


int numOfVars = -1;   //хранит количество переменных в формуле, нужно для ограничения рабочей части битовой шкалы калькулятора ОПЗ


//структура, определяющая узел дерева выражения
struct node
{

    struct node* left = nullptr;   //указатель на левого потомка
    struct node* right = nullptr;  //указатель на правого потомка
    short value = 0;               //значение узла - номер переменной (положительное целое) или номер оператора (отрицательное целое)
    short leftWeight = 0;          //вес левого ребра
    short rightWeight = 0;         //вес правого ребра
};

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

//Проверяет правильность скобочной конструкции
bool checkBraces(string str) {
    int braceCounter = 0;
    for (int i = 0; i <= str.size() - 1; i++) {
        if (str[i] == ')') {
            braceCounter -= 1;
        }
        if (str[i] == '(') {
            braceCounter += 1;
        }
        if (braceCounter < 0)
            return false;
    }
    if (braceCounter == 0)
        return true;
    else return false;
}

//Удаляет внешние парные скобки, пример ((a+b)) -> a+b
void removeOuterBraces(string& str) {
    if (str[0] != '(' || str[str.size() - 1] != ')')
        return;
    bool isGood = true;
    while (isGood) {
        if (str[0] != '(' || str[str.size() - 1] != ')')
            return;
        str.erase(0, 1);
        str.erase(str.size() - 1, 1);
        isGood = checkBraces(str);
    }
    str.insert(0, "(");
    str.append(")");
}

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

//массив всех доступных операторов, расставленных в порядке возрастания приоритета
char priorityArray[8] = { 'v', '|', '=', '>', '^', '+', '*', '!' };
//числ. обоз-я операторов -8   -7   -6   -5   -4   -3   -2   -1


//возвращает номер оператора по символу
int getOperatorsIntForm(char op) {
    switch (op) {
    case 'v': return -8; break;
    case '|': return -7; break;
    case '=': return -6; break;
    case '>': return -5; break;
    case '^': return -4; break;
    case '+': return -3; break;
    case '*': return -2; break;
    case '!': return -1; break;
    default: throw::invalid_argument("Wrong operator char");
    }
}

//проверяет, является ли символ обозначением оператора
bool isOperator(char ch) {

    for (int i = 0; i < 8; i++) {
        if (ch == priorityArray[i])
            return true;
    }
    return false;
}

//ищет наименее приоритетный оператор и возвращает его позицию в строке
int find_low_priority_operator(string expression) {
    bool isThereOperator = false;
    for (int i = 0; i < expression.size(); i++) {       //если в строке нет операторов, то вернуть -1
        isThereOperator = isOperator(expression[i]);
        if (isThereOperator) {
            break;
        }
    }

    if (!isThereOperator) {
        return -1;
    }

    /*Идем по строке справа налево и считаем попавшиеся скобки. Если попали в оператор и закрытых скобок столько же,
    * сколько открытых, то сохраняем его индекс в строке. Затем среди всех найденных операторов найдём самый правый
    * наименее приоритетный
    */
    int openBrace = 0;
    int closeBrace = 0;
    vector<operators> v;

    for (int i = expression.size() - 1; i >= 0; i--)
    {
        if (expression[i] == ')') {
            closeBrace += 1;

        }
        if (expression[i] == '(') {
            openBrace += 1;
        }
        if (closeBrace == openBrace && isOperator(expression[i])) {
            v.push_back(operators(i, expression[i]));

            closeBrace = 0;
            openBrace = 0;
        }
    }
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j <= v.size() - 1; j++) {
            if (v[j].value == priorityArray[i]) {
                return v[j].index;
            }
        }
    }
    return 0;
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


// Пример выражения, разобранного в дерево
//
// (x+y)^(x*z)^((z+x)*y) 
// 
// 
//            ^
//          /   \
//         /     \
//        /       \
//       /         \
//      ^           *
//    /   \        / \
//   +     *      +   y
//  / \   / \    / \
// x   y x   z   z  x


//этот массив представляет собой дерево. Он нужен для определения подходящих операндов
//первая координата - числовое представление оператора минус 2. Вторая - желаемое значение. Так можно попасть в массив подходящих операндов.
//здесь отсутствует отрицание, т.к. его обработка тривиальна

short operands[7][2][7] = {
    { { 3, 1,0, 0,1, 0,0 }, { 1, 1,1, -1,-1, -1,-1 } },   // 2 умножение

    { { 1, 0,0, -1,-1, -1,-1 }, { 3, 0,1, 1,0, 1,1 } },   // 3 сложение

    { { 2, 0,0, 1,1, -1,-1 }, { 2, 0,1, 1,0, -1,-1 } },   // 4 XOR

    { { 1, 1,0, -1,-1, -1,-1 }, { 3, 1,1, 0,0, 0,1 } },   // 5 импликация

    { { 2, 0,0, 1,1, -1,-1 }, { 2, 0,1, 1,0, -1,-1 } },   // 6 эквиваленция

    { { 1, 1,1, -1,-1, -1,-1 }, { 3, 1,0, 0,1, 0,0 } },   // 7 штрих шеффера

    { { 3, 0,1, 1,0, 1,1 }, { 1, 0,0, -1,-1, -1,-1 } },   // 8 стрелка пирса
};

//структура - член СКНФ(СДНФ)
//представлена двумя битовыми шкалами
//если i-й бит в первой шкале установлен в 1, значит переменная уже внесена в комбинацию
//вторая шкала хранит информацию о знаке переменной в бите на i-ом месте. (0 - нет отрицания, 1- есть отрицание)

struct sknfMember {
    bitset <MAX_VARS> vars;    //шкала переменных
    bitset <MAX_VARS> signs;   //шкала знаков переменных
};

//глобальные переменные, которые использует функция поиска
//объявлены глобально, т.к. требуется лишь по одному экзмепляру каждой в любой момент времени
//----------------------------------------------------------------------

list<sknfMember>::iterator it1;         //итератор для перемещения по списку комбинаций

stack <list<sknfMember>> globalStack;   //глобальный стек для сохранения данных, пришедших из родителя

//-----------------------------------------------------------------------



//функция поиска СКНФ
void sknfSearch(bool wantedValue, list<sknfMember>& lst, node* node) {

    if (lst.size() == 0) {   //если родитель прислал пустой список (не опустошённый, а пустой изначально!) - добавить туда уже ничего нельзя. Выходим
        return;
    }

    if (node->value < 0) {         //если попали в оператор

        if (node->value == -1) {   //в случае отрицания инвертируем желаемое значение и продолжим поиск направо
            sknfSearch(!wantedValue, lst, node->right);
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 1) {   // если пара операндов одна

            if (node->rightWeight < node->leftWeight) {         //если по правому ребру можно быстрее попать в переменную, идём сперва направо, затем налево
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
            }
            else {             //в противном случае идём налево, затем направо
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
            }
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 2) {   //если пар операндов две

            globalStack.push(lst);   //информацию из родителя нужно сохранить для рассмотрения второго случая. Положим ее на стек

            if (node->rightWeight < node->leftWeight) {            //развилка, аналогичная первому случаю

                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right); //пойдём право и налево со списком родителя
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);  //пойдём направо и налево со списком, хранящимся в стеке
                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);
            }
            else {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);
            }
            lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());   //объединим результаты обоих случаев в список родителя
            globalStack.pop();   //снимем результат второго случая со стека
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 3) {  //если пар операндов 3

            globalStack.push(lst);  //информацию из родителя нужно сохранить для рассмотрения второго и третьего случая. Положим ее на стек

            /*В случае трёх пар есть возможность сократить работу. Среди трёх пар одно число встречается 2 раза и среди левых значений, и среди правых
            * Например, импликация даёт 1 на парах (0,0), (0,1). (1,1). Если начинать поиск слева, то искать 0 слева нужно только 1 раз, а если
            * начинать искать справа, то единицу только 1 раз. Таким образом рассмотрим 3 пары за 5 вызовов вместо 6.
            */
            if (node->rightWeight < node->leftWeight) {     //аналогично случаю 1

                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], lst, node->right);  //рассмотрим "выбивающийся" случай. Тот, для которого и налево и направо нужно идти отдельно
                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], lst, node->left);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], globalStack.top(), node->right);  //сходим направо сразу для двух оставшихся случаев при помощи списка из стека

                globalStack.push(globalStack.top()); //скопируем в стек результат "общего" похода, чтобы совершить 2 последних вызова

                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], globalStack.top(), node->left);    //идём налево со списом со стека
                lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());  //запишем результат в список родителя

                globalStack.pop();   //снимем со стека один список

                sknfSearch(operands[abs(node->value) - 2][wantedValue][5], globalStack.top(), node->left); //сходим налево для третьего случая
            }
            else {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);

                globalStack.push(globalStack.top());

                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);
                lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());

                globalStack.pop();

                sknfSearch(operands[abs(node->value) - 2][wantedValue][6], globalStack.top(), node->right);
            }
            lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());  //запишем в конец списка родителя результат последнего третьего случая
            globalStack.pop();  //снимем со стека второй буферный список
            return;
        }
    }
    else {    //если перед нами переменная

        //если список пустой изначально, просто добавим туда одну комбинацию из одной переменной
        //Чтобы отличить пустой список от того, из которого удалены все комбинации, в нём создаётся невозможная ситуация - "метка"
        //В бит знака первой переменной ставится 1, что означает отрицание. Но в шкале переменных у первой переменной оставляют 0
        //Именно эту ситуацию проверяет if
        if (lst.size() == 1 && (*lst.begin()).signs.test(0) && !((*lst.begin()).vars.test(0))) {

            (*lst.begin()).signs.set(0, 0);   //уберём "метку"
            (*lst.begin()).vars.set(node->value - 1, 1);  //отметим найденную переменную
            (*lst.begin()).signs.set(node->value - 1, wantedValue);   //и запишем ее знак
            return;
        }
        else {  //если же список комбинаций не пуст, надо пройти по нему и добавить переменную туда, где ее не хватает. При противоречиях вырезать комбинацию

            it1 = lst.begin();  //внешний итератор поместим в начало списка комбинаций

            while (it1 != lst.end()) {

                if ((*it1).vars.test(node->value - 1)) {
                    if ((*it1).signs.test(node->value - 1) != wantedValue) {
                        lst.erase(it1++);
                    }
                    else {
                        ++it1;
                    }
                }
                else {
                    (*it1).vars.set(node->value - 1, 1);
                    (*it1).signs.set(node->value - 1, wantedValue);
                    ++it1;
                }
            }
            return;
        }
    }
}

//функция поиска СДНФ
void sdnfSearch(bool wantedValue, list<sknfMember>& lst, node* node) {

    if (lst.size() == 0) {   //если родитель прислал пустой список (не опустошённый, а пустой изначально!) - добавить туда уже ничего нельзя. Выходим
        return;
    }

    if (node->value < 0) {         //если попали в оператор

        if (node->value == -1) {   //в случае отрицания инвертируем желаемое значение и продолжим поиск направо
            sdnfSearch(!wantedValue, lst, node->right);
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 1) {   // если пара операндов одна

            if (node->rightWeight < node->leftWeight) {         //если по правому ребру можно быстрее попать в переменную, идём сперва направо, затем налево
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
            }
            else {             //в противном случае идём налево, затем направо
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
            }
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 2) {   //если пар операндов две

            globalStack.push(lst);   //информацию из родителя нужно сохранить для рассмотрения второго случая. Положим ее на стек

            if (node->rightWeight < node->leftWeight) {            //развилка, аналогичная первому случаю

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right); //пойдём право и налево со списком родителя
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);  //пойдём направо и налево со списком, хранящимся в стеке
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);
            }
            else {
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);
            }
            lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());   //объединим результаты обоих случаев в список родителя
            globalStack.pop();   //снимем результат второго случая со стека
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 3) {  //если пар операндов 3

            globalStack.push(lst);  //информацию из родителя нужно сохранить для рассмотрения второго и третьего случая. Положим ее на стек

            /*В случае трёх пар есть возможность сократить работу. Среди трёх пар одно число встречается 2 раза и среди левых значений, и среди правых
            * Например, импликация даёт 1 на парах (0,0), (0,1). (1,1). Если начинать поиск слева, то искать 0 слева нужно только 1 раз, а если
            * начинать искать справа, то единицу только 1 раз. Таким образом рассмотрим 3 пары за 5 вызовов вместо 6.
            */
            if (node->rightWeight < node->leftWeight) {     //аналогично случаю 1

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][4], lst, node->right);  //рассмотрим "выбивающийся" случай. Тот, для которого и налево и направо нужно идти отдельно
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][3], lst, node->left);

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][2], globalStack.top(), node->right);  //сходим направо сразу для двух оставшихся случаев при помощи списка из стека

                globalStack.push(globalStack.top()); //скопируем в стек результат "общего" похода, чтобы совершить 2 последних вызова

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][1], globalStack.top(), node->left);    //идём налево со списом со стека
                lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());  //запишем результат в список родителя

                globalStack.pop();   //снимем со стека один список

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][5], globalStack.top(), node->left); //сходим налево для третьего случая
            }
            else {
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sdnfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);

                globalStack.push(globalStack.top());

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);
                lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());

                globalStack.pop();

                sdnfSearch(operands[abs(node->value) - 2][wantedValue][6], globalStack.top(), node->right);
            }
            lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());  //запишем в конец списка родителя результат последнего третьего случая
            globalStack.pop();  //снимем со стека второй буферный список
            return;
        }
    }
    else {    //если перед нами переменная

        //если список пустой изначально, просто добавим туда одну комбинацию из одной переменной
        //Чтобы отличить пустой список от того, из которого удалены все комбинации, в нём создаётся невозможная ситуация - "метка"
        //В бит знака первой переменной ставится 1, что означает отрицание. Но в шкале переменных у первой переменной оставляют 0
        //Именно эту ситуацию проверяет if
        if (lst.size() == 1 && (*lst.begin()).signs.test(0) && !((*lst.begin()).vars.test(0))) {

            (*lst.begin()).signs.set(0, 0);   //уберём "метку"
            (*lst.begin()).vars.set(node->value - 1, 1);  //отметим найденную переменную
            (*lst.begin()).signs.set(node->value - 1, !wantedValue);   //и запишем ее знак
            return;
        }
        else {  //если же список комбинаций не пуст, надо пройти по нему и добавить переменную туда, где ее не хватает. При противоречиях вырезать комбинацию

            it1 = lst.begin();  //внешний итератор поместим в начало списка комбинаций

            while (it1 != lst.end()) {

                if ((*it1).vars.test(node->value - 1)) {
                    if ((*it1).signs.test(node->value - 1) != !wantedValue ) {
                        lst.erase(it1++);
                    }
                    else {
                        ++it1;
                    }
                }
                else {
                    (*it1).vars.set(node->value - 1, 1);
                    (*it1).signs.set(node->value - 1,!wantedValue );
                    ++it1;
                }
            }
            return;
        }
    }
}


//-----------------ГЕНЕРАТОРЫ---------------//

//считает количество вхождений в строку переменных и операторов
int countVarsAndOperators(string str) {
    int answer = 0;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == 'a' || isOperator(str[i])) {
            answer += 1;
        }
    }
    return answer;
}

//функция возвращает случайно одну из подготовленных формул, являющуюся тождественным нулем или единицей.
string complicateConstant(int numOfVars, int numOfVarsTotal, bool constFlag) {
    string answer;

    switch (numOfVars) {
    case 3: {
        //заранее подготовленные комбинации, дающие 0 при всех наборах
        string variantsList[4] = { "(!(1^((!2+3)*!2+1)>3)*3)", "(!(((3+2*1)^!(3+2*1))>!(3*!2*2)))", "((((2+!1)*!3)v3)*!1)", "(!3*!(2+!(1+!3|1)|!(3^2))*!1)" };

        //подберём 3 случайные переменные из доступных
        string var1 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var2 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var3 = "a" + to_string(rand() % numOfVarsTotal + 1);

        //выберем случайно одну из формул усложнения
        int variant = rand() % 4;

        string buf = variantsList[variant];
        for (int i = 0; i < buf.size(); i++) {   //и заменим шаблонные переменные на настоящие 
            if (buf[i] > 48 && buf[i] < 58) {
                switch (buf[i]) {
                case '1': answer.append(var1); break;
                case '2': answer.append(var2); break;
                case '3': answer.append(var3); break;
                }
                continue;
            }
            answer.append(string(1, buf[i]));
        }

        if (constFlag) {    //если нужна тождественная единица, навесим отрицание
            answer = "!" + answer;
        }
        return answer;
    }break;

    case 4: {
        string variantsList[1] = { "(1*3*(3*((1^(2+3^4))^2*4)>!3)*2*3*!2)" };
        string var1 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var2 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var3 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var4 = "a" + to_string(rand() % numOfVarsTotal + 1);
        int variant = 0;
        string buf = variantsList[variant];

        for (int i = 0; i < buf.size(); i++) {
            if (buf[i] > 48 && buf[i] < 58) {
                switch (buf[i]) {
                case '1': answer.append(var1); break;
                case '2': answer.append(var2); break;
                case '3': answer.append(var3); break;
                case '4': answer.append(var4); break;
                }
                continue;
            }
            answer.append(string(1, buf[i]));
        }
        if (constFlag) {
            answer = "!" + answer;
        }
        return answer;
    }break;
    }
}

//проверяет, являются ли члены СКНФ одинаковыми по составу, нужна для удаления дубликатов для вывода в качестве ожидаемого ответа
bool areMembersEqual(vector<string> a, vector<string> b) {
    bool answer = true;
    int memberLength = a.size();
    for (int i = 0; i < memberLength; i++) {
        if (a[i] != b[i]) {
            answer = false;
        }
    }
    return answer;
}

//Генератор формул
//Принимает максимальное количество членов СКНФ (могут появиться дубликаты, которые будут удалены), количество переменных, количество отрицаний в СКНФ
//и приблизительное суммарное количество переменных и операторов

pair<vector<vector<string>>, string> newGeneratorSKNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //нельзя создать больше членов, чем возможно при таком количестве переменных
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //нельзя вставить больше отрицаний, чем всего вхождений переменных в СКНФ
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1) * (2 * numOfVars - 1)) {   //нельзя просить длину меньше самой СКНФ
        throw invalid_argument("Too short approxSize");
    }


    vector<vector<string>> sknf;    //в этот вектор поместим будущую скнф

    int negationsSet = 0;    //количество уже установленных знаков отрицания


    for (int i = 0; i < ceilNumOfMembers; i++) {
        vector<string> member;
        for (int j = 1; j <= numOfVars; j++) {

            member.push_back("a" + to_string(j));
            int needNegation = rand() % 5;
            if (needNegation == 1 && negationsSet < numOfNegations) {
                member[member.size() - 1].insert(0, 1, '!');
                negationsSet += 1;
            }
        }
        sknf.push_back(member);
    }

    while (negationsSet < numOfNegations) {    //если установлено недостаточно отрицаний - добавим пока не наберём
        bool isReady = false;

        for (int i = 0; i < sknf.size(); i++) {

            for (int j = 0; j < sknf[i].size(); j++) {
                if (sknf[i][j][0] != '!' && (rand() % 2)) {
                    negationsSet += 1;
                    if (negationsSet == numOfNegations) {
                        isReady = true;
                        break;
                    }
                    sknf[i][j].insert(0, 1, '!');
                }
            }
            if (isReady)
                break;
        }
    }


    //теперь нужно удалить все дубликаты из скнф чтобы предоставить её на выход. А использовать далее можно и с дубликатами
    vector<vector<string>> standartizedSKNF;

    for (int i = 0; i < sknf.size(); i++) {
        bool needToAdd = true;
        for (int j = 0; j < standartizedSKNF.size(); j++) {
            if (areMembersEqual(sknf[i], standartizedSKNF[j])) {
                needToAdd = false;
            }
        }
        if (needToAdd) {
            standartizedSKNF.push_back(sknf[i]);
        }
    }

    //такое количество переменных и операторов нужно докинуть,отнимем то, что уже занимает сама СНКФ.
    int needToAdd = approxSize - (2 * numOfVars - 1) * (2 * ceilNumOfMembers - 1) - negationsSet;

    //Это можно в принципе менять. Это распределение ожидаемого увеличения длины на каждом уровне.
    int onVarLevel = needToAdd / 4;
    int onMemberLevel = onVarLevel;
    int onFormulaLevel = needToAdd / 2;

    //усложнение на уровне переменных. К переменным добавляем формулы, являющиеся тождественными нулями или единицами. (x=x*1 or x=x+0)

    int perMember = onVarLevel / ceilNumOfMembers;  // такое колчество должен набрать каждый член в среднем

    for (int i = 0; i < sknf.size(); i++) {

        string ans;
        int toAdd = perMember;   //оставшееся количество символов, которое нужно набрать

        while (toAdd > perMember / 10) {   //пока не набрали хотя бы 90% от нужного, продолжаем

            for (int j = 0; j < sknf[i].size(); j++) {

                if (toAdd <= perMember / 10) {
                    break;
                }

                int needComplicate = rand() % numOfVars;

                if (needComplicate == 1) {

                    int zeroOrOne = rand() % 2;

                    if (zeroOrOne == 1) {
                        ans = complicateConstant(3 + rand() % 2, numOfVars, 1);
                        sknf[i][j] += "*" + ans;
                    }
                    else {
                        ans = complicateConstant(3 + rand() % 2, numOfVars, 0);
                        sknf[i][j] += "+" + ans;
                    }
                    toAdd -= countVarsAndOperators(ans) + 1;
                    sknf[i][j] = "(" + sknf[i][j] + ")";
                }
            }
        }
    }


    //теперь проведём усложнение на уровне члена СКНФ. Будем запутывать переменные.

    //генератор случайных чисел, нужен для перемешивания элементов между собой
    random_device rd;
    default_random_engine rng(rd());

    int onLevel2 = onMemberLevel / ceilNumOfMembers;    //столько должнен набрать каждый член СКНФ

    for (int i = 0; i < sknf.size(); i++) {


        int toAdd = onLevel2;

        while (toAdd > onLevel2 / 10 && sknf[i].size() != 1) {

            shuffle(sknf[i].begin(), sknf[i].end(), rng);   //запутаем элементы между собой

            string newMember;
            int lastIndex = sknf[i].size() - 1;

            //Применяется равенство x+y=x^y^x*y 

            newMember = "(" + sknf[i][0] + "^" + sknf[i][lastIndex] + "^" + sknf[i][0] + "*" + sknf[i][lastIndex] + ")";
            toAdd -= countVarsAndOperators(sknf[i][0]) + countVarsAndOperators(sknf[i][lastIndex]) + 3;
            sknf[i].pop_back();
            sknf[i].erase(sknf[i].begin());
            sknf[i].push_back(newMember);
        }
    }

    //теперь усложнённые члены соединим в строки
    vector <string> compMembSKNF;
    for (int i = 0; i < sknf.size(); i++) {
        string buf = "(";
        for (int j = 0; j < sknf[i].size(); j++) {
            buf.append(sknf[i][j]);
            buf += "+";
        }
        buf[buf.size() - 1] = ')';
        compMembSKNF.push_back(buf);
    }


    //теперь проведём усложнение на уровне формулы, запутывая члены СКНФ. Между членами стоит *

    int toAdd = onFormulaLevel;  //осталось добрать

    //тут используется равенство x*y = (x+y)*(!x+y)*(x+!y)
    while (toAdd > onFormulaLevel / 10 && compMembSKNF.size() != 1) {
        shuffle(compMembSKNF.begin(), compMembSKNF.end(), rng);
        string buf;
        int lastIndex = compMembSKNF.size() - 1;
        buf = "((" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(!" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(" + compMembSKNF[0] + "+!" + compMembSKNF[lastIndex] + "))";
        toAdd -= 2 * countVarsAndOperators(compMembSKNF[0]) + 2 * countVarsAndOperators(compMembSKNF[lastIndex]) + 7;
        compMembSKNF.pop_back();
        compMembSKNF.erase(compMembSKNF.begin());
        compMembSKNF.push_back(buf);
    }

    //наконец, соединим всё в одну строку
    string answer;

    for (int i = 0; i < compMembSKNF.size(); i++) {
        answer += compMembSKNF[i];
        answer += "*";
    }
    answer.pop_back();

    return make_pair(standartizedSKNF, answer);
}

//Генератор формул
//Принимает максимальное количество членов СДНФ (могут появиться дубликаты, которые будут удалены), количество переменных, количество отрицаний в СДНФ
//и приблизительное суммарное количество переменных и операторов
pair<vector<vector<string>>, string> newGeneratorSDNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //нельзя создать больше членов, чем возможно при таком количестве переменных
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //нельзя вставить больше отрицаний, чем всего вхождений переменных в СКНФ
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1) * (2 * numOfVars - 1)) {   //нельзя просить длину меньше самой СКНФ
        throw invalid_argument("Too short approxSize");
    }


    vector<vector<string>> sdnf;    //в этот вектор поместим будущую скнф

    int negationsSet = 0;    //количество уже установленных знаков отрицания


    for (int i = 0; i < ceilNumOfMembers; i++) {
        vector<string> member;
        for (int j = 1; j <= numOfVars; j++) {

            member.push_back("a" + to_string(j));
            int needNegation = rand() % 5;
            if (needNegation == 1 && negationsSet < numOfNegations) {
                member[member.size() - 1].insert(0, 1, '!');
                negationsSet += 1;
            }
        }
        sdnf.push_back(member);
    }

    while (negationsSet < numOfNegations) {    //если установлено недостаточно отрицаний - добавим пока не наберём
        bool isReady = false;

        for (int i = 0; i < sdnf.size(); i++) {

            for (int j = 0; j < sdnf[i].size(); j++) {
                if (sdnf[i][j][0] != '!' && (rand() % 2)) {
                    negationsSet += 1;
                    if (negationsSet == numOfNegations) {
                        isReady = true;
                        break;
                    }
                    sdnf[i][j].insert(0, 1, '!');
                }
            }
            if (isReady)
                break;
        }
    }


    //теперь нужно удалить все дубликаты из скнф чтобы предоставить её на выход. А использовать далее можно и с дубликатами
    vector<vector<string>> standartizedSDNF;

    for (int i = 0; i < sdnf.size(); i++) {
        bool needToAdd = true;
        for (int j = 0; j < standartizedSDNF.size(); j++) {
            if (areMembersEqual(sdnf[i], standartizedSDNF[j])) {
                needToAdd = false;
            }
        }
        if (needToAdd) {
            standartizedSDNF.push_back(sdnf[i]);
        }
    }

    //такое количество переменных и операторов нужно докинуть,отнимем то, что уже занимает сама СНКФ.
    int needToAdd = approxSize - (2 * numOfVars - 1) * (2 * ceilNumOfMembers - 1) - negationsSet;

    //Это можно в принципе менять. Это распределение ожидаемого увеличения длины на каждом уровне.
    int onVarLevel = needToAdd / 4;
    int onMemberLevel = onVarLevel;
    int onFormulaLevel = needToAdd / 2;

    //усложнение на уровне переменных. К переменным добавляем формулы, являющиеся тождественными нулями или единицами. (x=x*1 or x=x+0)

    int perMember = onVarLevel / ceilNumOfMembers;  // такое колчество должен набрать каждый член в среднем

    for (int i = 0; i < sdnf.size(); i++) {

        string ans;
        int toAdd = perMember;   //оставшееся количество символов, которое нужно набрать

        while (toAdd > perMember / 10) {   //пока не набрали хотя бы 90% от нужного, продолжаем

            for (int j = 0; j < sdnf[i].size(); j++) {

                if (toAdd <= perMember / 10) {
                    break;
                }

                int needComplicate = rand() % numOfVars;

                if (needComplicate == 1) {

                    int zeroOrOne = rand() % 2;

                    if (zeroOrOne == 1) {
                        ans = complicateConstant(3 + rand() % 2, numOfVars, 1);
                        sdnf[i][j] += "*" + ans;
                    }
                    else {
                        ans = complicateConstant(3 + rand() % 2, numOfVars, 0);
                        sdnf[i][j] += "+" + ans;
                    }
                    toAdd -= countVarsAndOperators(ans) + 1;
                    sdnf[i][j] = "(" + sdnf[i][j] + ")";
                }
            }
        }
    }


    //теперь проведём усложнение на уровне члена СДНФ. Будем запутывать переменные.

    //генератор случайных чисел, нужен для перемешивания элементов между собой
    random_device rd;
    default_random_engine rng(rd());

    int onLevel2 = onMemberLevel / ceilNumOfMembers;    //столько должнен набрать каждый член СДНФ

    for (int i = 0; i < sdnf.size(); i++) {


        int toAdd = onLevel2;

        while (toAdd > onLevel2 / 10 && sdnf[i].size() != 1) {

            shuffle(sdnf[i].begin(), sdnf[i].end(), rng);   //запутаем элементы между собой

            string newMember;
            int lastIndex = sdnf[i].size() - 1;

            
            //тут используется равенство x*y = (x+y)*(!x+y)*(x+!y)
            newMember = "((" + sdnf[i][0] + "+" + sdnf[i][lastIndex] + ")*(!(" + sdnf[i][0] + ")+" + sdnf[i][lastIndex] + ")*(" + sdnf[i][0] + "+!(" + sdnf[i][lastIndex] + ")))";
            //newMember = "(" + sknf[i][0] + "^" + sknf[i][lastIndex] + "^" + sknf[i][0] + "*" + sknf[i][lastIndex] + ")";
            toAdd -= countVarsAndOperators(sdnf[i][0]) + countVarsAndOperators(sdnf[i][lastIndex]) + 7;
            sdnf[i].pop_back();
            sdnf[i].erase(sdnf[i].begin());
            sdnf[i].push_back(newMember);
        }
    }

    //теперь усложнённые члены соединим в строки
    vector <string> compMembSDNF;
    for (int i = 0; i < sdnf.size(); i++) {
        string buf = "(";
        for (int j = 0; j < sdnf[i].size(); j++) {
            buf.append(sdnf[i][j]);
            buf += "*";
        }
        buf[buf.size() - 1] = ')';
        compMembSDNF.push_back(buf);
    }


    //теперь проведём усложнение на уровне формулы, запутывая члены СДНФ. Между членами стоит +

    int toAdd = onFormulaLevel;  //осталось добрать

    
     //Применяется равенство x+y=x^y^x*y 
    while (toAdd > onFormulaLevel / 10 && compMembSDNF.size() != 1) {
        shuffle(compMembSDNF.begin(), compMembSDNF.end(), rng);
        string buf;
        int lastIndex = compMembSDNF.size() - 1;
        buf = "(" + compMembSDNF[0] + "^" + compMembSDNF[lastIndex] + "^" + compMembSDNF[0] + "*" + compMembSDNF[lastIndex] + ")";
        //buf = "((" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(!" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(" + compMembSKNF[0] + "+!" + compMembSKNF[lastIndex] + "))";
        toAdd -= 2 * countVarsAndOperators(compMembSDNF[0]) + 2 * countVarsAndOperators(compMembSDNF[lastIndex]) + 3;
        compMembSDNF.pop_back();
        compMembSDNF.erase(compMembSDNF.begin());
        compMembSDNF.push_back(buf);
    }

    //наконец, соединим всё в одну строку
    string answer;

    for (int i = 0; i < compMembSDNF.size(); i++) {
        answer += compMembSDNF[i];
        answer += "+";
    }
    answer.pop_back();

    return make_pair(standartizedSDNF, answer);
}

//-----------ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (сравнение ответов, вывод на экран и в файл)______//

list<list<short>> listOfBitsetsToListOfShorts(list<sknfMember> lst) {
    list<list<short>> answer;

    for (auto it1 = lst.begin(); it1 != lst.end(); it1++) {

        list<short> buf;
        for (int j = 0; j < MAX_VARS; j++) {
            if ((*it1).vars[j] == 1) {
                short bufVar = 0;
                if ((*it1).signs[j] == 0) {
                    bufVar = j + 1;
                }
                else {
                    bufVar = -(j + 1);
                }
                buf.push_back(bufVar);

            }
        }
        answer.push_back(buf);
    }
    return answer;
}

void printWantedAnswer(vector<vector<string>> vec) {
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec[i].size(); j++) {
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void printActualAnswerSKNF(list<sknfMember> ans) {
    for (auto it1 = ans.begin(); it1 != ans.end(); it1++) {
        
        for (int i = 0; i < MAX_VARS; i++) {
            if ((*it1).vars.test(i)) {
                
                
                if ((*it1).signs.test(i)) {
                    cout << "!";
                }
                cout << "a" << i + 1 << " ";
            }
        }
        cout << endl;
        
    }
}

void printActualAnswerSDNF(list<sknfMember> ans) {
    for (auto it1 = ans.begin(); it1 != ans.end(); it1++) {
        
        for (int i = 0; i < MAX_VARS; i++) {
            if ((*it1).vars.test(i)) {
               
                if ((*it1).signs.test(i)) {
                    cout << "!";
                }
                cout << "a" << i + 1 <<" ";
            }
        }
        cout << endl;
        
    }
}

void printSDNFInFile(list<sknfMember> ans,string exception)
{
    ofstream fout;
    fout.open("SDNF.txt");
    fout << "Используемые обозначения:\n" <<
        "! - отрицание\n" <<
        "+ - дизъюнкция\n" <<
        "* - конъюнкция\n" <<
        "> - импликация\n" <<
        "= - эквивалентность\n" <<
        "v - стрелка Пирса\n" <<
        "| - штрих Шеффера\n" <<
        "Формат названия переменных: a1, a2, ... , an\n";
    fout << "Формула:\n";
    for (int j = 0; j < exception.size(); ++j)
    {
        fout << exception[j];
        if (j%110 == 0 && j>1)
        {
            fout << endl;
        }
    }
    fout << "\nСДНФ:\n";
    for (auto it1 = ans.begin(); it1 != ans.end(); it1++) {
        fout << "(";
        for (int i = 0; i < MAX_VARS; i++) {
            if ((*it1).vars.test(i)) {
                if ((*it1).signs.test(i)) {
                    fout << "!";
                }
                if ((i) != 0)
                {
                    fout << "*";
                }
                fout << "a" << i + 1;
            }
        }
        fout << ")";
        if (std::next(it1) != ans.end())
        {
            fout << "+";
        }
    }
    fout.close();
}

void printSKNFInFile(list<sknfMember> ans, string exception)
{   
    ofstream fout;
    fout.open("SKNF.txt");
    fout << "Используемые обозначения:\n" <<
        "! - отрицание\n" <<
        "+ - дизъюнкция\n" <<
        "* - конъюнкция\n" <<
        "> - импликация\n" <<
        "= - эквивалентность\n" <<
        "v - стрелка Пирса\n" <<
        "| - штрих Шеффера\n" <<
        "Формат названия переменных: a1, a2, ... , an\n";
    fout << "Формула:\n";
    for (int j = 0; j < exception.size(); ++j)
    {
        fout << exception[j];
        if (j%110 == 1 && j >1)
        {
            fout << endl;
        }
    }
    
    fout << "\nСКНФ:\n";
    for (auto it1 = ans.begin(); it1 != ans.end(); it1++) {
        fout << "(";
        for (int i = 0; i < MAX_VARS; i++) {
            if ((*it1).vars.test(i)) {
                if ((*it1).signs.test(i)) {
                    fout << "!";
                }
                if ((i) != 0)
                {
                    fout << "+";
                }
                fout << "a" << i + 1;
                
            }
        }
        fout << ")";
        if (std::next(it1) != ans.end())
        {
            fout << "*";
        }
    }
    fout.close();
}

bool compareAnswers(list<list<short>> actualAns, vector<vector<string>> wantedAns) {

    if (actualAns.size() != wantedAns.size() || (*actualAns.begin()).size() != wantedAns[0].size())
        return false;
    vector<vector<string>> actualAnsVector;

    for (auto it1 = actualAns.begin(); it1 != actualAns.end(); it1++) {

        vector<string> buf;
        for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
            string bufVar = "";
            if ((*it2) < 0)
                bufVar += "!";
            bufVar += "a" + to_string(abs(*it2));
            buf.push_back(bufVar);
        }
        actualAnsVector.push_back(buf);
    }


    for (int i = 0; i < actualAnsVector.size(); i++) {
        bool found = false;

        for (int j = 0; j < wantedAns.size(); j++) {
            bool hasDiff = false;
            for (int k = 0; k < wantedAns[j].size(); k++) {
                if (wantedAns[j][k] != actualAnsVector[i][k]) {
                    hasDiff = true;
                    break;
                }
            }
            if (!hasDiff) {
                found = true;
                break;
            }

        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool checkUserInput(string str) {
    if (!checkBraces(str))
        return false;

    for (int i = 0; i < str.size(); i++) {
        if (!isOperator(str[i]) && str[i] != 'a' && str[i] != '(' && str[i] != ')' && !isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

void rpnPrintActualAnswer(vector<bitset<NEW_ALGO_MAX_VARS>> &vec, int flag) {
    bool valueWithNO = 1;
    if (flag == FLAG_SKNF) {
        valueWithNO = 1;
    }
    else if (flag == FLAG_SDNF) {
        valueWithNO = 0;
    }
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < numOfVars; j++) {
            if (vec[i].test(j) == valueWithNO)
                cout << "!";
                cout << "a" << j + 1 << " ";
        }
        cout << endl;
    }
}

void rpnPrintInFile(vector<bitset<NEW_ALGO_MAX_VARS>> &vec, string &expression, int flag) {
    ofstream fout;
    fout.open("SKNF.txt");
    fout << "Используемые обозначения:\n" <<
        "! - отрицание\n" <<
        "+ - дизъюнкция\n" <<
        "* - конъюнкция\n" <<
        "> - импликация\n" <<
        "= - эквивалентность\n" <<
        "v - стрелка Пирса\n" <<
        "| - штрих Шеффера\n" <<
        "Формат названия переменных: a1, a2, ... , an\n";
    fout << "Формула:\n";
    for (int j = 0; j < expression.size(); ++j)
    {
        fout << expression[j];
        if (j % 110 == 1 && j > 1)
        {
            fout << endl;
        }
    }

    if (flag == FLAG_SKNF) {
        fout << "\nСКНФ\n";
    }
    else if (flag == FLAG_SDNF) {
        fout << "\nСДНФ\n";
    }
    bool valueWithNO = 1;
    if (flag == FLAG_SKNF) {
        valueWithNO = 1;
    }
    else if (flag == FLAG_SDNF) {
        valueWithNO = 0;
    }
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < numOfVars; j++) {
            if (vec[i].test(j) == valueWithNO)
                fout << "!";
            fout << "a" << j + 1 << " ";
        }
        fout << endl;
    }
    fout.close();
}

//------------------АЛГОРИТМ_НА_ОПЗ-----------------// 

//Reverse Polish Notation - обратная польская запись
//вектор для хранения обратной польской записи (ОПЗ) выражения, ОПЗ получим из дерева
vector<short> rpn;

//совершает обратный обход дерева, получая ОПЗ
void postOrderTravers(node *node) {
    if (node == nullptr)
        return;
    postOrderTravers(node->left);
    postOrderTravers(node->right);
    rpn.push_back(node->value);
}

//прибавляет единицу к битовой шкале (как к двочному числу)
void incBitSet(bitset<NEW_ALGO_MAX_VARS> &bitset) {
    for (int i = 0; i < NEW_ALGO_MAX_VARS; i++) {
        if (!bitset.test(i)) {
            bitset.set(i, 1);
            for (int j = 0; j < i; j++) {
                bitset.set(j, 0);
            }
            break;
        }
    }
}

//функция поиска СКНФ (СДНФ)
vector<bitset<NEW_ALGO_MAX_VARS>> rpnCalcutator(node* node, int flag) {
    bool toFind = 0;
    if (flag == FLAG_SDNF) {
        toFind = 1;
    }
    else if (flag == FLAG_SKNF) {
        toFind = 0;
    }
   

    int bitToCheck = numOfVars;   //так как битовые шкалы статические (имеют избыточный размер), ограничим используемую часть битовой шкалы нашим количеством переменных
    vector <bitset<NEW_ALGO_MAX_VARS>> ans; //сюда поместим ответ

    bitset<NEW_ALGO_MAX_VARS> curSet(0);       //шкала текущей комбинации

    bitset<NEW_ALGO_STACK_LIMIT> stack(0);     //шкала, исполняющая роль стека
    int topIndex;  //индекс первого пустого элемента в стеке

    postOrderTravers(node); //найдём обратную польскую запись

    while (!curSet.test(bitToCheck)) {  // если мы имеем n переменных, используются биты шкалы с номерами 0 - (n-1). Когда бит с номером n станет равным 1, все наборы из интересующих нас битов были рассмотрены
        topIndex = 0;   //"очистим" стек. На самом деле информация о первом пустом элементе позволяет не очищать ненужные элементы стека, можно просто перезаписывать их.

        for (int i = 0; i < rpn.size(); i++) {   //цикл по обратной польской записи формулы

            if (rpn[i] > 0) {  //если попали в переменную

                stack.set(topIndex, curSet.test(rpn[i] - 1));   //поместим на стек её значение в текущем рассматриваемом наборе
                topIndex += 1;  //изменим индекс вершушки стека (ведь добавили один элемент)
            }
            else {                 //в противном случае перед нами оператор
                switch (rpn[i]) {
                case -1:   //отрицание
                {
                    //stack.set(topIndex - 1, !stack.test(topIndex - 1));
                    stack.flip(topIndex-1);    //инвертируем последний элемент стека
                }; break;

                case -2:  // конъюнкция
                {
                    stack.set(topIndex - 2, stack.test(topIndex - 2) && stack.test(topIndex - 1));
                    topIndex -= 1;
                }; break;
                
                case -3: // дизъюнкция
                {
                    stack.set(topIndex - 2, stack.test(topIndex - 2) || stack.test(topIndex - 1));
                    topIndex -= 1;
                }; break;

                case -4: // XOR (исключающее ИЛИ)
                {
                    stack.set(topIndex - 2, stack.test(topIndex - 2) ^ stack.test(topIndex - 1));
                    topIndex -= 1;
                }; break;

                case -5: // импликация
                {
                    stack.set(topIndex - 2, !stack.test(topIndex - 2) || stack.test(topIndex - 1));
                    topIndex -= 1;
                }; break;

                case -6: //эквиваленция
                {
                    stack.set(topIndex - 2, stack.test(topIndex - 2) == stack.test(topIndex - 1));
                    topIndex -= 1;
                }; break;

                case -7: //штрих Шеффера (НЕ-И)
                {
                    stack.set(topIndex - 2, !(stack.test(topIndex - 2) && stack.test(topIndex - 1)));
                    topIndex -= 1;
                }; break;

                case -8: //стрелка Пирса (НЕ-ИЛИ)
                {
                    stack.set(topIndex - 2, !(stack.test(topIndex - 2) || stack.test(topIndex - 1)));
                    topIndex -= 1;
                }; break;
                }
            }
        }

        if (stack.test(0) == toFind) {   //если формула на данном наборе дала 0 (1) - помещаем набор в СКНФ (СДНФ)
            ans.push_back(curSet);
        }
        incBitSet(curSet);
    }

    return ans;
}

void printAnswerHint() {
    cout << "\nО выводе ответа:\n" <<
        "Каждая строка ответа - один член СКНФ (СДНФ)\n" <<
        "Знаки между членами и переменными опущены для упрощения чтения.\nЕсли ищем СКНФ, то между переменными дизъюнкция (+), между членами - конъюнкция (*).\nЕсли ищем СДНФ - наоборот.\n";
}
int main() {
    setlocale(LC_ALL, "ru");
    char choice;
    cout << "Выберите источник входных данных:\n";
    cout << "1.Использовать генератор;\n";
    cout << "2.Ввести формулу с клавиатуры;\n";
    printAnswerHint();
    cin >> choice;
    while (choice != '1' && choice != '2') {
    cout << "Такого пункта нет. Введите 1 или 2\n";
    cin >> choice;
    }  
    switch (choice) {   //выбор источника ввода
    case '1':    //используя генератор
    {
        srand(time(NULL));
        int numOfMembers = -1;
        int numOfVariables = -1;
        int numOfNegations = -1;
        int approxSize = -1;
        bool everythingFine = true;

        cout << "Введите параметры формулы:\n" <<
            "1.К-во членов* СДНФ (СДНФ);\n" <<
            "2.К-во уникальных переменных в формуле;\n" <<
            "3.К-во отрицаний у переменных в итоговой СДНФ (СКНФ);\n" <<
            "4.Примерное количество вхождений переменных и операторов в формулу (их сумма).\n" <<
            "* - при генерации могут возникнуть одинаковые комбинации. Дубликаты существующих комбинаций будут удалены перед показом ожидаемого ответа.\n" <<
            "Формат ввода: <к-во членов> <к-во переменных> <к-во отрицаний> <желаемое к-во переменных и операторов>\n";

        do {
            cin >> numOfMembers;
            cin >> numOfVariables;
            cin >> numOfNegations;
            cin >> approxSize;

            if (numOfMembers <= 0 || numOfVariables <=0 || numOfNegations <=0 || approxSize <= 0) {
                cout << "Ни один из параметров не может быть отрицательным или равняться нулю.\n";
                everythingFine = false;
            }
            else if (numOfNegations > numOfMembers * numOfVariables) {
                cout << "Количество отрицаний привышает допустимое. Отрицаний в СКНФ (СДНФ) не может быть больше, чем (к-во переменных)*(к-во членов)\n";
                everythingFine = false;
            }
            else if (approxSize < (2 * numOfVariables - 1) * (2 * numOfMembers - 1)) {
                cout << "Желаемое к-во переменных и опеаторов в формуле меньше допустимого в данном случае. Генератор усложняет СКНФ (СДНФ). Её длина - минимальная допустимая.\n" <<
                    "Формула, по которой выполняется проверка: (2 * <к-во переменных> - 1)*(2 * <к-во членов> - 1)\n";
                everythingFine = false;
            }
            if (!everythingFine) {
                cout << "Пожалуйста, повторите ввод.\n";
            }
        } while (!everythingFine);

        cout << "Какую форму вы хотите найти:\n";
        cout << "1.СКНФ;\n";
        cout << "2.СДНФ;\n";
        cin >> choice;
        while (choice != '1' && choice != '2' && choice != '3') {
            cout << "Такого пункта нет. Введите 1 или 2\n";
            cin >> choice;
        }
        pair <vector<vector<string>>, string> answer;
        switch (choice) {    //выбор формы
        case '1':
        {
           answer = newGeneratorSKNF(numOfMembers, numOfVariables, numOfNegations, approxSize);
           cout << "Формула сгенерирована.\n";
           cout << "Ожидаемый ответ:\n";
           printWantedAnswer(answer.first);
           cout << "Проверка правильности скобочной конструкции: " << checkBraces(answer.second) << endl;
           cout << "Вхождений операторов и переменных в формуле: " << countVarsAndOperators(answer.second) << endl;
           cout << "Количество символов в строке: " << answer.second.size() << endl;
           cout << "Показать формулу?(1/0)\n";
           cin >> choice;
           while (choice != '0' && choice != '1') {
               cout << "Введите 1 или 0.\n";
               cin >> choice;
           }
           if (choice == '1') {
               cout << "Формула:\n";
               cout << answer.second << endl;
           }
           cout << "Какой алгоритм использовать?\n";
           cout << "1.Основанный на рекурсии\n";
           cout << "2.Основанный на обратной польской записи\n";
           cin >> choice;
           while (choice != '1' && choice != '2' && choice != '3') {
               cout << "Такого пункта нет. Введите 1 или 2\n";
               cin >> choice;
           }
           switch (choice) { //выбор алгоритма
           case '1':
           {
               list<sknfMember> resultSKNF;
               sknfMember buf;
               buf.signs.set(0, 1);
               resultSKNF.push_back(buf);
               node* root = new node;
               cout << "Начинается поиск.\n";
               std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
               addnode(answer.second, root);
               cout << "Дерево построено\n";
               distToLeaf(root);
               cout << "Рёбра отмечены\n";
               sknfSearch(0, resultSKNF, root);
               std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
               cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
               cout << "Равны ли ответы: " << compareAnswers(listOfBitsetsToListOfShorts(resultSKNF), answer.first) << endl;
               cout << "Куда вывести ответ?\n" <<
                   "1.На экран;\n" <<
                   "2.В файл;\n" <<
                   "3.На экран и в файл;\n";
               cin >> choice;
               while (choice != '1' && choice != '2' && choice != '3') {
                   cout << "Введите 1, 2 или 3.\n";
                   cin >> choice;
               }
               if (choice == '1') {
                   printActualAnswerSKNF(resultSKNF);
               }
               if (choice == '2') {
                   printSKNFInFile(resultSKNF,answer.second);
               }
               if (choice == '3') {
                   printActualAnswerSKNF(resultSKNF);
                   printSKNFInFile(resultSKNF, answer.second);
               }
           }; break;
           case '2':
           {
               node* root = new node;
               cout << "Начинается поиск.\n";
               std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
               addnode(answer.second, root);
               cout << "Дерево построено\n";
               vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SKNF);
               std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
               cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
               list<list<short>> buf;
               for (int i = 0; i < ans.size(); i++) {
                   list<short> buf2;
                   for (int j = 0; j < numOfVars; j++) {
                       ans[i].test(j) ? buf2.push_back(-(j + 1)) : buf2.push_back(j + 1);
                   }
                   buf.push_back(buf2);
               }
               cout << "Равны ли ответы: ";
               cout << compareAnswers(buf, answer.first) << endl;
               cout << "Куда вывести ответ?\n" <<
                   "1.На экран;\n" <<
                   "2.В файл;\n" <<
                   "3.На экран и в файл;\n";
               cin >> choice;
               while (choice != '1' && choice != '2' && choice != '3') {
                   cout << "Введите 1, 2 или 3.\n";
                   cin >> choice;
               }
               if (choice == '1') {
                   rpnPrintActualAnswer(ans, FLAG_SKNF);
               }
               if (choice == '2') {
                   rpnPrintInFile(ans, answer.second, FLAG_SKNF);
               }
               if (choice == '3') {
                   rpnPrintActualAnswer(ans, FLAG_SKNF);
                   rpnPrintInFile(ans, answer.second, FLAG_SKNF);
               }
           }; break;
           }
        }; break;
        case '2':
        {
            answer = newGeneratorSDNF(numOfMembers, numOfVariables, numOfNegations, approxSize);
            cout << "Формула сгенерирована.\n";
            cout << "Ожидаемый ответ:\n";
            printWantedAnswer(answer.first);
            cout << "Проверка правильности скобочной конструкции: " << checkBraces(answer.second) << endl;
            cout << "Вхождений операторов и переменных в формуле: " << countVarsAndOperators(answer.second) << endl;
            cout << "Количество символов в строке: " << answer.second.size() << endl;
            cout << "Показать формулу?(1/0)\n";
            cin >> choice;
            while (choice != '0' && choice != '1') {
                cout << "Введите 1 или 0.\n";
                cin >> choice;
            }
            if (choice == '1') {
                cout << "Формула:\n";
                cout << answer.second << endl;
            }

            cout << "Какой алгоритм использовать?\n";
            cout << "1.Основанный на рекурсии\n";
            cout << "2.Основанный на обратной польской записи\n";
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '3') {
                cout << "Такого пункта нет. Введите 1 или 2\n";
                cin >> choice;
            }

            switch (choice) { //выбор алгоритма
            case '1':
            {
                list<sknfMember> resultSDNF;
                sknfMember buf;
                buf.signs.set(0, 1);
                resultSDNF.push_back(buf);
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(answer.second, root);
                cout << "Дерево построено\n";
                distToLeaf(root);
                cout << "Рёбра отмечены\n";
                sdnfSearch(1, resultSDNF, root);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Равны ли ответы: " << compareAnswers(listOfBitsetsToListOfShorts(resultSDNF), answer.first) << endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {

                    printActualAnswerSKNF(resultSDNF);
                }
                if (choice == '2') {
                    printSKNFInFile(resultSDNF, answer.second);
                }
                if (choice == '3') {
                    printActualAnswerSKNF(resultSDNF);
                    printSKNFInFile(resultSDNF, answer.second);
                }
            }; break;
            case '2':
            {
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(answer.second, root);
                cout << "Дерево построено\n";
                vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SDNF);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                list<list<short>> buf;
                for (int i = 0; i < ans.size(); i++) {
                    list<short> buf2;
                    for (int j = 0; j < numOfVars; j++) {
                        ans[i].test(j) ? buf2.push_back(j + 1) : buf2.push_back(-(j + 1));
                    }
                    buf.push_back(buf2);
                }
                cout << "Равны ли ответы: ";
                cout << compareAnswers(buf, answer.first) << endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    rpnPrintActualAnswer(ans, FLAG_SDNF);
                }
                if (choice == '2') {
                    rpnPrintInFile(ans, answer.second, FLAG_SDNF);
                }
                if (choice == '3') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                    rpnPrintInFile(ans, answer.second, FLAG_SDNF);
                }
            }; break;
            }
        }; break;
        }

    }; break;
    case '2':    //ввод с клавиатуры
    {
        cout << "Используемые обозначения:\n" <<
            "! - отрицание\n" <<
            "+ - дизъюнкция\n" <<
            "* - конъюнкция\n" <<
            "> - импликация\n" <<
            "= - эквивалентность\n" <<
            "v - стрелка Пирса\n" <<
            "| - штрих Шеффера\n" <<
            "Формат названия переменных: a1, a2, ... , an\n" <<
            "Скобки допускаются. Отрицание может стоять как перед переменной, так и перед скобками.\n";

        cout << "Введите вашу формулу:\n";
        string calculate;
        cin >> calculate;
        while (!checkUserInput(calculate)) {
            cout << "Вы ввели некорректное выражение. Пожалуйста, проверьте его и исправьте ошибки.\n";
            cin >> calculate;
        }

        cout << "Какую форму вы хотите найти:\n";
        cout << "1.СКНФ;\n";
        cout << "2.СДНФ;\n";
        cin >> choice;
        while (choice != '1' && choice != '2') {
            cout << "Такого пункта нет. Введите 1 или 2\n";
            cin >> choice;
        }
        switch (choice) {  //выбор формы
        case '1':
        {
            cout << "Какой алгоритм использовать?\n";
            cout << "1.Основанный на рекурсии\n";
            cout << "2.Основанный на обратной польской записи\n";
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '3') {
                cout << "Такого пункта нет. Введите 1 или 2\n";
                cin >> choice;
            }
            
            switch (choice) {
            case '1':
            {
                list<sknfMember> resultSKNF;
                sknfMember buf;
                buf.signs.set(0, 1);
                resultSKNF.push_back(buf);
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                distToLeaf(root);
                cout << "Рёбра отмечены\n";
                sknfSearch(0, resultSKNF, root);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    printActualAnswerSKNF(resultSKNF);
                }
                if (choice == '2') {
                    printSKNFInFile(resultSKNF, calculate);
                }
                if (choice == '3') {
                    printActualAnswerSKNF(resultSKNF);
                    printSKNFInFile(resultSKNF, calculate);
                }
            }; break;
            case '2':
            {
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SKNF);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                }
                if (choice == '2') {
                    rpnPrintInFile(ans, calculate, FLAG_SKNF);
                }
                if (choice == '3') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                    rpnPrintInFile(ans, calculate, FLAG_SKNF);
                }
            }; break;
            }


        }; break;
        case '2':
        {
            cout << "Какой алгоритм использовать?\n";
            cout << "1.Основанный на рекурсии\n";
            cout << "2.Основанный на обратной польской записи\n";
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '3') {
                cout << "Такого пункта нет. Введите 1 или 2\n";
                cin >> choice;
            }
            switch (choice) {
            case '1':
            {
                list<sknfMember> resultSDNF;
                sknfMember buf;
                buf.signs.set(0, 1);
                resultSDNF.push_back(buf);
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                distToLeaf(root);
                cout << "Рёбра отмечены\n";
                sdnfSearch(1, resultSDNF, root);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    printActualAnswerSKNF(resultSDNF);
                }
                if (choice == '2') {
                    printSKNFInFile(resultSDNF, calculate);
                }
                if (choice == '3') {
                    printActualAnswerSKNF(resultSDNF);
                    printSKNFInFile(resultSDNF, calculate);
                }
            }; break;
            case '2':
            {
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SDNF);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    rpnPrintActualAnswer(ans, FLAG_SDNF);
                }
                if (choice == '2') {
                    rpnPrintInFile(ans, calculate, FLAG_SDNF);
                }
                if (choice == '3') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                    rpnPrintInFile(ans, calculate, FLAG_SDNF);
                }
            }; break;
            }
        }; break;
        }
    }; break;
    }
    cout << "Работа программы завершена.\n";
    return 0;
}