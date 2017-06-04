#include "cell.h"

Cell::Cell(int init_status):
status(init_status),
slump_id(0),
time_vacant(0),
isSlump(false)
{
    if (init_status == 1)
    {
        weight = 1;
    }
    else
    {
        weight = 0;
    }
}

//When structure is altered PrintState, Clear, StateCopy shpuld be adjusted too