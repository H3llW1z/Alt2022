#pragma once
#include "main.h"
#include "expressionFunctions.h"
int countVarsAndOperators(string str);
string complicateConstant(int numOfVars, int numOfVarsTotal, bool constFlag);
bool areMembersEqual(vector<string> a, vector<string> b);
pair<vector<vector<string>>, string> newGeneratorSKNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize);
pair<vector<vector<string>>, string> newGeneratorSDNF(int ceilNumOfMembers, int numOfVars, int numOfNegations, int approxSize);
