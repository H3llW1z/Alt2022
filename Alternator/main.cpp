#include "sknfSdnfMembers.h"
#include "main.h"
#include "tree.h"
#include "expressionFunctions.h"
#include "generators.h"
#include "OPZalgorithm.h"
#include "helperFunctions.h"

list<sknfMember>::iterator it1;        //итератор для перемещения по списку комбинаций
stack <list<sknfMember>> globalStack;   //глобальный стек для сохранения данных, пришедших из родителя
int numOfVars = -1;   //хранит количество переменных в формуле, нужно для ограничения рабочей части битовой шкалы калькулятора ОПЗ
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

int main() {
    setlocale(LC_ALL, "rus");
    char choice;
    cout << "Выберите источник входных данных:\n";
    cout << "1.Использовать генератор;\n";
    cout << "2.Ввести формулу с клавиатуры;\n";
    cout << "\nО выводе ответа:\n" <<
        "Каждая строка ответа - один член СКНФ (СДНФ)\n" <<
        "Знаки между членами и переменными опущены для упрощения чтения.\nЕсли ищем СКНФ, то между переменными дизъюнкция (+), между членами - конъюнкция (*).\nЕсли ищем СДНФ - наоборот.\n";
    cin >> choice;
    while (choice != '1' && choice != '2') {
    cout << "Такого пункта нет. Введите 1 или 2\n";
    cin >> choice;
    }  
    switch (choice) {   //выбор источника ввода
    case '1':    //используя генератор
    {
        srand(time(NULL));
        int numOfMembers = -1;
        int numOfVariables = -1;
        int numOfNegations = -1;
        int approxSize = -1;
        bool everythingFine = true;

        cout << "Введите параметры формулы:\n" <<
            "1.К-во членов* СДНФ (СДНФ);\n" <<
            "2.К-во уникальных переменных в формуле;\n" <<
            "3.К-во отрицаний у переменных в итоговой СДНФ (СКНФ);\n" <<
            "4.Примерное количество вхождений переменных и операторов в формулу (их сумма).\n" <<
            "* - при генерации могут возникнуть одинаковые комбинации. Дубликаты существующих комбинаций будут удалены перед показом ожидаемого ответа.\n" <<
            "Формат ввода: <к-во членов> <к-во переменных> <к-во отрицаний> <желаемое к-во переменных и операторов>\n";

        do {
            cin >> numOfMembers;
            cin >> numOfVariables;
            cin >> numOfNegations;
            cin >> approxSize;

            if (numOfMembers <= 0 || numOfVariables <=0 || numOfNegations <=0 || approxSize <= 0) {
                cout << "Ни один из параметров не может быть отрицательным или равняться нулю.\n";
                everythingFine = false;
            }
            else if (numOfNegations > numOfMembers * numOfVariables) {
                cout << "Количество отрицаний привышает допустимое. Отрицаний в СКНФ (СДНФ) не может быть больше, чем (к-во переменных)*(к-во членов)\n";
                everythingFine = false;
            }
            else if (approxSize < (2 * numOfVariables - 1) * (2 * numOfMembers - 1)) {
                cout << "Желаемое к-во переменных и опеаторов в формуле меньше допустимого в данном случае. Генератор усложняет СКНФ (СДНФ). Её длина - минимальная допустимая.\n" <<
                    "Формула, по которой выполняется проверка: (2 * <к-во переменных> - 1)*(2 * <к-во членов> - 1)\n";
                everythingFine = false;
            }
            if (!everythingFine) {
                cout << "Пожалуйста, повторите ввод.\n";
            }
        } while (!everythingFine);

        cout << "Какую форму вы хотите найти:\n";
        cout << "1.СКНФ;\n";
        cout << "2.СДНФ;\n";
        cin >> choice;
        while (choice != '1' && choice != '2' && choice != '3') {
            cout << "Такого пункта нет. Введите 1 или 2\n";
            cin >> choice;
        }
        pair <vector<vector<string>>, string> answer;
        switch (choice) {    //выбор формы
        case '1':
        {
           answer = newGeneratorSKNF(numOfMembers, numOfVariables, numOfNegations, approxSize);
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
           cout << "Какой алгоритм использовать?\n";
           cout << "1.Основанный на рекурсии\n";
           cout << "2.Основанный на обратной польской записи\n";
           cin >> choice;
           while (choice != '1' && choice != '2' && choice != '3') {
               cout << "Такого пункта нет. Введите 1 или 2\n";
               cin >> choice;
           }
           switch (choice) { //выбор алгоритма
           case '1':
           {
               list<sknfMember> resultSKNF;
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
               cout << "Куда вывести ответ?\n" <<
                   "1.На экран;\n" <<
                   "2.В файл;\n" <<
                   "3.На экран и в файл;\n";
               cin >> choice;
               while (choice != '1' && choice != '2' && choice != '3') {
                   cout << "Введите 1, 2 или 3.\n";
                   cin >> choice;
               }
               if (choice == '1') {
                   printActualAnswerSKNF(resultSKNF);
               }
               if (choice == '2') {
                   printSKNFInFile(resultSKNF,answer.second);
               }
               if (choice == '3') {
                   printActualAnswerSKNF(resultSKNF);
                   printSKNFInFile(resultSKNF, answer.second);
               }
           }; break;
           case '2':
           {
               node* root = new node;
               cout << "Начинается поиск.\n";
               std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
               addnode(answer.second, root);
               cout << "Дерево построено\n";
               vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SKNF);
               std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
               cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
               list<list<short>> buf;
               for (int i = 0; i < ans.size(); i++) {
                   list<short> buf2;
                   for (int j = 0; j < numOfVars; j++) {
                       ans[i].test(j) ? buf2.push_back(-(j + 1)) : buf2.push_back(j + 1);
                   }
                   buf.push_back(buf2);
               }
               cout << "Равны ли ответы: ";
               cout << compareAnswers(buf, answer.first) << endl;
               cout << "Куда вывести ответ?\n" <<
                   "1.На экран;\n" <<
                   "2.В файл;\n" <<
                   "3.На экран и в файл;\n";
               cin >> choice;
               while (choice != '1' && choice != '2' && choice != '3') {
                   cout << "Введите 1, 2 или 3.\n";
                   cin >> choice;
               }
               if (choice == '1') {
                   rpnPrintActualAnswer(ans, FLAG_SKNF);
               }
               if (choice == '2') {
                   rpnPrintInFile(ans, answer.second, FLAG_SKNF);
               }
               if (choice == '3') {
                   rpnPrintActualAnswer(ans, FLAG_SKNF);
                   rpnPrintInFile(ans, answer.second, FLAG_SKNF);
               }
           }; break;
           }
        }; break;
        case '2':
        {
            answer = newGeneratorSDNF(numOfMembers, numOfVariables, numOfNegations, approxSize);
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

            cout << "Какой алгоритм использовать?\n";
            cout << "1.Основанный на рекурсии\n";
            cout << "2.Основанный на обратной польской записи\n";
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '3') {
                cout << "Такого пункта нет. Введите 1 или 2\n";
                cin >> choice;
            }

            switch (choice) { //выбор алгоритма
            case '1':
            {
                list<sknfMember> resultSDNF;
                sknfMember buf;
                buf.signs.set(0, 1);
                resultSDNF.push_back(buf);
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(answer.second, root);
                cout << "Дерево построено\n";
                distToLeaf(root);
                cout << "Рёбра отмечены\n";
                sdnfSearch(1, resultSDNF, root);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Равны ли ответы: " << compareAnswers(listOfBitsetsToListOfShorts(resultSDNF), answer.first) << endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {

                    printActualAnswerSKNF(resultSDNF);
                }
                if (choice == '2') {
                    printSKNFInFile(resultSDNF, answer.second);
                }
                if (choice == '3') {
                    printActualAnswerSKNF(resultSDNF);
                    printSKNFInFile(resultSDNF, answer.second);
                }
            }; break;
            case '2':
            {
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(answer.second, root);
                cout << "Дерево построено\n";
                vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SDNF);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                list<list<short>> buf;
                for (int i = 0; i < ans.size(); i++) {
                    list<short> buf2;
                    for (int j = 0; j < numOfVars; j++) {
                        ans[i].test(j) ? buf2.push_back(j + 1) : buf2.push_back(-(j + 1));
                    }
                    buf.push_back(buf2);
                }
                cout << "Равны ли ответы: ";
                cout << compareAnswers(buf, answer.first) << endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    rpnPrintActualAnswer(ans, FLAG_SDNF);
                }
                if (choice == '2') {
                    rpnPrintInFile(ans, answer.second, FLAG_SDNF);
                }
                if (choice == '3') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                    rpnPrintInFile(ans, answer.second, FLAG_SDNF);
                }
            }; break;
            }
        }; break;
        }

    }; break;
    case '2':    //ввод с клавиатуры
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

        cout << "Введите вашу формулу:\n";
        string calculate;
        cin >> calculate;
        while (!checkUserInput(calculate)) {
            cout << "Вы ввели некорректное выражение. Пожалуйста, проверьте его и исправьте ошибки.\n";
            cin >> calculate;
        }

        cout << "Какую форму вы хотите найти:\n";
        cout << "1.СКНФ;\n";
        cout << "2.СДНФ;\n";
        cin >> choice;
        while (choice != '1' && choice != '2') {
            cout << "Такого пункта нет. Введите 1 или 2\n";
            cin >> choice;
        }
        switch (choice) {  //выбор формы
        case '1':
        {
            cout << "Какой алгоритм использовать?\n";
            cout << "1.Основанный на рекурсии\n";
            cout << "2.Основанный на обратной польской записи\n";
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '3') {
                cout << "Такого пункта нет. Введите 1 или 2\n";
                cin >> choice;
            }
            
            switch (choice) {
            case '1':
            {
                list<sknfMember> resultSKNF;
                sknfMember buf;
                buf.signs.set(0, 1);
                resultSKNF.push_back(buf);
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                distToLeaf(root);
                cout << "Рёбра отмечены\n";
                sknfSearch(0, resultSKNF, root);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    printActualAnswerSKNF(resultSKNF);
                }
                if (choice == '2') {
                    printSKNFInFile(resultSKNF, calculate);
                }
                if (choice == '3') {
                    printActualAnswerSKNF(resultSKNF);
                    printSKNFInFile(resultSKNF, calculate);
                }
            }; break;
            case '2':
            {
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SKNF);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                }
                if (choice == '2') {
                    rpnPrintInFile(ans, calculate, FLAG_SKNF);
                }
                if (choice == '3') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                    rpnPrintInFile(ans, calculate, FLAG_SKNF);
                }
            }; break;
            }


        }; break;
        case '2':
        {
            cout << "Какой алгоритм использовать?\n";
            cout << "1.Основанный на рекурсии\n";
            cout << "2.Основанный на обратной польской записи\n";
            cin >> choice;
            while (choice != '1' && choice != '2' && choice != '3') {
                cout << "Такого пункта нет. Введите 1 или 2\n";
                cin >> choice;
            }
            switch (choice) {
            case '1':
            {
                list<sknfMember> resultSDNF;
                sknfMember buf;
                buf.signs.set(0, 1);
                resultSDNF.push_back(buf);
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                distToLeaf(root);
                cout << "Рёбра отмечены\n";
                sdnfSearch(1, resultSDNF, root);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    printActualAnswerSKNF(resultSDNF);
                }
                if (choice == '2') {
                    printSKNFInFile(resultSDNF, calculate);
                }
                if (choice == '3') {
                    printActualAnswerSKNF(resultSDNF);
                    printSKNFInFile(resultSDNF, calculate);
                }
            }; break;
            case '2':
            {
                node* root = new node;
                cout << "Начинается поиск.\n";
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                addnode(calculate, root);
                cout << "Дерево построено\n";
                vector<bitset<NEW_ALGO_MAX_VARS>> ans = rpnCalcutator(root, FLAG_SDNF);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Поиск завершён. Затрачено времени: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " секунд" << std::endl;
                cout << "Куда вывести ответ?\n" <<
                    "1.На экран;\n" <<
                    "2.В файл;\n" <<
                    "3.На экран и в файл;\n";
                cin >> choice;
                while (choice != '1' && choice != '2' && choice != '3') {
                    cout << "Введите 1, 2 или 3.\n";
                    cin >> choice;
                }
                if (choice == '1') {
                    rpnPrintActualAnswer(ans, FLAG_SDNF);
                }
                if (choice == '2') {
                    rpnPrintInFile(ans, calculate, FLAG_SDNF);
                }
                if (choice == '3') {
                    rpnPrintActualAnswer(ans, FLAG_SKNF);
                    rpnPrintInFile(ans, calculate, FLAG_SDNF);
                }
            }; break;
            }
        }; break;
        }
    }; break;
    }

    cout << "Нажмите любую кнопку чтобы завершить работу прогарммы\n";
    _getch();
    return 0;
}