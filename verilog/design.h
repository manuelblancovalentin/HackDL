//
// Created by Manuel Blanco Valentin on 1/9/21.
//

#ifndef HLSPY_DESIGN_H
#define HLSPY_DESIGN_H

#include <string>
#include <vector>
#include "blocks.h"
#include "../jsoncpp/json/json.h"
#include "../helpers/hierarchy.h"
#include "../helpers/basic.h"
#include "Parser.h"

// Design class
class Design {

private:
    std::string __recursive_seeker__(VerilogBlock& vblock,
                              std::map<std::string,VerilogBlock> mod_references,
                              int TAB = 0,
                              const char* tab_str = "\t",
                              progressBar* pbar = new progressBar(),
                              int* counter = 0
                              );
    Hierarchy __hierarchy__ = Hierarchy(Json::Value());
    FLAGS* flags;

public:
    std::string NAME = "design";
    std::vector <std::string> SOURCES;
    std::vector <std::string> LIB;
    std::vector <VerilogBlock> ModuleDefinitions;

    // Get hierarchy definition
    Hierarchy get_hierarchy(void);

    // Constructor
    Design(std::string Name, std::vector<std::string> Sources,
           std::vector<std::string> Lib = {},
           FLAGS* __FLAGS__ = new FLAGS(FLAGS::NONE)){
        NAME = Name;
        SOURCES = Sources;
        LIB = Lib;
        flags = __FLAGS__;
    }
};


#endif //HLSPY_DESIGN_H
