#ifndef CELL_H
#define CELL_H

#include <iostream>

class Cell
{
public:
    Cell(){}
    Cell(int init_status);
    

    int status;
    int slump_id;
    int weight;
    int time_vacant;
    bool isSlump;
};
//Status:
//0 - vacant
//1 - moss
//2 - lichen

//If changed alter update, clear, statecopy

#endif