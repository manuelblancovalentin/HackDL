//
// Created by mbv6231 on 4/9/21.
//

#include <string>
#include <fstream>
#include <random>
#include "design.h"


void triplicate_modules(int TMR, std::vector<std::vector<std::string>>& subsets,
                        std::map<std::string, std::string> __serial_sources__,
                        std::string TMR_SUFFIX = "TMR"){

    // Init out module def and instance
    std::string task_str;

    // Parse type of TMR
    if (TMR == 3){
        // Check the modules we are supposed to modify
        std::cout << "Patterns to triplicate:\n";
        for (auto& s: subsets){
            for (auto& ss: s){
                std::cout << "\t" << ss.c_str() << "\n";
                if (__serial_sources__.find(ss) != __serial_sources__.end()){
                    // Pattern exists in sources, so let's get the source file
                    std::string src = __serial_sources__[ss];
                    std::cout << "\t\tFound at " << src.c_str() << std::endl;

                    // Get instance name
                    std::string token = ss.substr(ss.find_last_of('.') + 1);
                    // Which we will replace with
                    std::string rep = token + TMR_SUFFIX;

                    std::cout << "\t\tProceeding to replace instance " << token << " with " << rep << std::endl;

                }
            }
        }
    } else {
        std::cout << "[ERROR] - TMR class unrecognized or not implemented yet: " << TMR << std::endl;
    }

}


