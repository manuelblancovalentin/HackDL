//
// Created by mbv6231 on 4/9/21.
//

#include <string>
#include <fstream>
#include <random>
#include "design.h"


void triplicate_modules(int TMR, std::string TMR_SUFFIX, std::vector<std::vector<std::string>>& subsets){

    // Init out module def and instance
    std::string task_str;

    // Parse type of TMR
    if (TMR == 3){
        // Check the modules we are supposed to modify
        std::cout << "Patterns to triplicate:\n";
        for (auto& s: subsets){
            for (auto& ss: s){
                std::cout << "\t" << ss.c_str() << "\n";
            }
        }
    } else {
        std::cout << "[ERROR] - TMR class unrecognized or not implemented yet: " << TMR << std::endl;
    }

}


