#include <iostream>
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

using namespace std;


#define MAX_VARS 8     //максимальное количество переменных в формуле. МОЖНО РЕДАКТИРОВАТЬ (C++ не имеет встроенных динамических битовых шкал, сторонние работают хуже.
                       //Если удастся найти хорошую реализацию динамических шкал, программа будет доработана с их использованием. Это лишь тонкость реализации.

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
int distToLeaf(node *node) {

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
void removeOuterBraces(string &str) {
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
char priorityArray[8] = { 'v', '|', '=', '>', '^', '+', '*', '!'};
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
void sknfSearch(bool wantedValue, list<sknfMember> &lst, node* node) {

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
        string variantsList[4] = { "(!(1^((!2+3)*!2+1)>3)*3)", "(!(((3+2*1)^!(3+2*1))>!(3*!2*2)))", "((((2+!1)*!3)v3)*!1)", "(!3*!(2+!(1+!3|1)|!(3^2))*!1)"};

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

pair<vector<vector<string>>, string> newGenerator(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //нельзя создать больше членов, чем возможно при таком количестве переменных
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //нельзя вставить больше отрицаний, чем всего вхождений переменных в СКНФ
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1)*(2 * numOfVars - 1)) {   //нельзя просить длину меньше самой СКНФ
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
    while (toAdd > onFormulaLevel / 10 && compMembSKNF.size()!=1) {
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


//сравнивает ожидаемый ответ и тот, который получила функция
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

//печатает на экран ожидаемый ответ
void printWantedAnswer(vector<vector<string>> vec) {
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec[i].size(); j++) {
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

//печатает на экран ответ, вычисленный функций поиска
void printActualAnswer(list<sknfMember> ans) {
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


int main()
{
    list<sknfMember> resultSKNF;
    setlocale(LC_ALL, "ru");
    char choice = '-';
    cout << "Что вы хотите сделать?\n";
    cout << "1.Найти СКНФ\n";
    cout << "2.Найти СДНФ\n";
    cin >> choice;
    while (choice != '1' && choice != '2') {
        cout << "Такого пункта нет. Введите 1 или 2\n";
        cin >> choice;
    }
    switch (choice) {
    case '1':
    {
        cout << "1.Сгенерировать формулу при помощи генератора.\n";
        cout << "2.Ввести формулу с клавиатуры\n";
        cin >> choice;
        while (choice != '1' && choice != '2') {
            cout << "Такого пункта нет. Введите 1 или 2\n";
            cin >> choice;
        }
        switch (choice) {
        case '1':
        {
            srand(time(NULL));

            int numOfMembers = -1;
            int numOfVariables = -1;
            int numOfNegations = -1;
            int approxSize = -1;
            cout << "Введите количество членов, переменных, отрицаний и приблизительное количество операторов и переменных в формуле.\n";
            cin >> numOfMembers;
            cin >> numOfVariables;
            cin >> numOfNegations;
            cin >> approxSize;
            while (numOfMembers <= 0 || numOfVariables <= 0 || numOfNegations <= 0 || numOfNegations > numOfMembers * numOfVariables ||
                approxSize < (2 * numOfVariables - 1) * (2 * numOfMembers - 1)) {
                cout << "Вы ввели недопустимые данные\n";
                cout << "Попробуйте ещё раз\n";
                cin >> numOfMembers;
                cin >> numOfVariables;
                cin >> numOfNegations;
                cin >> approxSize;
            }
            pair <vector<vector<string>>, string> answer = newGenerator(numOfMembers, numOfVariables, numOfNegations, approxSize);
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
            resultSKNF.clear();
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
            
        }; break;
        case '2':
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
            string userInput;
            cout << "Введите вашу формулу:\n";
            cin >> userInput;
            while (!checkUserInput(userInput)) {
                cout << "Вы ввели некорректное выражение. Пожалуйста, проверьте его и исправьте ошибки.\n";
                cin >> userInput;
            }

            resultSKNF.clear();
            sknfMember buf;
            buf.signs.set(0, 1);
            resultSKNF.push_back(buf);

            node* root = new node;
            cout << "Начинается поиск.\n";
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            addnode(userInput, root);
            cout << "Дерево построено\n";
            distToLeaf(root);
            cout << "Рёбра отмечены\n";
            sknfSearch(0, resultSKNF, root);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
        }; break;
        }

        //вывод ответа

        cout << "Куда вывести ответ?\n" <<
            "1.На экран\n" <<
            "2.В файл\n" <<
            "3.И на экран и в файл.\n";
        cin >> choice;
        while (choice != '1' && choice != '2' && choice != '3') {
            cout << "Введите 1, 2 или 3.\n";
            cin >> choice;
        }
        if (choice == '1') {
            printActualAnswer(resultSKNF);
        }
        if (choice == '2') {
            //TODO
        }

        if (choice == '3') {
            //TODO
        }

    }; break;
    case '2':
    {

    }; break;
    }
    
    cout << "Программа завершена.\n";
    return 0;
}


