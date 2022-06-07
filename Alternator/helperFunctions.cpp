#include "helperFunctions.h"
#include "expressionFunctions.h"
//-----------ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (сравнение ответов, вывод на экран и в файл)______//
extern int numOfVars;
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
                cout << "a" << i + 1 << " ";
            }
        }
        cout << endl;

    }
}

void printSDNFInFile(list<sknfMember> ans, string exception)
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
        if (j % 110 == 0 && j > 1)
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
        if (j % 110 == 1 && j > 1)
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

void rpnPrintActualAnswer(vector<bitset<NEW_ALGO_MAX_VARS>>& vec, int flag) {
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

void rpnPrintInFile(vector<bitset<NEW_ALGO_MAX_VARS>>& vec, string& expression, int flag) {
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
void printAnswerHint() {
    cout << "\nО выводе ответа:\n" <<
        "Каждая строка ответа - один член СКНФ (СДНФ)\n" <<
        "Знаки между членами и переменными опущены для упрощения чтения.\nЕсли ищем СКНФ, то между переменными дизъюнкция (+), между членами - конъюнкция (*).\nЕсли ищем СДНФ - наоборот.\n";
}