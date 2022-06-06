#pragma once
#include "main.h"
#include "tree.h"

//структура - член — Ќ‘(—ƒЌ‘)
//представлена двум€ битовыми шкалами
//если i-й бит в первой шкале установлен в 1, значит переменна€ уже внесена в комбинацию
//втора€ шкала хранит информацию о знаке переменной в бите на i-ом месте. (0 - нет отрицани€, 1- есть отрицание)
struct sknfMember {
    bitset <MAX_VARS> vars;    //шкала переменных
    bitset <MAX_VARS> signs;   //шкала знаков переменных
};

//глобальные переменные, которые использует функци€ поиска
//объ€влены глобально, т.к. требуетс€ лишь по одному экзмепл€ру каждой в любой момент времени
//----------------------------------------------------------------------
void sknfSearch(bool wantedValue, list<sknfMember>& lst, node* node);
void sdnfSearch(bool wantedValue, list<sknfMember>& lst, node* node);
extern list<sknfMember>::iterator it1;        //итератор дл€ перемещени€ по списку комбинаций
extern stack <list<sknfMember>> globalStack;   //глобальный стек дл€ сохранени€ данных, пришедших из родител€
list<list<short>> listOfBitsetsToListOfShorts(list<sknfMember> lst);
