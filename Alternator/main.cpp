#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <cstdlib>

using namespace std;

struct node
{
    struct node* left = nullptr;
    struct node* right = nullptr;
    string value = "";
};

//��������� ������������ ��������� �����������
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

//������� ������� ������ ������, ����. ((a+b)) -> a+b
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

//��������������� ��������� ��� ������ �������� ������������� ���������
struct operators
{
    operators(int ind, char val) {
        value = val;
        index = ind;
    };
    char value;
    int index;
};

//������ ���� ��������� ����������, ������������� � ������� ����������� ����������
char priorityArray[6] = { '=', '>', '^', '+', '*', '!'};


//���� �������� ������������ �������� � ���������� ��� ������� � ������
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


//���������� ������ ���� ���������� ��������� � ����������� �� ��������� � ��������� ��������
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


//��������� ���� � ������ ���������
void addnode(string expression, node* tree) {
    removeOuterBraces(expression);
    int lpIndex = find_low_priority_operator(expression);
    //���� ��������� ���, �� ����� ������, ���������� ������ ���� ����������, ����. a15
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

// a - 1, A - 0
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
//    //���� �������� ����������
//    if (node->value[0]=='a') {
//
//        //���� ������ ������
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
//                        //����� ���������� ��� ���� � ������ ����������, �������.
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
        //���� ������ � ��������
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
        //���� ������ � ����������
        string valueToPost = node->value;

        if (wantedValue == 0)
            valueToPost[0] = 'A';

        //���� ������ ������, ������ ������� ���� ���� ���������� �� ����� ����������
        if (vec.size() == 0) {
            vector<string> tempVec;

            tempVec.push_back(valueToPost);
            vec.push_back(tempVec);

            return vec;

        }
        else {
            //������� ���� �� �����������
            stringstream curVarNumberContainer(node->value.substr(1, node->value.size() - 1));
            int curVarNumber;
            curVarNumberContainer >> curVarNumber;

            for (int i = 0; i < vec.size(); ++i) {
                if (vec[i].size() == 0)
                    continue;
                //���������� ���� �� ���������� ���������� � ����������
                bool needToPost = true;
                for (int j = 0; j < vec[i].size(); ++j) {
                    stringstream varInVecContainer(vec[i][j].substr(1, vec[i][j].size() - 1));
                    int varInVec;
                    varInVecContainer >> varInVec;

                    //���� ����� ���������� � ������� ������ �������, ��������� ����.
                    if (curVarNumber < varInVec) {
                        vec[i].insert(vec[i].begin() + j, valueToPost);
                        needToPost = false;
                        break;
                    }
                    //���� ���������� �� ��� �� ����������, ����� ���������, � ����� ���� ��� ������
                    else if (curVarNumber == varInVec) {
                        
                        //���� ��� ��� ������ � ������ ������ - ���������� ���������, � �����
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

            //������� ������ �� ���� ������ ����������.(����������� ����� ����� ������)
            vecClean(vec);
            //���� ������ ����� ������� ���� ������, ������� ���� ������ �������. ��� ����� ��� ������������� ������ ���������� � "������������" ��������.
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
//    //��� �� ��������� ��� �� ������������ ���������� � ���������� �� � ������.
//
//    vector<string> elemDisjuncts;
//
//    split_str(exp, '*', elemDisjuncts);
//
//    //������ ������� ������ ������
//    for (size_t i = 0; i < elemDisjuncts.size(); i++) {
//
//        removeOuterBraces(elemDisjuncts[i]);
//
//        vector<string> buf;
//        split_str(elemDisjuncts[i], '+', buf);
//        members.push_back(buf);
//    }
//    //��������� ������ ���� ���������� ���
//
//
//    //����� ���������� ����������, ������� ����� �������� � ������������
//
//    //����� ����� ������� ���������� ����������, ������� ����� �������� � ���������(������� ��� ������ ������)
//    int numOfVarsToAdd = -1;
//    //������ ���� ����������, ����������� � ��������� �� ������ ������
//    vector<string> vars;
//
//    //������� ���� �� ���� ���������� ���
//    for (size_t i = 0; i < members.size(); i++) {
//        //���������� ���� �� ���� ���������� � ��
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
//            //������� ���������� �� �����, ��� ��� ���� ��������� � ������
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
//    //���� �������� ���������� ���������� ������, ��� ���� ������, �� ������� ����������
//    if (numOfVars < vars.size()) {
//        throw std::invalid_argument("�������� ���������� ���������� ������ �������������.");
//    }
//    else {
//        numOfVarsToAdd = numOfVars - vars.size();
//    }
//
//    //������ � ��� ���� ��, ��� �����, ����� ������������� ���� ��� � ����.
//
//    //���� ����� ���������� ���������, ������ �� ������� ��� ������ ���������� � ���������������� �������.
//    vector<vector<string>> sknfMembers;
//    
//    //������� ���� �� ���� ��������� ���
//    for (size_t i = 0; i < members.size(); i++) {
//
//        //���� ������ �������� ����� ���� ����������, ������� �� ������� � ���� ����� ���
//        vector<string> varsToAdd;
//
//        //��������� �� ���� ����������, ������� � ��� ���� � ���������
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

pair<string, string> formulaGeneratorSKNF(int numOfMembers, int numOfVars, int numOfNegations) {

    if (numOfMembers > pow(numOfVars, 2)) {
        throw invalid_argument("���������� ������ ������ ����������");
    }

    vector<vector<string>> sknf;
    int negationsSet = 0; 
    for (int i = 0; i < numOfMembers; i++) {
        vector<string> member;
        string negation = "!";
        for (int j = 1; j <= numOfVars; j++) {

            member.push_back("a" + to_string(j));
            int needNegation = rand() % 2;
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

    return make_pair("a", "a");

}

int main()
{
    string expression = "a1*(a1+!a2+a3)*(!a1+a3)";
    //cin >> expression;

    pair<string, string> answer = formulaGeneratorSKNF(69, 200, 4500);


    //////////////////////
    return 0;
    //////////////////////
    vector<vector<string>> stash;
    //vector<string> buf{"n"};
    //stash.push_back(buf);
    node* root = new node;
    string calculate;
    cin >> calculate;
    addnode(calculate, root);
    //inOrderTravers(root);
    cout << endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
     stash = sknfSearch(0, stash, root);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[ms]" << std::endl;
    for (int i = 0; i < stash.size(); i++) {
            for (int j = 0; j < stash[i].size(); j++) {
                cout << stash[i][j] << " ";
            }
            cout << endl;
        
    }
    return 0;
}


