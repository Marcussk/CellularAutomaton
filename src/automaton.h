#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include "state.h"
#include "utils.h"
#include "slump.h"
#include "stats.h"

class Automaton
{
public:
    Automaton(State prev_state, State act_state, int duration, int distance);
    void PrintGeneration();
    void PrintAutomaton();
    void Update();
    int GetNeighbours(int origin_x, int origin_y, int distance, int attr);
    double GrowthProbability(int neighbours);
    double MAX(double val1, double val2);
    double MIN(double val1, double val2);
    void Run();
    int Stop(int code);
    double getRandomDouble();
    void SetParams(int lichen_creation_delay, int lichen_initial, double lichen_growth_rate, double growth_constant,
        double moss_dispersal, double lichen_dispersal, int cell_min_vacant_time,
        int minimum_slump_weight, int moss_maximum_weight, 
        double slump_constant, int slump_max_area, double slump_slip);
    void GenerateLichen();
    void SlumpReplace(State state, int original, int replaced);
    //void Slumpfill(State state, int origin_x, int origin_y);
    void CreateSlump(State state, int origin_x, int origin_y);
    void assignSlumps(int origin_x, int origin_y);
    int slumpNeighbours(int origin_x, int origin_y);
    void checkSlumps(int origin_x, int origin_y);
    void MossMerge();
    void GetStats();
    void SlumpRemove(State state, int remove);
    void SlumpsFalloff();

    State actual_state;
    State previous_state;
    std::vector<Stats> sim_stats_;

private:
    int generation_;
    int duration_;
    int distance_;

    int lichen_creation_delay_;
    int lichen_initial_;
    double lichen_growth_rate_;
    double moss_dispersal_;
    double lichen_dispersal_;
    double growth_constant_;
    int cell_min_vacant_time_;
    int minimum_slump_weight_; 
    int moss_maximum_weight_; 
    double slump_constant_; 
    int slump_max_area_; 
    double slump_slip_;

    std::vector<Slump> slumps;
    std::vector<int> patch_indexes_;
    int slump_index_;

    std::random_device                  rand_dev;
    std::mt19937                        generator;
    std::uniform_real_distribution<>  real_distr_0_1;


};

#endif