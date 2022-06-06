#pragma once
#include "main.h"
#include "sknfSdnfMembers.h"
extern list<sknfMember>::iterator it1;        
extern stack <list<sknfMember>> globalStack;
void printWantedAnswer(vector<vector<string>> vec);
void printActualAnswerSKNF(list<sknfMember> ans);
void printActualAnswerSDNF(list<sknfMember> ans);
void printSDNFInFile(list<sknfMember> ans, string exception);
void printSKNFInFile(list<sknfMember> ans, string exception);
bool compareAnswers(list<list<short>> actualAns, vector<vector<string>> wantedAns);
bool checkUserInput(string str);
void rpnPrintActualAnswer(vector<bitset<NEW_ALGO_MAX_VARS>>& vec, int flag);
void rpnPrintInFile(vector<bitset<NEW_ALGO_MAX_VARS>>& vec, string& expression, int flag);
void printAnswerHint();