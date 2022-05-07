#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <cstdlib>
#include <time.h>
#include <regex>
#include <random>
using namespace std;

struct node
{
    struct node* left = nullptr;
    struct node* right = nullptr;
    string value = "";
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

//void cleaner(vector <string> array) {
//    for (int i = 0; i < array.size(); i++) {
//        for (int j = 0; j < array[i].size(); j++) {
//            for (int k = 0; k < array[i].size(); k++) {
//                if (k == j) {
//                    continue;
//                }
//                if (array[i][j] == array[i][k]) {
//                    array[i].erase(j, 1);
//                }
//                if (abs(int(array[i][j]) - int(array[i][k])) == 32) {
//                    array[i].clear();
//                }
//            }
//        }
//    }
//}

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
vector<pair<int, int>> getSuitableOperands(char op, int wanted) {
    vector<pair<int, int>> ans;
    switch (op) {
    case '*': {
        if (wanted == 1) {
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(0, 0));
            ans.push_back(make_pair(1, 0));
            ans.push_back(make_pair(0, 1));
        }
    }break;

    case '+': {
        if (wanted == 1) {
            ans.push_back(make_pair(1, 0));
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(0, 0));
        }
    }break;

    case '^': {
        if (wanted == 1) {
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(1, 0));
        }
        else {
            ans.push_back(make_pair(1, 1));
            ans.push_back(make_pair(0, 0));
        }
    }break;

    case '!': {
        if (wanted == 1) {
            ans.push_back(make_pair(-1, 0));
        }
        else {
            ans.push_back(make_pair(-1, 1));
        }
    }break;

    case '=': {
        if (wanted == 1) {
            ans.push_back(make_pair(0, 0));
            ans.push_back(make_pair(1, 1));
        }
        else {
            ans.push_back(make_pair(0, 1));
            ans.push_back(make_pair(1, 0));
        }
    }break;

    case '>': {
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
        tree->value = expression;
        return;
    }
    tree->value = expression[lpIndex];
   
    if (tree->value[0] == '!') {
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


void inOrderTravers(node* root) {
    if (root) {
        inOrderTravers(root->left);
        cout << root->value << " ";
        inOrderTravers(root->right);
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

void vecClean(vector<vector<string>>& vec)
{
    /*for (int i = 0; i < vec.size(); ++i)
    {
        if (vec[i].size() == 0)
        {
            vec.erase(vec.begin() + i);
        }
    }*/
    vector<vector<string>> ans;
    for (int i = 0; i < vec.size();i++) {
        if (vec[i].size() != 0) {
            ans.push_back(vec[i]);
        }
    }
    vec.swap(ans);
}

//vector<vector<string>> sknfSearch(node* node, int wantedValue, vector<vector<string>> vec) {
//
//    //если попалась переменная
//    if (node->value[0]=='a') {
//
//        //если вектор пустой
//        if (!(vec.empty()) && vec[0][0]=="n") {
//            vector <string> temp;
//            if (wantedValue == 1) {
//                temp.push_back(node->value);
//            }
//            else {
//                string s = "A";
//                for (int i = 1; i < node->value.size(); i++) {
//                    s.push_back(node->value[i]);
//                }
//                temp.push_back(s);
//            }
//            vec.push_back(temp);
//            return vec;
//        }
//        else {
//            vecClean(vec);
//            string valueToPost;
//            if (wantedValue == 1) {
//                valueToPost = node->value;
//            }
//            else {
//                valueToPost = "A";
//                for (int i = 1; i < node->value.size(); i++) {
//                    valueToPost.push_back(node->value[i]);
//                }
//            }
//
//            for (int i = 0; i < vec.size();i++) {
//                if (vec[i].empty()) {
//                    continue;
//                }
//                bool isPermissible = true;
//                for (int j = 0; j < vec[i].size(); j++) {
//                    if (vec[i][j] == valueToPost) {
//                        //такая переменная уже есть в данной комбинации, выходим.
//                        isPermissible = false;
//                        break;
//                    }
//                    else if (vec[i][j].substr(1) == valueToPost.substr(1)) {
//                        if (vec[i][j][0] - valueToPost[0] != 0) {
//                            isPermissible = false;
//                            vector <string> x;
//                            vec[i].clear();
//                            vec[i].swap(x);
//                            break;
//                        }
//                    }
//                }
//                if (isPermissible) {
//                    vec[i].push_back(valueToPost);
//                }
//            }
//            vecClean(vec);
//            return vec;
//        }
//    }
//
//    vector<pair<int, int>> pairs = getSuitableOperands(node->value[0], wantedValue);
//    vector<vector<string>> answer;
//    if (node->value[0] == '!') {
//        answer = sknfSearch(node->right, pairs[0].second, vec);
//        return answer;
//    }
//    for (int i = 0; i < pairs.size(); i++) {
//        vector<vector<string>> res = sknfSearch(node->left, pairs[i].first, vec);
//        res = sknfSearch(node->right, pairs[i].second, res);
//        //vecClean(res);
//        answer.insert(answer.end(), res.begin(), res.end());
//    }
//    //vecClean(answer);
//    return answer;
//}

bool isOperator(char symbol) {
    int arraySize = sizeof(priorityArray)/sizeof(priorityArray[0]);

    for (int i = 0; i < arraySize; i++) {
        if (symbol == priorityArray[i])
            return true;
    }
    return false;
}

vector<vector<string>> sknfSearch(int wantedValue, vector<vector<string>> vec, node* node) {
    
    if (isOperator(node->value[0])) {
        //если попали в оператор
        vector<pair<int, int>> pairs = getSuitableOperands(node->value[0], wantedValue);
        
        vector<vector<string>> answer;

        if (node->value[0] == '!') {
            answer = sknfSearch(pairs[0].second, vec, node->right);
        }
        else {
            for (int i = 0; i < pairs.size(); ++i) {
                vector<vector<string>> curResult = sknfSearch(pairs[i].second, sknfSearch(pairs[i].first, vec, node->left), node->right);
                answer.insert(answer.end(), curResult.begin(), curResult.end());
            }
        }
        return answer;

    }
    else {
        //если попали в переменную
        string valueToPost = node->value;

        if (wantedValue == 0)
            valueToPost[0] = 'A';

        //если вектор пустой, просто добавим туда одну комбинацию из одной переменной
        if (vec.size() == 0) {
            vector<string> tempVec;

            tempVec.push_back(valueToPost);
            vec.push_back(tempVec);

            return vec;

        }
        else {
            //внешний цикл по комбинациям
            stringstream curVarNumberContainer(node->value.substr(1, node->value.size() - 1));
            int curVarNumber;
            curVarNumberContainer >> curVarNumber;

            for (int i = 0; i < vec.size(); ++i) {
                if (vec[i].size() == 0)
                    continue;
                //внутренний цикл по вхождениям переменной в комбинацию
                bool needToPost = true;
                for (int j = 0; j < vec[i].size(); ++j) {
                    stringstream varInVecContainer(vec[i][j].substr(1, vec[i][j].size() - 1));
                    int varInVec;
                    varInVecContainer >> varInVec;

                    //если нашли переменную с номером больше текущей, вставляем сюда.
                    if (curVarNumber < varInVec) {
                        vec[i].insert(vec[i].begin() + j, valueToPost);
                        needToPost = false;
                        break;
                    }
                    //если наткнулись на эту же переменную, нужно проверить, в каком виде она входит
                    else if (curVarNumber == varInVec) {
                        
                        //если она уже входит с другим знаком - комбинация дефектная, в мусор
                        if (valueToPost[0] != vec[i][j][0]) {
                            vec[i].clear(); 
                        }
                        needToPost = false;
                        break;
                    }
                }
                if (needToPost)
                    vec[i].push_back(valueToPost);
            }

            //очищаем вектор от всех пустых комбинаций.(освобождаем очень много памяти)
            vecClean(vec);
            //если вектор после очистки стал пустым, добавим один пустой элемент. Это нужно для разграничения пустых изначально и "опустошённых" векторов.
            if (vec.empty()) {
                vector<string> temp;
                vec.push_back(temp);
            }
            return vec;
        }
    }
}

void split_str(string const& str, const char delim, std::vector <string>& out)
{
    // create a stream from the string  
    stringstream s(str);

    string s2;
    while (getline(s, s2, delim))
    {
        out.push_back(s2); // store the string in s2  
    }
}

string getVarName(string str) {
    if (str[0] = '!') {
        return str.substr(1, str.size() - 1);
    }
    else return str;
}

//string knfToSknf(string &exp, int numOfNegatives, int numOfVars, int numOfComponents) {
//
//    vector<vector<string>> members;
//
//    size_t indexFrom = 0;
//    if (exp[0] == '(')
//        indexFrom = 1;
//
//    size_t indexTo = -1;
//    //тут мы разбиваем КНФ на элементарные дизъюнкции и записываем их в вектор.
//
//    vector<string> elemDisjuncts;
//
//    split_str(exp, '*', elemDisjuncts);
//
//    //удалим внешние лишние скобки
//    for (size_t i = 0; i < elemDisjuncts.size(); i++) {
//
//        removeOuterBraces(elemDisjuncts[i]);
//
//        vector<string> buf;
//        split_str(elemDisjuncts[i], '+', buf);
//        members.push_back(buf);
//    }
//    //построили вектор всех множителей кнф
//
//
//    //найдём количество переменных, которые нужно добавить к существующим
//
//    //здесь будем хранить количество переменных, которые нужно добавить в выражение(которых нет сейчас вообще)
//    int numOfVarsToAdd = -1;
//    //список всех переменных, участвующих в выражении на данный момент
//    vector<string> vars;
//
//    //внешний цикл по всем множителям кнф
//    for (size_t i = 0; i < members.size(); i++) {
//        //внутренний цикл по всем переменным в ЭД
//        for (size_t j = 0; j < members[i].size(); j++) {
//            bool isAdded = false;
//            string compareA;
//
//            if (members[i][j][0] == '!') {
//                compareA = members[i][j].substr(1, members[i][j].size()-1);
//            }
//            else {
//                compareA = members[i][j];
//            }
//            //сравним переменную со всеми, что уже были добавлены в список
//            for (size_t k = 0; k < vars.size(); k++) {
//                if (vars[k] == compareA) {
//                    isAdded = true;
//                }
//            }
//            if (!isAdded) {
//                vars.push_back(compareA);
//            }
//        }
//    }
//    
//    //если желаемое количество переменных меньше, чем есть сейчас, то бросаем исключение
//    if (numOfVars < vars.size()) {
//        throw std::invalid_argument("Желаемое количество переменных меньше существующего.");
//    }
//    else {
//        numOfVarsToAdd = numOfVars - vars.size();
//    }
//
//    //теперь у нас есть всё, что нужно, чтобы преобразовать нашу КНФ в СКНФ.
//
//    //сюда будем складывать слагаемые, каждое из которых это список переменных с соответствующими знаками.
//    vector<vector<string>> sknfMembers;
//    
//    //внешний цикл по всем слагаемым КНФ
//    for (size_t i = 0; i < members.size(); i++) {
//
//        //этот вектор содержит имена всех переменных, которых не хватает в этом члене КНФ
//        vector<string> varsToAdd;
//
//        //пробегаем по всем переменным, которые у нас есть в выражении
//        for (size_t k = 0; k < vars.size(); k++) {
//            bool isExists = false;
//            for (size_t j = 0; j < members[i].size(); j++) {
//
//                string varName = members[i][j][0] == '!' ? members[i][j].substr(1,members[i][j].size()-1): members[i][j];
//
//                if (vars[k] == varName) {
//                    isExists = true;
//                }
//            }
//
//            if (!isExists) {
//                varsToAdd.push_back(vars[k]);
//            }
//        }
//        
//        vector<string> bufMember;
//        //????
//        bufMember = members[i];
//        sknfMembers.push_back(members[i]);
//        for (size_t k = 0; k < varsToAdd.size(); k++) {
//            sknfMembers.push_back(members[i]);
//            int index = -2;
//            for (size_t j = 0; j < members[i].size(); j++) {
//                if (members[i][j] > varsToAdd[k]) {
//                    index = j - 1;
//                }
//            }
//            if (index == -2) {
//                sknfMembers[sknfMembers.size() - 1].push_back(varsToAdd[k]);
//                sknfMembers[sknfMembers.size() - 2].push_back("!" + varsToAdd[k]);
//            }
//            else if (index == -1) {
//                sknfMembers[sknfMembers.size() - 1].insert(sknfMembers[sknfMembers.size() - 1].begin(), varsToAdd[k]);
//                sknfMembers[sknfMembers.size() - 2].insert(sknfMembers[sknfMembers.size() - 2].begin(), "!" + varsToAdd[k]);
//            }
//            else {
//                sknfMembers[sknfMembers.size() - 1].insert(sknfMembers[sknfMembers.size() - 1].begin() + index, varsToAdd[k]);
//                sknfMembers[sknfMembers.size() - 2].insert(sknfMembers[sknfMembers.size() - 2].begin() + index, "!" + varsToAdd[k]);
//            }
//        }
//
//
//
//    }
//}


//функция возвращает формулу, являющуюся тождественным нулем или единицей.
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


pair<vector<vector<string>>, string> formulaGeneratorSKNF(int ceilNumOfMembers, int numOfVars, int numOfNegations) {


    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {
        throw invalid_argument("Количество членов больше возможного");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {
        throw invalid_argument("Количество отрицаний не может быть больше суммарного количества переменных в СКНФ");
    }


    //в этот вектор поместим будущую скнф
    vector<vector<string>> sknf;

    //количество уже установленных знаков отрицания
    int negationsSet = 0; 


    for (int i = 0; i < ceilNumOfMembers; i++) {
        vector<string> member;
        string negation = "!";
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
                    sknf[i][j].insert(0,1, '!');
                }
            }
            if (isReady)
                break;
        }
    } 
    //теперь нужно удалить все дубликаты из скнф чтобы предоставить её на выход. А использовать можно и с дубликатами
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
    

    


    //теперь усложним
    //усложнение на уровне переменных
    for (int i = 0; i < sknf.size();i++) {

        for (int j = 0; j < sknf[i].size(); j++) {

            int needComplicate = rand() % 4;

            if (needComplicate == 1) {

                int zeroOrOne = rand() % 2;
                if (zeroOrOne == 0) {
                    sknf[i][j] += "+" + complicateConstant(3 + rand() % 2, numOfVars, 0);
                }
                else {
                    sknf[i][j] += "*" + complicateConstant(3 + rand() % 2, numOfVars, 1);
                }
                sknf[i][j] = "(" + sknf[i][j] + ")";
            }
        }
    }


    /*string tempAnswer;
    for (int i = 0; i < sknf.size(); i++) {
        tempAnswer += "(";
        for (int j = 0; j < sknf[i].size(); j++) {
            tempAnswer += sknf[i][j] + "+";
        }
        tempAnswer.pop_back();
        tempAnswer += ")*";
    }
    tempAnswer.pop_back();
    return make_pair(standartizedSKNF, tempAnswer);*/

    //усложнение на уровне члена

    for (int i = 0; i < sknf.size(); i++) {
        vector<string> buf;

        random_device rd;
        default_random_engine rng(rd());

        shuffle(sknf[i].begin(), sknf[i].end(), rng);
        int numOfIterations = (sknf[i].size() - rand() % sknf[i].size()) / 2;

        for (int j = 0; j < numOfIterations; j++) {
            string newMember;
            int lastIndex = sknf[i].size() - 1;
            //x+y=x^y^x*y мб скобки по краям не нужны
            newMember = "(" + sknf[i][0] + "^" + sknf[i][lastIndex] + "^" + sknf[i][0] + "*" + sknf[i][lastIndex] + ")";
            buf.push_back(newMember);
            sknf[i].pop_back();
            sknf[i].erase(sknf[i].begin());
        }
        buf.insert(buf.end(), sknf[i].begin(), sknf[i].end());
        sknf[i] = buf;
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
    //теперь проведём усложнение на уровне формулы. Между членами стоит *

    random_device rd;
    default_random_engine rng(rd());

    shuffle(compMembSKNF.begin(), compMembSKNF.end(), rng);
    int numOfIterations = (compMembSKNF.size() - rand() % compMembSKNF.size()) / 2;

    vector<string> abobus;
    for (int i = 0; i < numOfIterations; i++) {
        string buf;
        int lastIndex = compMembSKNF.size() - 1;
        buf = "((" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(!" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(" + compMembSKNF[0] + "+!" + compMembSKNF[lastIndex] + "))";
        abobus.push_back(buf);
        compMembSKNF.pop_back();
        compMembSKNF.erase(compMembSKNF.begin());
    }
    ///!!!!!!вставил
    abobus.insert(abobus.end(), compMembSKNF.begin(), compMembSKNF.end());


    string answer;
    for (int i = 0; i < abobus.size(); i++) {
        answer.append(abobus[i]);
        answer += "*";
    }
    answer.pop_back();
    return make_pair(standartizedSKNF, answer);
}

int main()
{
    srand(time(NULL));
    //string expression = "a1*(a1+!a2+a3)*(!a1+a3)";
    //cin >> expression;

    pair<vector<vector<string>>, string> answer = formulaGeneratorSKNF(3 ,10, 13);


    //////////////////////
    //return 0;
    //////////////////////
    vector<vector<string>> stash;
    //vector<string> buf{"n"};
    //stash.push_back(buf);
    node* root = new node;
    string calculate;
    //cin >> calculate;
    calculate = answer.second;
    cout << "Formula:\n";
    cout << calculate << endl;
    cout << "Wanted answer:" << endl;
    for (int i = 0; i < answer.first.size(); i++) {
        for (int j = 0; j < answer.first[i].size(); j++) {
            cout << answer.first[i][j] << " ";
        }
        cout << endl;
    }


    //inOrderTravers(root);
    cout << "Are braces correct? " << checkBraces(calculate) << endl;
    cout << "SKNF search began." << endl;
    addnode(calculate, root);
    cout << "Tree ready." << endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
     stash = sknfSearch(0, stash, root);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[ms]" << std::endl;
    for (int i = 0; i < stash.size(); i++) {
        if (!stash[i].empty()) {
            for (int j = 0; j < stash[i].size(); j++) {
                string buf;
                if (stash[i][j][0] == 'a') {
                    buf += "!a";
                }
                else buf += 'a';
                buf += stash[i][j].substr(1, stash[i][j].size() - 1);
                cout << buf << " ";
            }
            cout << endl;
        }
    }
    return 0;
}


