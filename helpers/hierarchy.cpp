//
// Created by Manuel Blanco Valentin on 1/10/21.
//

#include "hierarchy.h"
#include <iostream>
#include <fstream>
#include "gui.h"
#include "basic.h"
#include <regex>

std::map<std::string, std::string> Hierarchy::get_sources(){
    return __serial_sources__;
};

std::map<std::string, std::vector<std::string>> Hierarchy::get_instances(){
    return __serial_instances__;
};

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
bool Hierarchy::serialize_hierarchy(const Json::Value& val,  std::string carry, std::string carry_ref, std::string parent_src){

    bool is_entity = true;
    std::vector<std::string> blacklist = {"ref","arrayspan", "bitspan", "type", "value",
                                          "parameters", "sourcefile_"};

    switch (val.type()) {
        case Json::objectValue: {
            if (!val.empty()) {

                std::vector<std::string> keys = val.getMemberNames();
                for (size_t i=0; i<keys.size(); i++) {

                    const std::string& key = keys[i];

                    is_entity &= (std::find(blacklist.begin(),blacklist.end(),key) == blacklist.end());

                    std::vector<std::string> inst_refs;
                    std::string carry_tmp = (!carry.empty() ? carry + "." + key : key);
                    std::string carry_ref_tmp3 = (!carry_ref.empty() ? carry_ref + "." + key : key);
                    std::string carry_ref_tmp, carry_ref_tmp2 = carry_ref;

                    // Go deeper if you can
                    if (is_entity){
                        std::string ref = "";
                        if (val[key].type() == Json::objectValue) {
                            std::vector<std::string> nkeys = val[key].getMemberNames();

                            if (std::find(nkeys.begin(), nkeys.end(), "ref") != nkeys.end()) {
                                ref = val[key]["ref"].asString();
                            }

                            carry_ref_tmp = (!carry_ref.empty() ? carry_ref + "." + ref : ref);
                            carry_ref_tmp2 = (!carry.empty() ? carry + "." + ref : ref);

                        }
                        inst_refs.push_back(ref);
                        inst_refs.push_back(carry_tmp);
                        inst_refs.push_back(carry_ref_tmp);
                        inst_refs.push_back(carry_ref_tmp2);
                        inst_refs.push_back(carry_ref_tmp3);
                        __serial_instances__.insert(std::pair<std::string, std::vector<std::string>>(carry_tmp,inst_refs));
                        //std::cout << carry_tmp << " " << carry_ref_tmp << " " << carry_ref_tmp2 << " " << carry_ref_tmp3 << std::endl;

                        std::string next_src = parent_src;
                        if (std::find(keys.begin(),keys.end(),"sourcefile_") != keys.end()){
                            next_src = val[key]["sourcefile_"].asString();
                            //__serial_sources__.insert(std::pair<std::string,std::string>(carry, next_src));
                        }
                        if (!parent_src.empty()) {
                            __serial_sources__.insert(std::pair<std::string,std::string>(carry, parent_src));
                        }

                        serialize_hierarchy(val[key], carry_tmp, carry_ref_tmp, next_src);

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

    if (!patterns.empty()) std::cout << "[INFO] - Looking for patterns to create subset" << std::endl;

    // Loop thru patterns
    for (auto& pat: patterns){
        // Check if exists
        const std::regex r(pat);

        // Loop thru __serialized instances__
        for (const auto& pair: __serial_instances__){
            std::smatch sm;
            std::string ent = pair.first;
            std::vector<std::string> refs = pair.second;

            std::string rf = refs[0];
            std::vector<std::string> Rs = {refs.begin()+1,refs.end()};

            for (const auto& ref: Rs){
                if (regex_search(ref, sm, r)) {
                    std::cout << string_format("\tPattern %s found in hierarchy in instance %s with reference %s",pat.c_str(),ent.c_str(),rf.c_str()) << std::endl;

                    std::string ent_tmp = ent;
                    for (int i=1; i< sm.size(); i++) {
                        ent_tmp.replace(sm.position(i),sm.position(i)+sm[i].str().size(),"$");
                    }

                    if (!out_subset.contains(ent_tmp)){
                        out_subset.insert(std::pair<std::string, std::vector<std::string>>(ent_tmp,{ent}));
                    } else {
                        out_subset.at(ent_tmp).push_back(ent);
                    }
                    break;
                }
            }

        }
    }

    if (out_subset.empty()) std::cout << "No patterns found in subset" << std::endl;

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

