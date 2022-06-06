#include "OPZalgorithm.h"
//------------------АЛГОРИТМ_НА_ОПЗ-----------------// 

//Reverse Polish Notation - обратная польская запись
//вектор для хранения обратной польской записи (ОПЗ) выражения, ОПЗ получим из дерева
vector<short> rpn;
extern int numOfVars;   //хранит количество переменных в формуле, нужно для ограничения рабочей части битовой шкалы калькулятора ОПЗ
//совершает обратный обход дерева, получая ОПЗ
void postOrderTravers(node* node) {
    if (node == nullptr)
        return;
    postOrderTravers(node->left);
    postOrderTravers(node->right);
    rpn.push_back(node->value);
}

//прибавляет единицу к битовой шкале (как к двочному числу)
void incBitSet(bitset<NEW_ALGO_MAX_VARS>& bitset) {
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
                    stack.flip(topIndex - 1);    //инвертируем последний элемент стека
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