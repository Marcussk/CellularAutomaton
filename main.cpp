#include "main.h"

using namespace std;
std::vector<int> patch_indexes;
std::vector<Slump> slumps;

void GenerateInitialState(State* t, int dimension, int initialMoss)
{
    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            t->cells[t->getIndex(i, j)] = Cell(0);
        }
    }

    const int range_from  = 0;
    const int range_to    = (dimension*dimension-1);
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(range_from, range_to);
    for (int k = 0; k < initialMoss; k++)
    {
        t->cells[distr(generator)] = Cell(1);
    }

}




bool arg_parse(int argc, char *argv[], std::string *in_file, int *experiment_count ){
	int params;
	bool file_set = false;

	while( (params = getopt(argc, argv, "i:n:")) != -1){
		switch(params){
			case 'i':
				*in_file = optarg;
				file_set = true;
				break;
			case 'n':
				*experiment_count = atoi(optarg);
				break;
			case '?':
				if(optopt == 'i' || optopt == 'n'){
					exit(1);
				} else {
					exit(1);
				}
			default:
				return false;
				exit(1);
		}
	}

	if(!file_set){
		cout << "No config file included." << endl;
		exit(1);
	} 

	return true;
}


int main(int argc, char *argv[])
{
    //Initialize program parameters
    int duration = -1;
    int dimension = -1;
    int distance = -1;
    int moss_initial = -1;
    int lichen_initial = -1;
    int lichen_creation_delay = -1;
    double lichen_growth_rate = -1;
    double growth_constant = -1.0;    
    double moss_dispersal = -1.0;
    double lichen_dispersal = -1.0;
    int min_vacanttime = -1;
    int min_weight = -1;
    int max_weight = -1;
    double slump_constant = -1.0;
    int max_area = -1;
    double slump_slip = -1.0;
    pugi::xml_document doc;

    //defalut 1
    int experiment_count = 1;
    std::string input_file;


    if(!arg_parse(argc, argv, &input_file, &experiment_count) ){
    	std::cout << "Incorrect parameters" << std::endl;	
        std::cout << "Usage: ./CA -i filename -n number";
    	exit(1);
    }
    std::cout << "Input file: " <<  input_file << endl;

    
    //load configuration File
    if (!doc.load_file(input_file.c_str()))
    {
        std::cerr << "Can not load config file" << std::endl;
        return(1);
    }
    pugi::xml_node configs = doc.child("Configuration");

    //load configuration parameters
    for (pugi::xml_node item = configs.first_child(); item; item = item.next_sibling())
    {
        std::string iname = item.name();
        if (iname.compare("duration") == 0){duration = atoi(item.first_child().value());}
        if (iname.compare("dimension") == 0){dimension = atoi(item.first_child().value());}
        if (iname.compare("distance") == 0){distance = atoi(item.first_child().value());}
        if (iname.compare("moss_initial") == 0){moss_initial = atoi(item.first_child().value());}
        if (iname.compare("lichen_initial") == 0){lichen_initial = atoi(item.first_child().value());}
        if (iname.compare("lichen_creation_delay") == 0){lichen_creation_delay = atoi(item.first_child().value());}
        if (iname.compare("lichen_growth_rate") == 0){lichen_growth_rate = atof(item.first_child().value());}
        if (iname.compare("growth_constant") == 0){growth_constant = atof(item.first_child().value());}
        if (iname.compare("moss_dispersal") == 0){moss_dispersal = atof(item.first_child().value());}
        if (iname.compare("lichen_dispersal") == 0){lichen_dispersal = atof(item.first_child().value());}
        if (iname.compare("min_vacanttime") == 0){min_vacanttime = atoi(item.first_child().value());}
        if (iname.compare("min_weight") == 0){min_weight = atoi(item.first_child().value());}
        if (iname.compare("max_weight") == 0){max_weight = atoi(item.first_child().value());}
        if (iname.compare("slump_constant") == 0){slump_constant = atof(item.first_child().value());}
        if (iname.compare("max_area") == 0){max_area = atoi(item.first_child().value());}
        if (iname.compare("slump_slip") == 0){slump_slip = atof(item.first_child().value());}
    }

    //Total stats for each time step accross all iterations
    std::vector<Stats> total_stats_;

    //Prefill values
    for (int i = 0; i <= duration; ++i)
    {
        Stats temp = Stats();
        temp.slump_total = 0.0;
        temp.moss_amount = 0.0;
        temp.lichen_amount = 0.0; 
        total_stats_.push_back(temp);
    }

    for (int l = 0; l < experiment_count; ++l)
    {
        //std::cout << "Experiment iteration " << l << std::endl;
        State previous_state(dimension);
        GenerateInitialState(&previous_state, dimension, moss_initial);
        State actual_state(dimension);
        actual_state.clear();


        Automaton CA(previous_state, actual_state, duration, distance);
        CA.SetParams(lichen_creation_delay, lichen_initial, lichen_growth_rate, growth_constant,
            moss_dispersal, lichen_dispersal, min_vacanttime,
            min_weight, max_weight, 
            slump_constant, max_area, slump_slip);

        CA.Run();

        //simulation stats accross duration
        //std::cout << "Simulation results:" << std::endl;
        for (unsigned int i=0; i<CA.sim_stats_.size();i++){
                //std::cout << "Step: " << i << std::endl;
                //std::cout << "Slump total: " << CA.sim_stats_[i].slump_total << std::endl;
                //std::cout << "Moss total: " << CA.sim_stats_[i].moss_amount << std::endl;
                //std::cout << "Lichen total: " << CA.sim_stats_[i].lichen_amount << std::endl;
            total_stats_[i].slump_total += CA.sim_stats_[i].slump_total;
            total_stats_[i].moss_amount += CA.sim_stats_[i].moss_amount;
            total_stats_[i].lichen_amount += CA.sim_stats_[i].lichen_amount;
        }
        
        CA.Stop(0);

    }

    std::cout << "Experiment results:" << std::endl;
    for (unsigned int i=0; i<total_stats_.size();i++){
        //Absolute count    
            total_stats_[i].slump_total /= experiment_count;
            total_stats_[i].moss_amount /= experiment_count;
            total_stats_[i].lichen_amount /= experiment_count;
        //Percentage
            total_stats_[i].slump_total = ((100*total_stats_[i].slump_total)/(dimension*dimension));
            total_stats_[i].moss_amount = ((100*total_stats_[i].moss_amount)/(dimension*dimension));
            total_stats_[i].lichen_amount = ((100*total_stats_[i].lichen_amount)/(dimension*dimension));

        std::cout << "Step: " << i << std::endl;
        //std::cout << "Slump total: " << total_stats_[i].slump_total << std::endl;
        std::cout << "Moss total: " << total_stats_[i].moss_amount << '%' << std::endl;
        std::cout << "Lichen total: " << total_stats_[i].lichen_amount << '%' << std::endl;
    }
    return 0;    
}