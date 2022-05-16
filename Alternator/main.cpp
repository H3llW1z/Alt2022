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

#define MAX_VARS 10


//структура, определяющая узел дерева выражения
struct node
{
    struct node* left = nullptr;
    struct node* right = nullptr;
    short value = 0;
};


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


//Удаляет внешние парные скобки, прим. ((a+b)) -> a+b
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
char priorityArray[6] = { '=', '>', '^', '+', '*', '!'};
//числ. обоз-я операторов -6   -5   -4   -3   -2   -1


int getOperatorsIntForm(char op) {
    switch (op) {
    case '=': return -6; break;
    case '>': return -5; break;
    case '^': return -4; break;
    case '+': return -3; break;
    case '*': return -2; break;
    case '!': return -1; break;
    default: throw::invalid_argument("Wrong operator char");
    }
}


  //ищет наименее приоритетный оператор и возвращает его позицию в строке
int find_low_priority_operator(string expression) {
    bool isThereOperator = false;
    for (int i = 0; i < expression.size(); i++) {
        for (int j = 0; j < 6; j++) {
            if (expression[i] == priorityArray[j]) {
                isThereOperator = true;
                break;
            }
        }
        if (isThereOperator) {
            break;
        }
    }

    if (!isThereOperator) {
        return -1;
    }


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
        if (closeBrace == openBrace && (expression[i] == '!' || expression[i] == '>' || expression[i] == '^' || expression[i] == '=' || expression[i] == '+' || expression[i] == '*')) {
            v.push_back(operators(i, expression[i]));

            closeBrace = 0;
            openBrace = 0;
        }
    }
    for (int i = 0; i < 6; ++i) {
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
    removeOuterBraces(expression);
    int lpIndex = find_low_priority_operator(expression);
    //если оператора нет, то имеем строку, содержащую только одну переменную, прим. a15
    if (lpIndex == -1) { 
        stringstream intVarContainer(expression.substr(1, expression.size() - 1));
        int intVar;
        intVarContainer >> intVar;
        tree->value = intVar;
        return;
    }
    tree->value = getOperatorsIntForm(expression[lpIndex]);
   
    if (tree->value == -1) { // если !
        tree->right = new node;
        addnode(expression.substr(lpIndex + 1), tree->right);
    }
    else {
        tree->left = new node;
        tree->right = new node;
        addnode(expression.substr(0, lpIndex), tree->left);
        addnode(expression.substr(lpIndex + 1), tree->right);
    }
}


// a - 0, A - 1
// раньше было наоборот
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
// 
//a5, a31341, A134


//этот массив представляет собой дерево. Он нужен для определения подходящих операндов.
//первая координата - числовое представление оператора минус 2. Вторая - желаемое значение. Так можно попасть в массив подходящих операндов.
//здесь отсутствует отрицание, т.к. его обработка тривиальна.

short operands[5][2][7] = {
    { { 3, 1,0, 0,1, 0,0 }, { 1, 1,1, -1,-1, -1,-1 } },   // 2 умножение

    { {1, 0,0, -1,-1, -1,-1 }, { 3, 0,1, 1,0, 1,1 } },   // 3 сложение

    { {2, 0,0, 1,1, -1,-1 }, {2, 0,1, 1,0, -1,-1 } },   // 4 XOR

    { {1, 1,0, -1,-1, -1,-1 }, {3, 1,1, 0,0, 0,1 } },   // 5 импликация

    { {2, 0,0, 1,1, -1,-1 }, { 2, 0,1, 1,0, -1, -1} }    // эквиваленция
};

struct sknfMember {
    bitset <MAX_VARS> vars;
    bitset <MAX_VARS> signs;
};

//глобальные переменные, которые использует функция поиска.
//объявлены глобально, т.к. требуется лишь по одному экзмепляру каждой в любой момент времени.
//----------------------------------------------------------------------

list<sknfMember>::iterator it1;         //итератор для перемещения по списку комбинаций

stack <list<sknfMember>> globalStack;   //глобальный стек для сохранения данных, пришедших из родителя.

//-----------------------------------------------------------------------


void sknfSearch(bool wantedValue, list<sknfMember> &lst, node* node) {
    if (lst.size() == 0) {
        return;
    }

    if (node->value < 0) {         //если попали в оператор

        if (node->value == -1) {
            sknfSearch(!wantedValue, lst, node->right);
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 1) {   // если пара операндов одна
            if (node->right->value > 0) {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
            }
            else {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
            }
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 2) {   //если пар операндов две
            globalStack.push(lst);
            if (node->right->value > 0) {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);
            }
            else {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], lst, node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], lst, node->right);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], globalStack.top(), node->left);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], globalStack.top(), node->right);
            }
            lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());
            globalStack.pop();
            return;
        }
        if (operands[abs(node->value) - 2][wantedValue][0] == 3) {  //если пар операндов 3
            globalStack.push(lst);

            if (node->right->value > 0) {
                sknfSearch(operands[abs(node->value) - 2][wantedValue][4], lst, node->right);
                sknfSearch(operands[abs(node->value) - 2][wantedValue][3], lst, node->left);

                sknfSearch(operands[abs(node->value) - 2][wantedValue][2], globalStack.top(), node->right);
                globalStack.push(globalStack.top());
                sknfSearch(operands[abs(node->value) - 2][wantedValue][1], globalStack.top(), node->left);
                lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());
                globalStack.pop();

                sknfSearch(operands[abs(node->value) - 2][wantedValue][5], globalStack.top(), node->left);
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
            lst.insert(lst.end(), globalStack.top().begin(), globalStack.top().end());
            globalStack.pop();
            return;
        }
    }
    else {    //если перед нами переменная
        //если список пустой изначально, просто добавим туда одну комбинацию из одной переменной
        if (lst.size() == 1 && (*lst.begin()).signs.test(0) && !((*lst.begin()).vars.test(0))) {
            (*lst.begin()).signs.set(0, 0);
            (*lst.begin()).vars.set(node->value - 1, 1);
            (*lst.begin()).signs.set(node->value - 1, wantedValue);   //если хотим единицу - нужно отрицание. 1 - значит нужно отрицание
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

//размер массива операторов указан как 6, может быть увеличен!!!
bool isOperator(char ch) {

    for (int i = 0; i < 6; i++) {
        if (ch == priorityArray[i])
            return true;
    }
    return false;
}

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
        string variantsList[2] = { "(!(1^((!2+3)*!2+1)>3)*3)", "(!(((3+2*1)^!(3+2*1))>!(3*!2*2)))" };

        string var1 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var2 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var3 = "a" + to_string(rand() % numOfVarsTotal + 1);

        int variant = rand() % 2;

        string buf = variantsList[variant];
        for (int i = 0; i < buf.size(); i++) {
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

        if (constFlag) {
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

pair<vector<vector<string>>, string> newGenerator(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1)*(2 * numOfVars - 1)) {
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

    while (negationsSet < numOfNegations) {
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

    int perMember = onVarLevel / ceilNumOfMembers;
    
    for (int i = 0; i < sknf.size(); i++) {

        string ans;
        int toAdd = perMember;
        while (toAdd > perMember / 10) {

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

    random_device rd;
    default_random_engine rng(rd());

    int onLevel2 = onMemberLevel / ceilNumOfMembers;

    for (int i = 0; i < sknf.size(); i++) {


        int toAdd = onLevel2;

        while (toAdd > onLevel2 / 10 && sknf[i].size() != 1) {

            shuffle(sknf[i].begin(), sknf[i].end(), rng);

            string newMember;
            int lastIndex = sknf[i].size() - 1;
            //x+y=x^y^x*y 
            
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
    //onFormulaLevel

    int toAdd = onFormulaLevel;

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

void inOrderTravers(node* root) {
    if (root) {
        inOrderTravers(root->left);
        cout << root->value << " ";
        inOrderTravers(root->right);
    }
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

int main()
{
    srand(time(NULL));
    pair <vector<vector<string>>, string> answer = newGenerator(10, 10, 42, 1200);
    //pair <vector<vector<string>>, string> answer = newGenerator(20, 17, 200, 2000);
    list<sknfMember> resultSKNF;
    sknfMember buf;
    buf.signs.set(0, 1);
    resultSKNF.push_back(buf);
    node* root = new node;
    string calculate;

    calculate = answer.second;
    cout << "Formula:\n";
    cout << calculate << endl;

    cout << "Number of variables and operators: " << countVarsAndOperators(calculate) << endl;
    cout << "Formula length: " << calculate.size() << endl;
    
    cout << "Are braces correct? " << checkBraces(calculate) << endl;

    cout << "Wanted answer:" << endl;
    for (int i = 0; i < answer.first.size(); i++) {
        for (int j = 0; j < answer.first[i].size(); j++) {
            cout << answer.first[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "SKNF search began." << endl;
    //calculate = "((((a2^a4^a2*a4)+a5+(a1+(a4*a8*(a8*((a4^(a5+a8^a8))^a5*a8)>!a8)*a5*a8*!a5))+a7+a8+!a6+!a3)+((!a3^a2^!a3*a2)+((a6*!(!(a6^((!a3+a7)*!a3+a6)>a7)*a7))^(a7*!(!(((a1+a7*a3)^!(a1+a7*a3))>!(a1*!a7*a7))))^(a6*!(!(a6^((!a3+a7)*!a3+a6)>a7)*a7))*(a7*!(!(((a1+a7*a3)^!(a1+a7*a3))>!(a1*!a7*a7)))))+!a8+(a5+(!(((a3+a5*a1)^!(a3+a5*a1))>!(a3*!a5*a5))))+a4+(a1*!(!(a6^((!a5+a5)*!a5+a6)>a5)*a5))))*(!((a2^a4^a2*a4)+a5+(a1+(a4*a8*(a8*((a4^(a5+a8^a8))^a5*a8)>!a8)*a5*a8*!a5))+a7+a8+!a6+!a3)+((!a3^a2^!a3*a2)+((a6*!(!(a6^((!a3+a7)*!a3+a6)>a7)*a7))^(a7*!(!(((a1+a7*a3)^!(a1+a7*a3))>!(a1*!a7*a7))))^(a6*!(!(a6^((!a3+a7)*!a3+a6)>a7)*a7))*(a7*!(!(((a1+a7*a3)^!(a1+a7*a3))>!(a1*!a7*a7)))))+!a8+(a5+(!(((a3+a5*a1)^!(a3+a5*a1))>!(a3*!a5*a5))))+a4+(a1*!(!(a6^((!a5+a5)*!a5+a6)>a5)*a5))))*(((a2^a4^a2*a4)+a5+(a1+(a4*a8*(a8*((a4^(a5+a8^a8))^a5*a8)>!a8)*a5*a8*!a5))+a7+a8+!a6+!a3)+!((!a3^a2^!a3*a2)+((a6*!(!(a6^((!a3+a7)*!a3+a6)>a7)*a7))^(a7*!(!(((a1+a7*a3)^!(a1+a7*a3))>!(a1*!a7*a7))))^(a6*!(!(a6^((!a3+a7)*!a3+a6)>a7)*a7))*(a7*!(!(((a1+a7*a3)^!(a1+a7*a3))>!(a1*!a7*a7)))))+!a8+(a5+(!(((a3+a5*a1)^!(a3+a5*a1))>!(a3*!a5*a5))))+a4+(a1*!(!(a6^((!a5+a5)*!a5+a6)>a5)*a5)))))";
    calculate = "(!a10+(a8^(a5^a4^a5*a4)^a8*(a5^a4^a5*a4))+(a2+(!(((a6+a2*a10)^!(a6+a2*a10))>!(a6*!a2*a2))))+a6+a9+a1+(a3^a7^a3*a7))*(((a8+a5+a6+a4+a9+(a10^a3^a10*a3)+a7+(!a2^(a1+(a5*a2*(a2*((a5^(a2+a2^a7))^a2*a7)>!a2)*a2*a2*!a2))^!a2*(a1+(a5*a2*(a2*((a5^(a2+a2^a7))^a2*a7)>!a2)*a2*a2*!a2))))+(!a2+a9+!a8+(!a6*!(!(((a5+a9*a2)^!(a5+a9*a2))>!(a5*!a9*a9))))+!a4+!a3+!a7+(!a10^(!a5^!a1^!a5*!a1)^!a10*(!a5^!a1^!a5*!a1))))*(!(a8+a5+a6+a4+a9+(a10^a3^a10*a3)+a7+(!a2^(a1+(a5*a2*(a2*((a5^(a2+a2^a7))^a2*a7)>!a2)*a2*a2*!a2))^!a2*(a1+(a5*a2*(a2*((a5^(a2+a2^a7))^a2*a7)>!a2)*a2*a2*!a2))))+(!a2+a9+!a8+(!a6*!(!(((a5+a9*a2)^!(a5+a9*a2))>!(a5*!a9*a9))))+!a4+!a3+!a7+(!a10^(!a5^!a1^!a5*!a1)^!a10*(!a5^!a1^!a5*!a1))))*((a8+a5+a6+a4+a9+(a10^a3^a10*a3)+a7+(!a2^(a1+(a5*a2*(a2*((a5^(a2+a2^a7))^a2*a7)>!a2)*a2*a2*!a2))^!a2*(a1+(a5*a2*(a2*((a5^(a2+a2^a7))^a2*a7)>!a2)*a2*a2*!a2))))+!(!a2+a9+!a8+(!a6*!(!(((a5+a9*a2)^!(a5+a9*a2))>!(a5*!a9*a9))))+!a4+!a3+!a7+(!a10^(!a5^!a1^!a5*!a1)^!a10*(!a5^!a1^!a5*!a1)))))*((!a4+(a2*a7*(a7*((a2^(a5+a7^a1))^a5*a1)>!a7)*a5*a7*!a5))+!a2+!a8+a7+!a9+!a6+!a10+(!a5^(!a3^a1^!a3*a1)^!a5*(!a3^a1^!a3*a1)))*((!a8^a3^!a8*a3)+a9+(!a7^a4^!a7*a4)+a10+!a5+(!a1*!(!(a1^((!a6+a2)*!a6+a1)>a2)*a2))+(!a2^a6^!a2*a6))*((!a2^a3^!a2*a3)+!a7+!a9+(!a8+(a1*a8*(a8*((a1^(a1+a8^a1))^a1*a1)>!a8)*a1*a8*!a1))+!a10+!a6+((!a4^a1^!a4*a1)^a5^(!a4^a1^!a4*a1)*a5))*(a10+(a4+(!(((a7+a2*a9)^!(a7+a2*a9))>!(a7*!a2*a2))))+a9+!a1+a2+(!a6^a5^!a6*a5)+(a3^(a8^a7^a8*a7)^a3*(a8^a7^a8*a7)))*(a1+a6+(a2^a4^a2*a4)+!a10+a7+a5+a9+((a3+(a4*a9*(a9*((a4^(a2+a9^a3))^a2*a3)>!a9)*a2*a9*!a2))^a8^(a3+(a4*a9*(a9*((a4^(a2+a9^a3))^a2*a3)>!a9)*a2*a9*!a2))*a8))*((((a6^!a9^a6*!a9)+a5+(a3+(a9*a5*(a5*((a9^(a7+a5^a2))^a7*a2)>!a5)*a7*a5*!a7))+(!a4^!a2^!a4*!a2)+a7+a10+(a1^!a8^a1*!a8))+(a9+!a7+a6+!a1+a3+a5+a2+a10+((!a8*!(a7*a10*(a10*((a7^(a5+a10^a7))^a5*a7)>!a10)*a5*a10*!a5))^a4^(!a8*!(a7*a10*(a10*((a7^(a5+a10^a7))^a5*a7)>!a10)*a5*a10*!a5))*a4)))*(!((a6^!a9^a6*!a9)+a5+(a3+(a9*a5*(a5*((a9^(a7+a5^a2))^a7*a2)>!a5)*a7*a5*!a7))+(!a4^!a2^!a4*!a2)+a7+a10+(a1^!a8^a1*!a8))+(a9+!a7+a6+!a1+a3+a5+a2+a10+((!a8*!(a7*a10*(a10*((a7^(a5+a10^a7))^a5*a7)>!a10)*a5*a10*!a5))^a4^(!a8*!(a7*a10*(a10*((a7^(a5+a10^a7))^a5*a7)>!a10)*a5*a10*!a5))*a4)))*(((a6^!a9^a6*!a9)+a5+(a3+(a9*a5*(a5*((a9^(a7+a5^a2))^a7*a2)>!a5)*a7*a5*!a7))+(!a4^!a2^!a4*!a2)+a7+a10+(a1^!a8^a1*!a8))+!(a9+!a7+a6+!a1+a3+a5+a2+a10+((!a8*!(a7*a10*(a10*((a7^(a5+a10^a7))^a5*a7)>!a10)*a5*a10*!a5))^a4^(!a8*!(a7*a10*(a10*((a7^(a5+a10^a7))^a5*a7)>!a10)*a5*a10*!a5))*a4))))";
    addnode(calculate, root);
    cout << "Tree ready." << endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cout << "Search began." << endl;
    sknfSearch(0, resultSKNF, root);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    cout << "Actual answer: " << endl;
    for (auto it1 = resultSKNF.begin(); it1 != resultSKNF.end(); it1++) {
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
    return 0;
}


