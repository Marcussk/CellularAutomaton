#include "utils.h"


void PrintState(State state, int param)
{
    std::cout << "Printing " << param << std::endl;
    for (int i = 0; i < state.dim; ++i)
    {
        std::cout << std::endl;
        for (int j = 0; j < state.dim; j++)
        {
            int actIndex = state.getIndex(i, j);
            switch (param)
            {
                case 1:
                    std::cout << state.cells[actIndex].status;
                    break;
                case 2:
                    std::cout << state.cells[actIndex].slump_id;
                    break;
                case 3:
                    std::cout << state.cells[actIndex].weight;
                    break;
                case 4:
                    std::cout << state.cells[actIndex].time_vacant;
                    break;
                case 5:
                    if (state.cells[actIndex].isSlump == true)
                    {
                        std::cout << 1;
                    }
                    else
                    {
                        std::cout << 0;
                    }
                    break;
                default:
                    std::cerr << "PrintState unknown parameter" << std::endl;
                    break;
            }
        }
    }

    std::cout << std::endl;
}

int getRandomInt(int min, int max)
{
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(min, max);
    return distr(generator);
}



int getRandom()
{
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(0, 100);
    return distr(generator);
}