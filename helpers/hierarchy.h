//
// Created by Manuel Blanco Valentin on 1/10/21.
//

#ifndef HLSPY_HIERARCHY_H
#define HLSPY_HIERARCHY_H

#include <iostream>
#include "../jsoncpp/json/json.h"
#include "gui.h"

class Hierarchy: public Json::Value {
private:
    //Json::Value __hierarchy__;
    Json::Value __hierarchy__;
    std::vector<std::string> __serial_instances__;

    // Tree
    std::string __tree__;

    int __print__(Json::Value& h, std::string FILEPATH);

public:

    // Method to save to json
    int save_json(std::string FILEPATH);
    int load_json(std::string FILEPATH);

    // Method to print tree
    void print(void) {std::cout << __hierarchy__; std::cout << std::endl; }
    int print_txt(std::string FILEPATH) {return __print__(__hierarchy__,FILEPATH);}

    // Constructor
    Hierarchy(Json::Value h = new Json::Value()): __hierarchy__(h) {};

    // Method to find subset according to pattern
    std::vector<std::vector<std::string>> subset(std::vector<std::string> patterns);
    bool serialize_hierarchy(const Json::Value& val, std::string carry = "");
};


#endif //HLSPY_HIERARCHY_H
