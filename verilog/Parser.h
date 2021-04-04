//
// Created by Manuel Blanco Valentin on 1/6/21.
//

#ifndef HLSPY_PARSER_H
#define HLSPY_PARSER_H

#include <string>
#include "../helpers/gui.h"
#include "../helpers/basic.h"
#include "blocks.h"
#include "../helpers/flags.h"

// Parser class
class Parser {
private:

    // __parse__ function
    VerilogBlock __parse__(std::vector <VerilogBlock>& module_definitions,
                           std::map<std::string,VerilogBlock>& module_references,
                           std::vector <std::string>& sources,
                           std::vector <std::string>& lib,
                           std::map<std::string,std::vector<std::string>>& orphan_instances,
                           std::ifstream& stream,
                           progressBar& pbar,
                           int& start_line,
                           std::vector <std::string>& ancestors,
                           std::vector <std::string>& children,
                           std::string previous_line = "",
                           std::string TAB = "",
                           std::string NAME = "block",
                           std::string REF = "");

    VerilogBlock __parse_file__(std::vector <VerilogBlock>& module_definitions,
                                std::map<std::string,VerilogBlock>& module_references,
                                std::vector <std::string>& sources,
                                std::vector <std::string>& lib,
                                std::map<std::string,std::vector<std::string>>& orphan_instances,
                                std::string FILENAME,
                                std::string NAME);

    // Function to count number lines
    static int __count_number_lines__(const char *FILENAME);

    // Create flag objects
    FLAGS flags;

public:
    // Inputs for initializer
    std::string FILENAME;
    std::string TAB;
    std::string NAME = "block";

    // Number of lines
    int num_lines = -1;

    // Line offset
    int line_offset = 0;

    // Initialization
    Parser(std::string FILENAME = "",
           std::string NAME = "block",
           FLAGS* flags = new FLAGS()):
                FILENAME(FILENAME),
                NAME(NAME),
                flags(*flags){}

    // Functionalities
    VerilogBlock parse(std::vector<VerilogBlock>& module_definitions,
                       std::map<std::string,VerilogBlock>& module_references,
                       std::vector<std::string> &sources,
                       std::vector<std::string> &lib,
                       std::map<std::string,std::vector<std::string>>& orphan_instances) {return __parse_file__(module_definitions,
                                                                              module_references,
                                                                              sources,
                                                                              lib,
                                                                              orphan_instances,
                                                                              FILENAME,
                                                                              NAME);}

};

bool LineChecker(std::string mainStr, std::vector<std::string> patterns, int n_patterns);


template<class InputIterator, class T>
InputIterator find_module (InputIterator first, InputIterator last, const T& val)
{
    while (first!=last) {
        if (&(*first)==val) return first;
        ++first;
    }
    return last;
}

template<class InputIterator, class T>
InputIterator find_module_def (InputIterator first, InputIterator last, const T& val)
{
    while (first!=last) {
        if ((*first).name==val) return first;
        ++first;
    }
    return last;
}

#endif //HLSPY_PARSER_H
