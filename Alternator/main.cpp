#include <iostream>
#include <vector>
#include <algorithm>

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
//a5, a31341, A134

//vector<string> sknfSearch(node* node, int wantedValue, vector<string> vec) {
//
//    //если попалась переменная
//    if (int(node->value) >= 97 && int(node->value) <= 122) {
//
//        //если вектор пустой
//        if (vec.empty()) {
//            string s;
//            if (wantedValue == 0) {
//                s.push_back(node->value);
//            }
//            else {
//                s.push_back(char(toupper(node->value)));
//            }
//            vec.push_back(s);
//            return vec;
//        }
//        else {
//            char valueToPost;
//            if (wantedValue == 1) {
//                valueToPost = char(toupper(node->value));
//            }
//            else {
//                valueToPost = node->value;
//            }
//            for (int i = 0; i < vec.size(); i++) {
//                if (vec[i].empty())
//                    continue;
//                bool good = true;
//                for (int j = 0; j < vec[i].size(); j++) {
//                    if (vec[i][j] == valueToPost) {
//                        good = false;
//                        break;
//                    }
//                    else if (vec[i][j] == int(valueToPost) + 32 || vec[i][j] == int(valueToPost) - 32) {
//                        //добавить удаление
//                        vec[i].clear();
//                        good = false;
//                    }
//                }
//                if (good)
//                    vec[i].push_back(valueToPost);
//                // что-то ещё...
//            }
//            return vec;
//        }
//    }
//    vector<pair<int, int>> pairs = getSuitableOperands(node->value, wantedValue);
//    vector<string> answer;
//    if (node->value == '!') {
//        answer = sknfSearch(node->right, pairs[0].second, vec);
//        return answer;
//    }
//    for (int i = 0; i < pairs.size(); i++) {
//        vector<string> res = sknfSearch(node->left, pairs[i].first, vec);
//        res = sknfSearch(node->right, pairs[i].second, res);
//        answer.insert(answer.end(), res.begin(), res.end());
//    }
//    return answer;
//}



int main()
{
    vector<string> stash;
    node* root = new node;
    string calculate;
    cin >> calculate;
    addnode(calculate, root);
    inOrderTravers(root);

    //stash = sknfSearch(root, 0, stash);

   /* for (int i = 0; i < stash.size(); i++) {
        if (!stash[i].empty()) {
            sort(stash[i].begin(), stash[i].end());
            for (int j = 0; j < stash[i].size(); j++) {
                if (isupper(stash[i][j])) {
                    cout << '!' << (char)tolower(stash[i][j]) << " ";
                }
                else {
                    cout << stash[i][j] << " ";
                }
            }
            cout << endl;
        }
    }*/
}


