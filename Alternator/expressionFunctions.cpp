//Проверяет правильность скобочной конструкции

#include "expressionFunctions.h"
extern int numOfVars; 
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
