#include "automaton.h"


Automaton::Automaton(State prev_state, State act_state, int duration, int distance):
actual_state(act_state),
previous_state(prev_state),
sim_stats_(),
generation_(0),
duration_(duration),
distance_(distance),

lichen_creation_delay_(0),
lichen_initial_(0),
lichen_growth_rate_(0.0),
moss_dispersal_(0.0),
lichen_dispersal_(0.0),
growth_constant_(0.0),
cell_min_vacant_time_(0),
minimum_slump_weight_(0),
moss_maximum_weight_(0),
slump_constant_(0.0),
slump_max_area_(0),
slump_slip_(0.0),
slump_index_(0),
rand_dev(),
generator(rand_dev()),
real_distr_0_1(0, 1)

{

}

void Automaton::SetParams(int lichen_creation_delay, int lichen_initial, double lichen_growth_rate, double growth_constant,
        double moss_dispersal, double lichen_dispersal, int cell_min_vacant_time,
        int minimum_slump_weight, int moss_maximum_weight, 
        double slump_constant, int slump_max_area, double slump_slip)
{
    lichen_creation_delay_ = lichen_creation_delay;
    lichen_initial_ = lichen_initial;
    lichen_growth_rate_ = lichen_growth_rate;
    growth_constant_ = growth_constant;
    moss_dispersal_ = moss_dispersal;
    lichen_dispersal_ = lichen_dispersal;
    cell_min_vacant_time_ = cell_min_vacant_time;
    minimum_slump_weight_ = minimum_slump_weight;  
    moss_maximum_weight_ = moss_maximum_weight; 
    slump_constant_ = slump_constant; 
    slump_max_area_ = slump_max_area; 
    slump_slip_ = slump_slip;

}


void Automaton::Run()
{

    LOG("Run")   
    while(generation_ < duration_)
    {
        if ( generation_ == lichen_creation_delay_)
        {
            GenerateLichen();
        }
        Update();
        GetStats();
        generation_++;
        previous_state.StateCopy(actual_state);
        actual_state.clear();
        PrintGeneration();
    }
    LOG("Finished")
    previous_state.StateToBitmap();
}



void Automaton::Update()
{
    //LOG("Update")

    for (int i = 0; i < previous_state.dim; i++)
    {
        for(int j = 0; j < previous_state.dim; j++)
        {
            int cellIndex = actual_state.getIndex(i, j);
            double moss_growth_probability = GrowthProbability(GetNeighbours(i, j, distance_, 1));
            double slump_growth_probability = GrowthProbability(GetNeighbours(i, j, distance_, 3));
            double lichen_growth_probability = GrowthProbability(GetNeighbours(i, j, distance_, 2));

            double slump_creation_probability = slump_constant_ * previous_state.cells[cellIndex].weight;
            double chance = getRandomDouble();

            actual_state.cells[cellIndex].status = previous_state.cells[cellIndex].status;
            actual_state.cells[cellIndex].slump_id =  previous_state.cells[cellIndex].slump_id;
            actual_state.cells[cellIndex].weight =  previous_state.cells[cellIndex].weight;
            actual_state.cells[cellIndex].time_vacant =  previous_state.cells[cellIndex].time_vacant;
            actual_state.cells[cellIndex].isSlump =  previous_state.cells[cellIndex].isSlump;

            //Moss expansion grows only if not infected
            if((previous_state.cells[cellIndex].status == 0) && 
                    ((chance <= moss_growth_probability) ||
                    ((chance <= moss_dispersal_) && (previous_state.cells[cellIndex].time_vacant > cell_min_vacant_time_))))
            {
                //std::cout << "expand" << std::endl;
                actual_state.cells[cellIndex].status = 1;
                actual_state.cells[cellIndex].time_vacant = 0;    
            }

            //Slump creation
            if(((previous_state.cells[cellIndex].status == 1) || (previous_state.cells[cellIndex].status == 2)) && 
                (chance <= slump_creation_probability))
            {
                //std::cout << "Slump created" << std::endl;
                actual_state.cells[cellIndex].isSlump = true;
                actual_state.cells[cellIndex].time_vacant = 0;
            }

            //Slump expansion
            if(((previous_state.cells[cellIndex].status == 1) || (previous_state.cells[cellIndex].status == 2)) && 
                    ((chance <= slump_growth_probability) ))
            {
                actual_state.cells[cellIndex].isSlump = true;
                actual_state.cells[cellIndex].time_vacant = 0;  
            }

            //Lichen expansion grows on moss only and cell is vacant for time
            if(previous_state.cells[cellIndex].status == 1)
            {
                if((chance <= lichen_growth_probability) || 
                ((chance  <= lichen_dispersal_) && (previous_state.cells[cellIndex].time_vacant > cell_min_vacant_time_)))
                {
                    actual_state.cells[cellIndex].status = 2;
                    actual_state.cells[cellIndex].time_vacant = 0;       
                }
            }
    
            //Weight incrementation by growth
            if (((actual_state.cells[cellIndex].status == 1) || (actual_state.cells[cellIndex].status == 3)) && 
                    (previous_state.cells[cellIndex].weight < moss_maximum_weight_))
            {
                actual_state.cells[cellIndex].weight += 1;
            }

            //Vacant time incrementation
            if (actual_state.cells[cellIndex].status == 0)
            {
                actual_state.cells[cellIndex].time_vacant += 1;
            }

            if (actual_state.cells[cellIndex].isSlump == true)
            {
                if (actual_state.cells[cellIndex].slump_id == 0)
                {
                    assignSlumps(i, j);
                }
                    
                checkSlumps(i, j);
                
            }
        } //for i     
    } // for j
    SlumpsFalloff();
}

void Automaton::GenerateLichen()
{
    std::vector<int> moss_population;
    std::vector<int>::iterator it;
    
    //Get moss    
    for (int i = 0; i < previous_state.dim; i++)
    {
        for(int j = 0; j < previous_state.dim; j++)
        {
            int cellIndex = previous_state.getIndex(i, j);
            if (previous_state.cells[cellIndex].status == 1)
            {
                moss_population.push_back(cellIndex);
            }
        }
    }

    //Generate Lichen
    for (int i = 0; i < lichen_initial_; ++i)
    {
        std::uniform_int_distribution<> n_distr(0, moss_population.size());
        int random_index = n_distr(generator);
        int previous_slumpid = previous_state.cells[moss_population[random_index]].slump_id;
        previous_state.cells[moss_population[random_index]].status = 2;
        previous_state.cells[moss_population[random_index]].slump_id = previous_slumpid;
        previous_state.cells[moss_population[random_index]].time_vacant = 0;
    }
}

int Automaton::GetNeighbours(int origin_x, int origin_y, int distance, int attr)
{
    int sum = 0;
    int x;
    int y;
    for ( int i = -distance; i <= distance; i++)
    {
        for ( int j = -distance; j <= distance; j++)
        {
            x = origin_x + j;
            y = origin_y + i;
            if (((x >= 0) && (x < previous_state.dim)) && ((y >= 0) && (y < previous_state.dim)) && ((i != 0) || (j != 0)))
            {
                if ( attr == 3 )
                {
                    if (previous_state.getCell(previous_state.getIndex(x, y)).isSlump == true)
                    {
                        sum += 1;
                    }   
                }
                else
                {
                    if (previous_state.getCell(previous_state.getIndex(x, y)).status == attr)
                    {
                        sum += 1;
                    }
                }                
            }
        }
    }
    return sum;
}

double Automaton::GrowthProbability(int neighbours)
{
    double fraction = neighbours/8.0;
    double probability = MIN(1.0, growth_constant_ * fraction); 
    return probability;
}

void Automaton::SlumpRemove(State state, int remove)
{
    std::vector<Slump>::iterator it;
    for (int i = 0; i < (state.dim * state.dim); ++i)
    {
        if (state.cells[i].slump_id == remove)
        {
            //std::cout << i;
            state.cells[i].slump_id = 0;
            state.cells[i].status = 0;
            state.cells[i].weight = 0;
            state.cells[i].time_vacant = 0;
            state.cells[i].isSlump = false;
        }
    }

    for (it = slumps.begin(); it != slumps.end(); ++it) 
    {
        if(it->id == remove)
        {
            it->size = 0;
        }
    }

    //return old index back
    patch_indexes_.push_back(remove);
    //sort array
    if(!std::is_sorted(patch_indexes_.begin(), patch_indexes_.end()))
    {
        std::sort(patch_indexes_.begin(), patch_indexes_.end());
    }
}

void Automaton::SlumpReplace(State state, int original, int replaced)
{
    int total = 0;
    std::vector<Slump>::iterator it;

    //Replace slump ID
    for (int i = 0; i < (state.dim * state.dim); ++i)
    {
        if (state.cells[i].slump_id == original)
        {
            //std::cout << i;
            state.cells[i].slump_id = replaced;
            total++;
        }
    }

    //Reassign size
    for (it = slumps.begin(); it != slumps.end(); ++it) {
        if(it->id == original)
        {
            it->size = 0;
        }
        if(it->id == replaced)
        {
            it->size += total;
        }
    }

    //return old index back
    patch_indexes_.push_back(original);
    //sort array
    if(!std::is_sorted(patch_indexes_.begin(), patch_indexes_.end()))
    {
        std::sort(patch_indexes_.begin(), patch_indexes_.end());
    }
}

int Automaton::slumpNeighbours(int origin_x, int origin_y)
{
    int group_id = 0;
    Cell neighbour_cell;
    
    //For all neighbours
    for ( int i = -1; i <= 1; i++)
    {
        for ( int j = -1; j <= 1; j++)
        {
            int x = origin_x + j;
            int y = origin_y + i;

            // Check whether neighbour is valid
            if (((x >= 0) && (x < actual_state.dim)) && ((y >= 0) && (y < actual_state.dim)) && ((i != 0) || (j != 0)))
            {
                neighbour_cell  = actual_state.getCell(actual_state.getIndex(x, y));

                //IF neighbor cell is infected and is in patch
                if ((neighbour_cell.isSlump == true)  && neighbour_cell.slump_id != 0)
                {  
                    //only when groups are differen  
                    if(group_id != neighbour_cell.slump_id)
                    {                  
                        //nno change
                        if(group_id == 0)
                        {
                            group_id = neighbour_cell.slump_id;
                        }
                    }
                }// if in patch    
            }//if neighbour is valid
        }//for j
    }//for i
    return group_id;
}

void Automaton::assignSlumps(int origin_x, int origin_y)
{
    int neighbourid = slumpNeighbours(origin_x, origin_y);
        if(neighbourid != 0){
            //Assign"
            actual_state.cells[actual_state.getIndex(origin_x, origin_y)].slump_id = neighbourid;  
            std::vector<Slump>::iterator it;
            for (it = slumps.begin(); it != slumps.end(); ++it) {
                if(it->id == neighbourid)
                {
                it->size++;
                }
            }
        }
        else
        { //No group found, make new one
            CreateSlump(actual_state, origin_x, origin_y);
        }
}

void Automaton::MossMerge()
{
    for (int i = 0; i < actual_state.dim; i++)
    {
        for(int j = 0; j < actual_state.dim; j++)
        {
            checkSlumps(i,j);
        }
    }

}

void Automaton::SlumpsFalloff()
{   
    Cell actCell;
    Cell checkCell;
    int found = 0;
    int total = 0;
    std::vector<int> checked_slumps_;

    // For all cells
    for (int i = 0; i < actual_state.dim; i++)
    {
        for(int j = 0; j < actual_state.dim; j++)
        {
            actCell = actual_state.getCell(actual_state.getIndex(i, j));
            
            //If cell si in slump
            if (actCell.slump_id != 0)
            {
                //Check whether cells was parsed
                for (unsigned int i=0; i<checked_slumps_.size();i++){
                    if (checked_slumps_[i] == actCell.slump_id)
                    {
                        found = 1;
                    }
                }
                //If it was not parse it
                if(!found)
                {
                    total = 0;
                    //For all cells
                    for (int l = 0; l < actual_state.dim; l++)
                    {
                        for(int m = 0; m < actual_state.dim; m++)
                        {
                            checkCell = actual_state.cells[actual_state.getIndex(l, m)];
                            //Count number of cells in slump
                           if(checkCell.slump_id == actCell.slump_id)
                           {
                            total++;
                           }
                        }
                    }

                    //Check total slump size
                    if (total > slump_max_area_)
                    {
                        SlumpRemove(actual_state, actCell.slump_id);
                    }
                    checked_slumps_.push_back(actCell.slump_id);
                }
            }
        }
    }

}

void Automaton::checkSlumps(int origin_x, int origin_y)
{
    int origin_id = actual_state.getCell(actual_state.getIndex(origin_x, origin_y)).slump_id;
    Cell neighbour_cell;
    for ( int i = -1; i <= 1; i++)
    {
        for ( int j = -1; j <= 1; j++)
        {
            int x = origin_x + j;
            int y = origin_y + i;

            // Check whether neighbour is valid
            if (((x >= 0) && (x < actual_state.dim)) && ((y >= 0) && (y < actual_state.dim)) && ((i != 0) || (j != 0)))
            {
                neighbour_cell  = actual_state.getCell(actual_state.getIndex(x, y));

                //IF neighbor cell is infected and is in patch
                if ((neighbour_cell.isSlump == true)  && neighbour_cell.slump_id != 0)
                {  
                    //Iba pokial je rozdiel medzi skupinami  
                    if(origin_id != neighbour_cell.slump_id)
                    {
                        SlumpReplace(actual_state, neighbour_cell.slump_id, origin_id);
                    }
                }// if in patch    
            }//if neighbour is valid
        }//for j
    }//for i
}

void Automaton::CreateSlump(State state,int origin_x, int origin_y)
{
    slump_index_++;
    int cellid;
    std::vector<Slump>::iterator it;
          
    //If there is unused index
    if(!patch_indexes_.empty())
    {
        state.cells[state.getIndex(origin_x, origin_y)].slump_id = patch_indexes_.back();  
        cellid = patch_indexes_.back();
        patch_indexes_.pop_back();

        for (it = slumps.begin(); it != slumps.end(); ++it) {
            if (it->id == cellid)
            {
                it->size = 1;
                break;
            }

        }
    }
    else //Get new index
    {
        state.cells[state.getIndex(origin_x, origin_y)].slump_id = slump_index_;  
        Slump s = Slump(slump_index_);
        s.size = 1;
        slumps.push_back(s);
    }
}

double Automaton::MAX(double val1, double val2)
{
    return (val1 > val2 ? val1 : val2);
}

double Automaton::MIN(double val1, double val2)
{
    return (val1 < val2 ? val1 : val2);
}

int Automaton::Stop(int code)
{
    previous_state.Release();
    actual_state.Release();
    return code;
}

double Automaton::getRandomDouble()
{   
    return real_distr_0_1(generator);
}

void Automaton::GetStats()
{
    int slumptotal = 0;
    int mosstotal = 0;
    int lichentotal = 0;
    std::vector<Slump>::iterator it;
    for (it = slumps.begin(); it != slumps.end(); ++it) {
        slumptotal += it->size;
    }
    
    for(int i = 0; i < actual_state.dim; i++)
    {
        for(int j = 0; j < actual_state.dim; j++)
        {
            Cell actCell = actual_state.getCell(actual_state.getIndex(i, j));
            if (actCell.status == 1)
            {
                mosstotal++;
            }
            else if (actCell.status == 2)
            {
                lichentotal++;
            }
        }
    }
    Stats s = Stats();
    s.slump_total = slumptotal;
    s.moss_amount = mosstotal;
    s.lichen_amount = lichentotal; 
    sim_stats_.push_back(s);
}

void Automaton::PrintGeneration()
{
    std::cout << "Generation: " << generation_ << std::endl;
}

void Automaton::PrintAutomaton()
{
    PrintGeneration();
    std::cout << "Actual state: " << std::endl;
    PrintState(actual_state, 1);
    std::cout << "Previous state: " << std::endl;
    PrintState(previous_state, 1);
}


