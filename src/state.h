#ifndef STATE_H
#define STATE_H

#include <iostream>
//#include <vector>
#include "cell.h"
#include "debug.h"
#include "../lib/EasyBMP.h"

class State
{
public:
    //~State();
    State(int dimension);
    void clear();
    void StateCopy(State source);
    int getIndex(int i, int j);
    Cell getCell(int index);
    void Release();
    void StateToBitmap();


    int dim;
    Cell* cells;
    BMP OutImage;
    //std::vector<int> cells;
};

#endif