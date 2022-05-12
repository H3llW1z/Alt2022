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
using namespace std;

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


//возвращает список всех допустимых операндов в зависимости от оператора и желаемого значения
vector<pair<int, int>> getSuitableOperands(int op, int wanted) {
    vector<pair<int, int>> ans;
    switch (op) {
    case -2: {
        if (wanted == 1) {
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(0, 0));
            ans.push_back(make_pair(1, 0));
            ans.push_back(make_pair(0, 1));
        }
    }break;

    case -3: {
        if (wanted == 1) {
            ans.push_back(make_pair(1, 0));
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(0, 0));
        }
    }break;

    case -4: {
        if (wanted == 1) {
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(1, 0));
        }
        else {
            ans.push_back(make_pair(1, 1));
            ans.push_back(make_pair(0, 0));
        }
    }break;

    case -1: {
        if (wanted == 1) {
            ans.push_back(make_pair(-1, 0));
        }
        else {
            ans.push_back(make_pair(-1, 1));
        }
    }break;

    case -6: {
        if (wanted == 1) {
            ans.push_back(make_pair(0, 0));
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(1, 0));
        }
    }break;

    case -5: {
        if (wanted == 1) {
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(0, 0));
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(1, 0));
        }
    }break;
    }
    return ans;
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


void sknfSearch(int wantedValue, list<list<short>> &lst, node* node) {
    if (lst.size() == 0)
        return;

    if (node->value < 0) {         //если попали в оператор

        vector<pair<int, int>> pairs = getSuitableOperands(node->value, wantedValue);  //получим все допустымые пары операндов

        if (node->value == -1) {   //если оператор - отрицание - идём только направо (допустимый операнд лишь один)
            sknfSearch(pairs[0].second, lst, node->right);
            return;
        }
        else {
            //во всех остальных случаях идём налево и направо, рассматривая все допустимые пары операндов
            list<list<short>> fromLeft0;
            list<list<short>> fromLeft1;
            if (pairs.size() > 1) {
                fromLeft1 = lst;
                sknfSearch(1, fromLeft1, node->left);
                fromLeft0 = lst;
                sknfSearch(0, fromLeft0, node->left);
            }
            else if (pairs[0].first == 1) {
                fromLeft1 = lst;
                sknfSearch(1, fromLeft1, node->left);
            }
            else {
                fromLeft0 = lst;
                sknfSearch(0, fromLeft0, node->left);
            }


            lst.clear();  ///не факт что работает как надо
            for (int i = 0; i < pairs.size(); ++i) {
                list<list<short>> buf = pairs[i].first == 1 ? fromLeft1 : fromLeft0;
                sknfSearch(pairs[i].second, buf, node->right);
                lst.insert(lst.end(), buf.begin(), buf.end());
            }
            return;
        }
    }
    else {    //если перед нами переменная
        int valueToPost = wantedValue == 1 ? -node->value : node->value;
        //если список пустой изначально, просто добавим туда одну комбинацию из одной переменной
        if (lst.size() == 1 && (*lst.begin()).size() == 0) {
            (*lst.begin()).push_back(valueToPost);
            return;
        }
        else {  //если же список комбинаций не пуст, надо пройти по нему и добавить переменную туда, где ее не хватает. При противоречиях вырезать комбинацию


            list<list<short>>::iterator it1;   //итератор для перемещения по списку комбинаций
            list<short>::iterator it2;         //итератор для перемещения внутри одной комбинации
            bool needToPost;        //храни информацию, нужно ли записать переменную в список
            it1 = lst.begin();  //внешний итератор поместим в начало списка комбинаций
            while (it1 != lst.end()) {
                needToPost=true;
                it2 = (* it1).begin();

                while (it2 != (*it1).end()) {
                    // если нашли переменную с номером больше текущей, вставляем сюда.
                    if (node->value < abs((*it2))) {
                        (*it1).insert(it2, valueToPost);
                        needToPost = false;
                        ++it1;
                        break;
                    }

                    //если наткнулись на эту же переменную, нужно проверить, в каком виде она входит
                    else if (node->value == abs((*it2))) {

                        //если она уже входит с другим знаком - комбинация дефектная, в мусор
                        if (valueToPost != (*it2)) {
                            lst.erase(it1++);
                        }
                        else {
                            ++it1;
                        }

                        needToPost = false;
                        break;
                    }
                    ++it2;
                }
                if (needToPost) {
                    (*it1).push_back(valueToPost);
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

    pair <vector<vector<string>>, string> answer = newGenerator(10, 8, 8, 1000);

    list<list<short>> resultSKNF;
    list<short> buf;
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
    addnode(calculate, root);
    cout << "Tree ready." << endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cout << "Search began." << endl;
    sknfSearch(0, resultSKNF, root);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    cout << "Actual answer: " << endl;

    for (auto it1 = resultSKNF.begin(); it1 != resultSKNF.end(); it1++) {

        for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
            if ((*it2) < 0)
                cout << "!";
            cout << "a";
            cout << abs((*it2)) << " ";

        }
        cout << endl;
    }


    cout << "Are answers equal? " << compareAnswers(resultSKNF, answer.first);
    
    return 0;
}


