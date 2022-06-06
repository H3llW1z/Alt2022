#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <cstdlib>
#include <time.h>
#include <random>
#include <iterator>
#include <list>
#include <stack>
#include <bitset>
#include <fstream>
using namespace std;


#define MAX_VARS 15     //������������ ���������� ���������� � �������. ����� ������������� (C++ �� ����� ���������� ������������ ������� ����, ��������� �������� ����.
//���� ������� ����� ������� ���������� ������������ ����, ��������� ����� ���������� � �� ��������������. ��� ���� �������� ����������.


//��-�� ����, ��� ������� ����� �����������, ���������� ������ ������������� ������. ��� �������� ����� ������, ���� ��� ����� ������ ���������� ��� ����� ������� �������
#define NEW_ALGO_MAX_VARS 200
#define NEW_ALGO_STACK_LIMIT 50000

//����� ��� ������ ������ ������������ �� ��� (���� ��� ����)
#define FLAG_SKNF -25 
#define FLAG_SDNF -50
//���� ������ ������������ ����� ������. �� ����� ��� ����������� ���������� ���������
//������ ���������� - �������� ������������� ��������� ����� 2. ������ - �������� ��������. ��� ����� ������� � ������ ���������� ���������.
//����� ����������� ���������, �.�. ��� ��������� ����������
#ifndef _MAIN_H_
#define _MAIN_H_
const short operands[7][2][7] = {
    { { 3, 1,0, 0,1, 0,0 }, { 1, 1,1, -1,-1, -1,-1 } },   // 2 ���������

    { { 1, 0,0, -1,-1, -1,-1 }, { 3, 0,1, 1,0, 1,1 } },   // 3 ��������

    { { 2, 0,0, 1,1, -1,-1 }, { 2, 0,1, 1,0, -1,-1 } },   // 4 XOR

    { { 1, 1,0, -1,-1, -1,-1 }, { 3, 1,1, 0,0, 0,1 } },   // 5 ����������

    { { 2, 0,0, 1,1, -1,-1 }, { 2, 0,1, 1,0, -1,-1 } },   // 6 ������������

    { { 1, 1,1, -1,-1, -1,-1 }, { 3, 1,0, 0,1, 0,0 } },   // 7 ����� �������

    { { 3, 0,1, 1,0, 1,1 }, { 1, 0,0, -1,-1, -1,-1 } },   // 8 ������� �����
};
#endif
