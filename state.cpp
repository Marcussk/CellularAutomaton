#include "state.h"


State::State(int dimension):
OutImage()
{
    dim = dimension;
    cells = new Cell[dimension*dimension];
}


void State::clear()
{
    for (int i = 0; i < (dim * dim); ++i)
    {

        this->cells[i].status = 0;
        this->cells[i].slump_id = 0;
        this->cells[i].time_vacant = 0;
        this->cells[i].weight = 0;
        this->cells[i].isSlump = false;
    }
}

void State::StateCopy(State source)
{
    for (int i = 0; i < (dim*dim); ++i)
    {
        this->cells[i].isSlump = source.cells[i].isSlump;
        this->cells[i].status = source.cells[i].status;
        this->cells[i].slump_id = source.cells[i].slump_id;
        this->cells[i].time_vacant = source.cells[i].time_vacant;
        this->cells[i].weight = source.cells[i].weight;
    }
}

int State::getIndex(int i, int j)
{
    return (i * dim + j);
}

void State::Release()
{
    delete[] cells;
    cells = NULL;
}

Cell State::getCell(int index)
{
    return this->cells[index]; 
}

void State::StateToBitmap()
{
    Cell actCell;
    OutImage.SetSize(dim, dim);
    // Set its color depth to 32-bits
    OutImage.SetBitDepth(32);
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {   
            actCell = this->getCell(getIndex(j, i));
            if (actCell.status == 0)
            {
                OutImage(i,j)->Red = 255;
                OutImage(i,j)->Green = 255;
                OutImage(i,j)->Blue = 255;
            }
            else if (actCell.status == 1)
            {
                OutImage(i,j)->Red = 128;
                OutImage(i,j)->Green = 128;
                OutImage(i,j)->Blue = 128;                
            }
            else
            {
                OutImage(i,j)->Red = 0;
                OutImage(i,j)->Green = 0;
                OutImage(i,j)->Blue = 0;   
            }

            OutImage(i,j)->Alpha = 0;
        }
    }
    OutImage.WriteToFile("Output.bmp");
}


