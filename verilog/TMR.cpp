//
// Created by mbv6231 on 4/9/21.
//

#include <string>
#include <fstream>
#include <random>
#include "design.h"

void triplicate_modules(int TMR,
                        std::vector<std::vector<std::string>>& subsets,
                        std::map<std::string, std::string> __serial_instances__,
                        std::map<std::string, std::string> __serial_sources__,
                        std::string TMR_SUFFIX = "TMR",
                        std::string OUTPATH = ""){

    // Init out module def and instance
    std::string task_str;

    std::map<std::string,std::vector<std::string>> mod_files;

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

                    // mod reference
                    std::string ref = __serial_instances__[ss];

                    // Which we will replace with
                    std::string rep = ref + TMR_SUFFIX;

                    // Let's copy the file
                    std::string out = src.substr(src.find_last_of(PATH_SEPARATOR)+1);
                    size_t ext = out.find_last_of(".");
                    std::string extension = out.substr(ext+1);
                    out = out.substr(0,ext);
                    std::string outpath = OUTPATH + PATH_SEPARATOR + out + TMR_SUFFIX + "." + extension;

                    bool first_time = (mod_files.find(outpath) == mod_files.end());
                    if (std::filesystem::exists(outpath) & first_time){
                        std::filesystem::remove(outpath);
                    }

                    if (first_time){
                        std::filesystem::copy(src, outpath);
                        std::cout << "\t\tWriting TMR file to " << outpath << std::endl;
                        mod_files.insert(std::pair<std::string,std::vector<std::string>>(outpath,{}));
                    }

                    // Check if we have already executed this instruction for this file
                    std::string cmd = "sed -i '' -e 's/" + ref + "/" + rep + "/g' " + outpath;

                    bool notexec = true;
                    for (const auto& el: mod_files[outpath]){
                        notexec &= (el != cmd);
                    }

                    if (notexec){
                        std::cout << "\t\tProceeding to replace instance " << ref << " with " << rep << std::endl;
                        std::cout << "\t\tExecuting command " << cmd << std::endl;
                        system(cmd.c_str());
                        mod_files[outpath].push_back(cmd);
                    } else {
                        std::cout << "\t\tInstance " << ref << " was previously replaced with " << rep << std::endl;
                    }
                }
            }
        }
    } else {
        std::cout << "[ERROR] - TMR class unrecognized or not implemented yet: " << TMR << std::endl;
    }

}


