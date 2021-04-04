//
// Created by Manuel Blanco Valentin on 1/10/21.
//

#include "hierarchy.h"
#include <iostream>
#include <fstream>
#include "gui.h"
#include "basic.h"
#include <regex>

// Load from json
int Hierarchy::load_json(std::string FILEPATH){
    std::ifstream json_file(FILEPATH, std::ifstream::binary);
    Json::Value root;
    json_file >> root;
    __hierarchy__ = root;

    // Remember to serialize
    serialize_hierarchy(__hierarchy__);

    // Print info
    std::cout << "[INFO] - Hierarchy successfully loaded from file " + FILEPATH << std::endl;

    return 0;
}

// Save to json method
int Hierarchy::save_json(std::string FILEPATH) {

    // Open stream
    std::ofstream file_id;
    file_id.open(FILEPATH);

    file_id << Hierarchy::__hierarchy__;

    // Create json writer to file and write
    //Json::StreamWriterBuilder builder;
    //std::unique_ptr<Json::StreamWriter> writer(
    //        builder.newStreamWriter());
    //writer->write(Hierarchy::__hierarchy__, &file_id);

    // Close file
    file_id.close();

    // Print to console
    std::cout << "[INFO] - Hierarchy successfully saved to file " + FILEPATH << std::endl;

    return 0;
}

// Parse Json
bool Hierarchy::serialize_hierarchy(const Json::Value& val, std::string carry){

    bool is_entity = false;

    switch (val.type()) {
        case Json::objectValue: {
            if (!val.empty()) {

                std::vector<std::string> keys = val.getMemberNames();
                for (size_t i=0; i<keys.size(); i++) {

                    const std::string& key = keys[i];

                    for (auto& k: std::vector<std::string>{"arrayspan", "bitspan", "type", "value"}){
                        is_entity |= (std::find(keys.begin(),keys.end(),k) != keys.end());
                    }

                    std::string carry_tmp = (!carry.empty() ? carry + "." + key : key);
                    if (serialize_hierarchy(val[key], carry_tmp)){
                        __serial_instances__.push_back(carry_tmp);
                    }

                }
            }
            break;
        }
    }
    return is_entity;
}

// Find subset
std::vector<std::vector<std::string>> Hierarchy::subset(std::vector<std::string> patterns){
    // Init output
    std::map<std::string, std::vector<std::string>> out_subset;

    std::cout << "[INFO] - Looking for patterns to create subset" << std::endl;


    // Loop thru patterns
    for (auto& pat: patterns){
        // Check if exists
        //std::regex rgx(pat);
        const std::regex r(pat);

        // Loop thru __serialized instances__
        for (auto const& ent: __serial_instances__){
            std::smatch sm;

            if (regex_search(ent, sm, r)) {
                std::cout << string_format("\tPattern %s found in hierarchy in instance %s",pat.c_str(),ent.c_str()) << std::endl;

                std::string ent_tmp = ent;
                for (int i=1; i< sm.size(); i++) {
                    //std::string ent_tmp = std::regex_replace (ent, sm[i].str(),"$");
                    //std::cout << sm[i] << " at " << sm.position(i) << std::endl;
                    ent_tmp.replace(sm.position(i),sm.position(i)+sm[i].str().size(),"$");
                }

                if (!out_subset.contains(ent_tmp)){
                    out_subset.insert(std::pair<std::string, std::vector<std::string>>(ent_tmp,{ent}));
                } else {
                    out_subset.at(ent_tmp).push_back(ent);
                }

            }
        }
    }

    std::vector<std::vector<std::string>> out;

    for (auto& k: out_subset){
        out.push_back(k.second);
    }

    return out;
}



// Print tree method
int Hierarchy::__print__(Json::Value& h, std::string FILEPATH = ""){

    // Get tree
    std::string s = __tree__;

    // Compile if empty
    if (s.empty()){

        // Call tree function on hierarchy
        tree(s, h, "", true);
        trim(s);

        // Store tree in Hierarchy object
        __tree__ = s;

    }

    // Print to console if filename is empty
    if (FILEPATH.empty()){
        std::cout << std::endl << s << std::endl;
    } else {
        std::ofstream of;
        of.open (FILEPATH);
        of << s;
        of.close();

        // Print to console
        std::cout << "[INFO] - Tree successfully saved to file " + FILEPATH << std::endl;

    }


    return 0;
}

