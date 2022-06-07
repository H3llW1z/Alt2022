#include "generators.h"
extern int numOfVars;
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
string complicateConstant(int numOfVars, int numOfVarsTotal, bool constFlag) {
    string answer;

    switch (numOfVars) {
    case 3: {
        //заранее подготовленные комбинации, дающие 0 при всех наборах
        string variantsList[4] = { "(!(1^((!2+3)*!2+1)>3)*3)", "(!(((3+2*1)^!(3+2*1))>!(3*!2*2)))", "((((2+!1)*!3)v3)*!1)", "(!3*!(2+!(1+!3|1)|!(3^2))*!1)" };

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

pair<vector<vector<string>>, string> newGeneratorSKNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //нельзя создать больше членов, чем возможно при таком количестве переменных
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //нельзя вставить больше отрицаний, чем всего вхождений переменных в СКНФ
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1) * (2 * numOfVars - 1)) {   //нельзя просить длину меньше самой СКНФ
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
    while (toAdd > onFormulaLevel / 10 && compMembSKNF.size() != 1) {
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

//Генератор формул
//Принимает максимальное количество членов СДНФ (могут появиться дубликаты, которые будут удалены), количество переменных, количество отрицаний в СДНФ
//и приблизительное суммарное количество переменных и операторов
pair<vector<vector<string>>, string> newGeneratorSDNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //проверки перед генерацией
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //нельзя создать больше членов, чем возможно при таком количестве переменных
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //нельзя вставить больше отрицаний, чем всего вхождений переменных в СКНФ
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1) * (2 * numOfVars - 1)) {   //нельзя просить длину меньше самой СКНФ
        throw invalid_argument("Too short approxSize");
    }


    vector<vector<string>> sdnf;    //в этот вектор поместим будущую скнф

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
        sdnf.push_back(member);
    }

    while (negationsSet < numOfNegations) {    //если установлено недостаточно отрицаний - добавим пока не наберём
        bool isReady = false;

        for (int i = 0; i < sdnf.size(); i++) {

            for (int j = 0; j < sdnf[i].size(); j++) {
                if (sdnf[i][j][0] != '!' && (rand() % 2)) {
                    negationsSet += 1;
                    if (negationsSet == numOfNegations) {
                        isReady = true;
                        break;
                    }
                    sdnf[i][j].insert(0, 1, '!');
                }
            }
            if (isReady)
                break;
        }
    }


    //теперь нужно удалить все дубликаты из скнф чтобы предоставить её на выход. А использовать далее можно и с дубликатами
    vector<vector<string>> standartizedSDNF;

    for (int i = 0; i < sdnf.size(); i++) {
        bool needToAdd = true;
        for (int j = 0; j < standartizedSDNF.size(); j++) {
            if (areMembersEqual(sdnf[i], standartizedSDNF[j])) {
                needToAdd = false;
            }
        }
        if (needToAdd) {
            standartizedSDNF.push_back(sdnf[i]);
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

    for (int i = 0; i < sdnf.size(); i++) {

        string ans;
        int toAdd = perMember;   //оставшееся количество символов, которое нужно набрать

        while (toAdd > perMember / 10) {   //пока не набрали хотя бы 90% от нужного, продолжаем

            for (int j = 0; j < sdnf[i].size(); j++) {

                if (toAdd <= perMember / 10) {
                    break;
                }

                int needComplicate = rand() % numOfVars;

                if (needComplicate == 1) {

                    int zeroOrOne = rand() % 2;

                    if (zeroOrOne == 1) {
                        ans = complicateConstant(3 + rand() % 2, numOfVars, 1);
                        sdnf[i][j] += "*" + ans;
                    }
                    else {
                        ans = complicateConstant(3 + rand() % 2, numOfVars, 0);
                        sdnf[i][j] += "+" + ans;
                    }
                    toAdd -= countVarsAndOperators(ans) + 1;
                    sdnf[i][j] = "(" + sdnf[i][j] + ")";
                }
            }
        }
    }


    //теперь проведём усложнение на уровне члена СДНФ. Будем запутывать переменные.

    //генератор случайных чисел, нужен для перемешивания элементов между собой
    random_device rd;
    default_random_engine rng(rd());

    int onLevel2 = onMemberLevel / ceilNumOfMembers;    //столько должнен набрать каждый член СДНФ

    for (int i = 0; i < sdnf.size(); i++) {


        int toAdd = onLevel2;

        while (toAdd > onLevel2 / 10 && sdnf[i].size() != 1) {

            shuffle(sdnf[i].begin(), sdnf[i].end(), rng);   //запутаем элементы между собой

            string newMember;
            int lastIndex = sdnf[i].size() - 1;


            //тут используется равенство x*y = (x+y)*(!x+y)*(x+!y)
            newMember = "((" + sdnf[i][0] + "+" + sdnf[i][lastIndex] + ")*(!(" + sdnf[i][0] + ")+" + sdnf[i][lastIndex] + ")*(" + sdnf[i][0] + "+!(" + sdnf[i][lastIndex] + ")))";
            //newMember = "(" + sknf[i][0] + "^" + sknf[i][lastIndex] + "^" + sknf[i][0] + "*" + sknf[i][lastIndex] + ")";
            toAdd -= countVarsAndOperators(sdnf[i][0]) + countVarsAndOperators(sdnf[i][lastIndex]) + 7;
            sdnf[i].pop_back();
            sdnf[i].erase(sdnf[i].begin());
            sdnf[i].push_back(newMember);
        }
    }

    //теперь усложнённые члены соединим в строки
    vector <string> compMembSDNF;
    for (int i = 0; i < sdnf.size(); i++) {
        string buf = "(";
        for (int j = 0; j < sdnf[i].size(); j++) {
            buf.append(sdnf[i][j]);
            buf += "*";
        }
        buf[buf.size() - 1] = ')';
        compMembSDNF.push_back(buf);
    }


    //теперь проведём усложнение на уровне формулы, запутывая члены СДНФ. Между членами стоит +

    int toAdd = onFormulaLevel;  //осталось добрать


     //Применяется равенство x+y=x^y^x*y 
    while (toAdd > onFormulaLevel / 10 && compMembSDNF.size() != 1) {
        shuffle(compMembSDNF.begin(), compMembSDNF.end(), rng);
        string buf;
        int lastIndex = compMembSDNF.size() - 1;
        buf = "(" + compMembSDNF[0] + "^" + compMembSDNF[lastIndex] + "^" + compMembSDNF[0] + "*" + compMembSDNF[lastIndex] + ")";
        //buf = "((" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(!" + compMembSKNF[0] + "+" + compMembSKNF[lastIndex] + ")*(" + compMembSKNF[0] + "+!" + compMembSKNF[lastIndex] + "))";
        toAdd -= 2 * countVarsAndOperators(compMembSDNF[0]) + 2 * countVarsAndOperators(compMembSDNF[lastIndex]) + 3;
        compMembSDNF.pop_back();
        compMembSDNF.erase(compMembSDNF.begin());
        compMembSDNF.push_back(buf);
    }

    //наконец, соединим всё в одну строку
    string answer;

    for (int i = 0; i < compMembSDNF.size(); i++) {
        answer += compMembSDNF[i];
        answer += "+";
    }
    answer.pop_back();

    return make_pair(standartizedSDNF, answer);
}
