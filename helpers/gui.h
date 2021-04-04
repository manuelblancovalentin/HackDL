//
// Created by Manuel Blanco Valentin on 1/6/21.
//

#ifndef HLSPY_GUI_H
#define HLSPY_GUI_H
#include <string>
#include "../jsoncpp/json/json.h"

// PROGRESS BAR DEFINITIONS
#define PBWIDTH 60

class progressBar {

private:
    void printProgress(int iter, std::string message, std::string prefix);
public:
    // Init vars
    int total;

    // Initialization
    progressBar(int t = 1): total(t) {}

    // Functions
    void update(int iter, std::string message, std::string prefix) {return printProgress(iter, message, prefix);}
};


// Print tree
void tree(std::string& s, const Json::Value& val, std::string TAB = "", bool first = false,
          std::string sep = " : ");

#endif //HLSPY_GUI_H
