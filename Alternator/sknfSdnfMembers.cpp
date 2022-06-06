#include "sknfSdnfMembers.h"

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
                    if ((*it1).signs.test(node->value - 1) != !wantedValue) {
                        lst.erase(it1++);
                    }
                    else {
                        ++it1;
                    }
                }
                else {
                    (*it1).vars.set(node->value - 1, 1);
                    (*it1).signs.set(node->value - 1, !wantedValue);
                    ++it1;
                }
            }
            return;
        }
    }
}

