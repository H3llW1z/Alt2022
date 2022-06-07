#include "generators.h"
extern int numOfVars;
//������� ���������� ��������� � ������ ���������� � ���������� 
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
        //������� �������������� ����������, ������ 0 ��� ���� �������
        string variantsList[4] = { "(!(1^((!2+3)*!2+1)>3)*3)", "(!(((3+2*1)^!(3+2*1))>!(3*!2*2)))", "((((2+!1)*!3)v3)*!1)", "(!3*!(2+!(1+!3|1)|!(3^2))*!1)" };

        //������� 3 ��������� ���������� �� ���������
        string var1 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var2 = "a" + to_string(rand() % numOfVarsTotal + 1);
        string var3 = "a" + to_string(rand() % numOfVarsTotal + 1);

        //������� �������� ���� �� ������ ����������
        int variant = rand() % 4;

        string buf = variantsList[variant];
        for (int i = 0; i < buf.size(); i++) {   //� ������� ��������� ���������� �� ��������� 
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

        if (constFlag) {    //���� ����� ������������� �������, ������� ���������
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

//���������, �������� �� ����� ���� ����������� �� �������, ����� ��� �������� ���������� ��� ������ � �������� ���������� ������
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

//��������� ������
//��������� ������������ ���������� ������ ���� (����� ��������� ���������, ������� ����� �������), ���������� ����������, ���������� ��������� � ����
//� ��������������� ��������� ���������� ���������� � ����������

pair<vector<vector<string>>, string> newGeneratorSKNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //�������� ����� ����������
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //������ ������� ������ ������, ��� �������� ��� ����� ���������� ����������
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //������ �������� ������ ���������, ��� ����� ��������� ���������� � ����
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1) * (2 * numOfVars - 1)) {   //������ ������� ����� ������ ����� ����
        throw invalid_argument("Too short approxSize");
    }


    vector<vector<string>> sknf;    //� ���� ������ �������� ������� ����

    int negationsSet = 0;    //���������� ��� ������������� ������ ���������


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

    while (negationsSet < numOfNegations) {    //���� ����������� ������������ ��������� - ������� ���� �� ������
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


    //������ ����� ������� ��� ��������� �� ���� ����� ������������ � �� �����. � ������������ ����� ����� � � �����������
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

    //����� ���������� ���������� � ���������� ����� ��������,������� ��, ��� ��� �������� ���� ����.
    int needToAdd = approxSize - (2 * numOfVars - 1) * (2 * ceilNumOfMembers - 1) - negationsSet;

    //��� ����� � �������� ������. ��� ������������� ���������� ���������� ����� �� ������ ������.
    int onVarLevel = needToAdd / 4;
    int onMemberLevel = onVarLevel;
    int onFormulaLevel = needToAdd / 2;

    //���������� �� ������ ����������. � ���������� ��������� �������, ���������� �������������� ������ ��� ���������. (x=x*1 or x=x+0)

    int perMember = onVarLevel / ceilNumOfMembers;  // ����� ��������� ������ ������� ������ ���� � �������

    for (int i = 0; i < sknf.size(); i++) {

        string ans;
        int toAdd = perMember;   //���������� ���������� ��������, ������� ����� �������

        while (toAdd > perMember / 10) {   //���� �� ������� ���� �� 90% �� �������, ����������

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


    //������ ������� ���������� �� ������ ����� ����. ����� ���������� ����������.

    //��������� ��������� �����, ����� ��� ������������� ��������� ����� �����
    random_device rd;
    default_random_engine rng(rd());

    int onLevel2 = onMemberLevel / ceilNumOfMembers;    //������� ������� ������� ������ ���� ����

    for (int i = 0; i < sknf.size(); i++) {


        int toAdd = onLevel2;

        while (toAdd > onLevel2 / 10 && sknf[i].size() != 1) {

            shuffle(sknf[i].begin(), sknf[i].end(), rng);   //�������� �������� ����� �����

            string newMember;
            int lastIndex = sknf[i].size() - 1;

            //����������� ��������� x+y=x^y^x*y 

            newMember = "(" + sknf[i][0] + "^" + sknf[i][lastIndex] + "^" + sknf[i][0] + "*" + sknf[i][lastIndex] + ")";
            toAdd -= countVarsAndOperators(sknf[i][0]) + countVarsAndOperators(sknf[i][lastIndex]) + 3;
            sknf[i].pop_back();
            sknf[i].erase(sknf[i].begin());
            sknf[i].push_back(newMember);
        }
    }

    //������ ���������� ����� �������� � ������
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


    //������ ������� ���������� �� ������ �������, ��������� ����� ����. ����� ������� ����� *

    int toAdd = onFormulaLevel;  //�������� �������

    //��� ������������ ��������� x*y = (x+y)*(!x+y)*(x+!y)
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

    //�������, �������� �� � ���� ������
    string answer;

    for (int i = 0; i < compMembSKNF.size(); i++) {
        answer += compMembSKNF[i];
        answer += "*";
    }
    answer.pop_back();

    return make_pair(standartizedSKNF, answer);
}

//��������� ������
//��������� ������������ ���������� ������ ���� (����� ��������� ���������, ������� ����� �������), ���������� ����������, ���������� ��������� � ����
//� ��������������� ��������� ���������� ���������� � ����������
pair<vector<vector<string>>, string> newGeneratorSDNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize) {
    //�������� ����� ����������
    if (ceilNumOfMembers > pow(2, numOfVars)) {  //������ ������� ������ ������, ��� �������� ��� ����� ���������� ����������
        throw invalid_argument("Number of members bigger than possible");
    }

    if (numOfNegations > ceilNumOfMembers * numOfVars) {    //������ �������� ������ ���������, ��� ����� ��������� ���������� � ����
        throw invalid_argument("Number of negations can't be bigger than summary number of variables in PCNF");
    }

    if (approxSize < (2 * ceilNumOfMembers - 1) * (2 * numOfVars - 1)) {   //������ ������� ����� ������ ����� ����
        throw invalid_argument("Too short approxSize");
    }


    vector<vector<string>> sdnf;    //� ���� ������ �������� ������� ����

    int negationsSet = 0;    //���������� ��� ������������� ������ ���������
    


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

    while (negationsSet < numOfNegations) {    //���� ����������� ������������ ��������� - ������� ���� �� ������
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


    //������ ����� ������� ��� ��������� �� ���� ����� ������������ � �� �����. � ������������ ����� ����� � � �����������
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

    //����� ���������� ���������� � ���������� ����� ��������,������� ��, ��� ��� �������� ���� ����.
    int needToAdd = approxSize - (2 * numOfVars - 1) * (2 * ceilNumOfMembers - 1) - negationsSet;

    //��� ����� � �������� ������. ��� ������������� ���������� ���������� ����� �� ������ ������.
    int onVarLevel = needToAdd / 4;
    int onMemberLevel = onVarLevel;
    int onFormulaLevel = needToAdd / 2;

    //���������� �� ������ ����������. � ���������� ��������� �������, ���������� �������������� ������ ��� ���������. (x=x*1 or x=x+0)

    int perMember = onVarLevel / ceilNumOfMembers;  // ����� ��������� ������ ������� ������ ���� � �������

    for (int i = 0; i < sdnf.size(); i++) {

        string ans;
        int toAdd = perMember;   //���������� ���������� ��������, ������� ����� �������

        while (toAdd > perMember / 10) {   //���� �� ������� ���� �� 90% �� �������, ����������

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


    //������ ������� ���������� �� ������ ����� ����. ����� ���������� ����������.

    //��������� ��������� �����, ����� ��� ������������� ��������� ����� �����
    random_device rd;
    default_random_engine rng(rd());

    int onLevel2 = onMemberLevel / ceilNumOfMembers;    //������� ������� ������� ������ ���� ����

    for (int i = 0; i < sdnf.size(); i++) {


        int toAdd = onLevel2;

        while (toAdd > onLevel2 / 10 && sdnf[i].size() != 1) {

            shuffle(sdnf[i].begin(), sdnf[i].end(), rng);   //�������� �������� ����� �����

            string newMember;
            int lastIndex = sdnf[i].size() - 1;


            //��� ������������ ��������� x*y = (x+y)*(!x+y)*(x+!y)
            newMember = "((" + sdnf[i][0] + "+" + sdnf[i][lastIndex] + ")*(!(" + sdnf[i][0] + ")+" + sdnf[i][lastIndex] + ")*(" + sdnf[i][0] + "+!(" + sdnf[i][lastIndex] + ")))";
            //newMember = "(" + sknf[i][0] + "^" + sknf[i][lastIndex] + "^" + sknf[i][0] + "*" + sknf[i][lastIndex] + ")";
            toAdd -= countVarsAndOperators(sdnf[i][0]) + countVarsAndOperators(sdnf[i][lastIndex]) + 7;
            sdnf[i].pop_back();
            sdnf[i].erase(sdnf[i].begin());
            sdnf[i].push_back(newMember);
        }
    }

    //������ ���������� ����� �������� � ������
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


    //������ ������� ���������� �� ������ �������, ��������� ����� ����. ����� ������� ����� +

    int toAdd = onFormulaLevel;  //�������� �������


     //����������� ��������� x+y=x^y^x*y 
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

    //�������, �������� �� � ���� ������
    string answer;

    for (int i = 0; i < compMembSDNF.size(); i++) {
        answer += compMembSDNF[i];
        answer += "+";
    }
    answer.pop_back();

    return make_pair(standartizedSDNF, answer);
}
