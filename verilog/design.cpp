//
// Created by Manuel Blanco Valentin on 1/9/21.
//

#include "design.h"
#include "Parser.h"
#include "blocks.h"
#include <iostream>
#include <climits>
#include <filesystem>


// Get hierarchy method
Hierarchy Design::get_hierarchy() {

    // Initialize vlog blocks
    std::vector <VerilogBlock> vblocks;

    // Make sure ModuleDefinitions is empty
    std::vector <VerilogBlock> module_definitions = Design::ModuleDefinitions;

    // Module references maps
    std::map<std::string,VerilogBlock> module_references;

    // We will loop thru all filenames in source
    std::vector <std::string> src_left = Design::SOURCES;

    // Keep track of instances that have not been defined
    std::map<std::string,std::vector<std::string>> orphan_instances;

    // Append *.v and *.sv from LIBS to SOURCES
    for (auto dir : Design::LIB){
        for (const auto & entry : std::filesystem::directory_iterator(dir))
            if ((entry.path().extension() == ".sv") || (entry.path().extension() == ".v"))
            {
                bool found = false;
                for (auto src: src_left){
                    found |= (src == entry.path());
                }

                if (!found){
                    std::cout << "Adding file " << entry.path() << " to sources" << std::endl;
                    src_left.push_back(entry.path());
                }
            }

    }


    while (!src_left.empty()){
        // Get first element
        auto& it = src_left[0];
        // Init parser object
        Parser p(it,it,Design::flags);
        // Parse
        VerilogBlock vlogtmp = p.parse(module_definitions, module_references, src_left, Design::LIB, orphan_instances);

        // Pushback
        vblocks.push_back(vlogtmp);

        // Remove source we just analyzed
        src_left.erase(src_left.begin());

    }

    // Let's try to find the top_module(s) now
    std::vector<std::vector<int>> calls = {};
    int niters = 0;
    for (auto& vb: vblocks) {
        std::vector<int> call_tmp(vb.children.size(), 0);
        calls.push_back(call_tmp);
        niters += call_tmp.size();
    }

    // Create progressBar to display progress for unpatient asses like mine.
    progressBar pbar_top(niters-1);

    // Check how many times a module children appears in the other rest of
    std::cout << "\n[INFO] - Looking for top modules in module definitions..." << std::endl;
    int i_accum = 0;
    for (int i = 0; i < vblocks.size(); i++) {
        for (int k = 0; k < vblocks[i].children.size(); k++){
            std::string mtmp = string_format("Considering module %s as top-module candidate",vblocks[i].children[k].c_str());
            pbar_top.update(i_accum, mtmp ,"\t");

            // How many times do we find this moddef inside other modules children?
            // (I know it's a bit convoluted, but this is the same as asking if this is a children
            // of any other module)
            for (int j = 0; j < module_definitions.size(); j++){
                if (find(module_definitions[j].children.begin(), module_definitions[j].children.end(), vblocks[i].children[k]) != module_definitions[j].children.end()){
                    // We found this module name in the children list of another vblock.
                    // This means this is not a top_module. Add a count to it
                    calls[i][k] += 1;
                    break;
                }
            }
            i_accum += 1;
        }
    }

    // Print endline
    std::cout << std::endl;

    // Now find the module(s) with the least number of calls
    int min_recurrence = INT_MAX;
    for (auto& c: calls){
        for (auto& cc: c) {
            if (cc < min_recurrence) {
                min_recurrence = cc;
            }
        }
    }

    std::vector<TopModule> top_modules;
    for (int i = 0; i < calls.size(); i++){
        for (int j = 0; j < calls[i].size(); j++) {
            if (calls[i][j] == min_recurrence) {
                // Get module name
                std::string mod_ref_tmp = vblocks[i].children[j];

                // Look for ref in module_definitions
                // index_ref was of type std::__wrap_iter<VerilogBlock*>
                auto index_ref = find_module_def(module_definitions.begin(),
                                                 module_definitions.end(),
                                                 mod_ref_tmp);
                if (index_ref != module_definitions.end())
                {
                    std::string mtmp = string_format("\t\tModule %s set as top-module",mod_ref_tmp.c_str());
                    std::cout << mtmp << std::endl;

                    // Create temp top module
                    TopModule tptmp(mod_ref_tmp, &(*index_ref));

                    // Add to structure
                    top_modules.push_back(tptmp);

                }

            }
        }
    }

    // Print endline
    std::cout << std::endl;

    // Check for orphan modules
    for (auto& tp: top_modules) tp.REF->__fix_orphans__(module_references);

    // All top_modules are independent top_modules
    // With this, now we can build our design hierarchy. Let's loop thru top_modules.
    // Our hierarchy in this case will be a json object, which is extremely powerful
    // and useful for us rn
    //Json::Value hierarchy;

    std::string hierarchy = "{\n";
    hierarchy += "\t\"top_modules\":\n\t{\n";
    int TAB = 2;
    const char *tab_str = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

    std::string hSources = "{\n";

    int tpmmax = top_modules.size();
    for (int tpm = 0; tpm < tpmmax; tpm++){
        // Get vars
        std::string h_name = top_modules[tpm].name;
        VerilogBlock* REF (top_modules[tpm].REF);

        /*
         * Create hierarchy entry for this module
         */
        hierarchy += string_format("%.*s\"%s\" : \n%.*s{\n", TAB, tab_str, h_name.c_str(), TAB, tab_str);
        /*
         * Create source/definition entry for this module
         */
        hierarchy += REF->subhierarchy;
        // Append to hierarchy
        hierarchy += string_format("%.*s}", TAB, tab_str);
        if (tpm != (tpmmax - 1)){
            hierarchy += ",";
        }
        hierarchy += "\n";

    }

    // Close group
    hierarchy += "\t}\n";
    hierarchy += "}";


    //std::cout << hierarchy << std::endl;

    // Transform this to Json::Value
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( hierarchy.c_str(), root );     //parse process
    if ( !parsingSuccessful )
    {
        throw std::invalid_argument("Impossible to parse the json file to create hierarchy. Aborting.");
    }

    // Create actual hierarchy object
    Hierarchy h(root);

    // Remember to serialize
    h.serialize_hierarchy(root);

    // Set hierarchy in Design (private)
    Design::__hierarchy__ = h;

    // Return and exit
    return h;

}


// Implementation of the recursive seeker that we will use to create our hierarchy
std::string Design::__recursive_seeker__(VerilogBlock& vblock,
                                 std::map<std::string, VerilogBlock> module_references,
                                 int TAB,
                                 const char* tab_str,
                                 progressBar* pbar,
                                 int* counter) {

    // Init json block
    //Json::Value tmp;
    std::string hierarchy_tmp;

    // Check if this is a module or instance

    // Get block name
    std::string name = vblock.name;

    // Get reference
    std::string ref = vblock.ref;

    std::string message;

    // Get all fields
    //std::vector<Parameter>* tmp_params = &vblock.parameters;
    //Ports* tmp_ports = &vblock.ports;
    //std::vector<NetWire>* tmp_netwires = &vblock.netwires;
    std::map<std::string, std::vector<VerilogBlock>> tmp_instances = vblock.instances;

    // If the reference does not match the name, it means we have to try to look this up
    // in the module_definitions.

    if (std::strcmp(ref.c_str(), "module") == 0) {
        // This is a module definition. Probably the top module. This means we can keep using
        // it. We don't need to fetch anything. All parameters are here already.
        message = string_format("%.*sAdding %s to hierarchy", TAB, tab_str, vblock.name.c_str());
        pbar->total += tmp_instances.size();
        *counter += 1;
        pbar->update(*counter, message,"");


        // Place subhierarchy of module
        hierarchy_tmp += vblock.subhierarchy;

        // Now we can proceed and loop thru the instances
        // Every instance of this module will have the same definition
        // Only its name will change
        // We only have to apply recursivity 1 time PER MODULE DEFINITION
        for (std::map<std::string, std::vector<VerilogBlock>>::iterator it = tmp_instances.begin();
             it != tmp_instances.end(); it++) {
            std::string h_tmp_mod;

            if (module_references.contains(it->first)) {
                // Call recursivity
                h_tmp_mod = __recursive_seeker__(module_references.at(it->first), module_references,
                                                 TAB + 1, tab_str,
                                                 pbar, counter);

                // Now that we have the definition for the module (all recursive) let's loop thru all the instances
                // that use this module definition
                pbar->total += it->second.size();
                for (auto &inst: it->second) {
                    message = string_format("%.*sAdding %s to hierarchy", TAB+1, tab_str, inst.name.c_str());
                    *counter += 1;
                    pbar->update(*counter, message,"");

                    // Open instance def
                    hierarchy_tmp += string_format("%.*s\"%s\" : \n%.*s{\n", TAB, tab_str, inst.name.c_str(),
                                                   TAB, tab_str);

                    // Append module def
                    hierarchy_tmp += h_tmp_mod;

                    // Close inst def
                    hierarchy_tmp += string_format("%.*s},\n", TAB, tab_str);

                }
            } else {
                std::string prefix = string_format("%.*s\"", TAB, tab_str);
                std::string postfix = string_format("\" : {\n%.*s\"ref\": \"%s\"\n%.*s},\n",TAB, tab_str,
                                                    it->first.c_str(),
                                                    TAB, tab_str);

                pbar->total += it->second.size();
                for (auto &inst: it->second) {
                    hierarchy_tmp += prefix + inst.name + postfix;
                    *counter += 1;
                    message = string_format("%.*sAdding %s to hierarchy", TAB+1, tab_str, inst.name.c_str());
                    pbar->update(*counter, message,"");
                }
            }

        }

        // Discard last comma
        if (tmp_instances.size() > 0) hierarchy_tmp = hierarchy_tmp.substr(0, hierarchy_tmp.size() - 3) + "\n";


        // Close group
        hierarchy_tmp += "}\n";

    }

    return hierarchy_tmp;
}