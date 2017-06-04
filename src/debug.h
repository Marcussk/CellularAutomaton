#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

// MACRO to toggle debugging on or off
// comment it out to toggle it off
#define DEBUG

//debug MACRO definition
#ifdef DEBUG
#define LOG(message) std::cout << message << std::endl;
#else
#define LOG(message)  
#endif


#endif