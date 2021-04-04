//
// Created by Manuel Blanco Valentin on 1/6/21.
//

#include "gui.h"
#include <stdio.h>
#include <iostream>
#include "basic.h"

// Definitions
std::string PBARFILLCHAR = "█";
std::string PBAREMPTYCHAR = "░";

// Progress bar print progress
void progressBar::printProgress(int iter, std::string message, std::string prefix) {

    // Init variables
    double percentage = (double) iter/(double) progressBar::total;
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    //int rpad = PBWIDTH - lpad;
    std::string char_percentage;

    // Print beginning of line
    char_percentage = string_format("\r%s%3d%% [", prefix.c_str(), val);
    std::cout << char_percentage;

    // Print fill
    for (int i = 0; i < lpad; i++) std::cout << PBARFILLCHAR;
    // Print empty
    for (int i = lpad; i < PBWIDTH; i++) std::cout << PBAREMPTYCHAR;
    // Print end of line
    std::cout << "] - " << message;

    // Flush cout
    fflush(stdout);
}


std::string getJsonValue(const Json::Value &val){
    std::string val_key;

    if (val.isString()) {
        val_key = string_format("%s", val.asString().c_str());
    } else if (val.isBool()) {
        val_key = string_format("%d", val.asBool());
    } else if (val.isInt()) {
        val_key = string_format("%d", val.asInt());
    } else if (val.isUInt()) {
        val_key = string_format("%u", val.asUInt());
    } else if (val.isDouble()) {
        val_key = string_format("%f", val.asDouble());
    } else {
        val_key = string_format("%d", val.type());
    }

    return val_key;
}


// Json to tree
void tree(std::string& str, const Json::Value& val, std::string TAB, bool first, std::string sep){

    // Set some params
    int n = 3;

    switch (val.type()) {
        case Json::nullValue: str += sep + "null"; break;
        case Json::booleanValue: str += sep + (val.asBool() ? "true" : "false"); break;
        case Json::intValue: str += sep; str += val.asLargestInt(); break;
        case Json::uintValue: str += sep; str += val.asLargestUInt(); break;
        case Json::realValue: str += sep; str += val.asDouble(); break;
        case Json::stringValue: str += sep; str += '"' + val.asString() + '"'; break;
        case Json::arrayValue: {
            Json::ArrayIndex size = val.size();
            if (size == 0)
                str += " : []";
            else {
                str += " : [";
                std::string newIndent = TAB + "\t";
                for (Json::ArrayIndex i=0; i<size; i++) {
                    //Indent(ofs, newIndent);
                    //str += TAB;
                    tree(str, val[i], newIndent, first, sep = " ");
                    str += (i + 1 == size ? "" : ",");
                }
                //str += TAB;
                str += ']';
            }
            break;
        }
        case Json::objectValue: {
            if (val.empty())
                str += "{}";
            else {

                //str += !first ?  "├" : " "; //"{\n"
                std::vector<std::string> keys = val.getMemberNames();
                for (size_t i=0; i<keys.size(); i++) {

                    // Print depth.
                    std::string c = (i + 1 == keys.size() ? "└" : "├");

                    // This is not efficient at all, we could use string formatting expliciting
                    // the num of times we want to repeat a character, but I'm tired.
                    std::string hf, s;
                    for (int nk = 0; nk < n; nk++) hf += "-";
                    for (int nk = 0; nk < n; nk++) s += " ";

                    const std::string& key = keys[i];
                    str += (!first ? "\n" + TAB + c + hf : "");
                    str += key;

                    std::string c2 = (i + 1 == keys.size() ? " " : "│");
                    std::string tab_u = (!first ? TAB + c2 + s : " ");

                    tree(str, val[key], tab_u, false);
                    //str += "\n";
                    str += (i + 1 == keys.size() ? "\n" : "");
                }
                str += TAB;
                //str += '\n';
            }
            break;
        }
        default :
            std::cerr << "Wrong type!" << std::endl;
            exit(0);
    }
}




// Print tree

/*
std::string tree(const Json::Value &val, std::string TAB, bool last, bool first){

    // Init string
    std::string str = "";
    // Set some params
    int n = 3;
    int nn = 0;

    // Loop thru elements
    for (Json::Value::const_iterator itr = val.begin(); itr != val.end(); itr++) {

        // Print depth.
        std::string c;
        if (nn != (val.size() - 1)) {
            c = "├";
        } else {
            c = "└";
        }

        // This is not efficient at all, we could use string formatting expliciting
        // the num of times we want to repeat a character, but I'm tired.
        std::string hf, s;
        for (int nk = 0; nk < n; nk++) hf += "-";
        for (int nk = 0; nk < n; nk++) s += " ";

        std::string val_key = getJsonValue(itr.key());


        if (!first) {
            // Append to str
            str += "\n" + TAB + c + hf + val_key;
        } else {
            str += val_key;
        }

        std::string c2;
        if (nn == (val.size() - 1)) {
            c2 = " ";
        } else {
            c2 = "│";
        }

        std::string tab_u;

        if (!first) {
            tab_u = TAB + c2 + s;
        } else {
            tab_u += ' ';
        }

        // Recursivity
        std::size_t lopn = (*itr).size();
        if (lopn > 0) {
            str += tree(*itr, tab_u, last & (nn == (val.size() - 1)), false);
        } else {
            str += " : " + getJsonValue(*itr);
        }

        // Increase nn
        nn++;
    }


    return str;
}
*/