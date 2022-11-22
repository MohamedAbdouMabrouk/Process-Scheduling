#ifndef command_h
#define command_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <queue>
#include <math.h>
using namespace std;

typedef struct Process
{
    char name[20];
    char *out;
    int start;
    int finish;
    int counter;
    int len;
    int index;
    float ratio;
    int level;
} Process;

struct CompareLen
{
    bool operator()(Process const &p1, Process const &p2)
    {
        if (p2.counter == p1.counter)
        {
            return p2.start < p1.start;
        }
        return p2.counter < p1.counter;
    }
};

struct ComparePriority
{
    bool operator()(Process const &p1, Process const &p2)
    {
        if(p2.counter == p1.counter)
            return p2.level > p1.level;
        return p2.counter > p1.counter;
    }
};

struct CompareRatio
{
    bool operator()(Process const &p1, Process const &p2)
    {
        if (p2.ratio == p1.ratio)
        {
            return p2.start < p1.start;
        }
        return p2.ratio > p1.ratio;
    }
};

void switchTask(int task, int max, int length, int pNumber, Process **processes);
void printTrace(int length, int type, int max, int pNumber, Process **processes);
void printStats(int length, int type, int max, int pNumber, Process **processes);
void FCFS(int length, int pNumber, Process **processes);
void RR(int length, int pNumber, Process **processes, int max);
void SPN(int length, int pNumber, Process **processes);
void SRT(int length, int pNumber, Process **processes);
void HRRN(int length, int pNumber, Process **processes);
void FB_1(int length, int pNumber, Process **processes);
void FB_2i(int length, int pNumber, Process **processes);
void AGING(int length, int pNumber, Process **processes, int max);
void init(char *tORs, char *type, int *length, int* pNumber);
void initProc(int* pNumber, int* length, Process **processes);
void clear(int pNumber, int length, Process **processes);
char *translate(int task);
void freeEm(int *pNumber, Process **processes);

#endif
