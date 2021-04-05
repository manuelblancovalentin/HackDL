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

    int tpmmax = top_modules.size();
    for (int tpm = 0; tpm < tpmmax; tpm++){
        // Get name
        std::string h_name = top_modules[tpm].name;

        // Create entry for this top_module
        hierarchy += string_format("%.*s\"%s\" : \n%.*s{\n", TAB, tab_str, h_name.c_str(), TAB, tab_str);
        //hierarchy += Design::__recursive_seeker__(*(top_modules[tpm].REF), module_references, TAB + 1, tab_str,
        //                                          &pbar,
        //                                          &counter);
        //Json::Value tmp = Design::__recursive_seeker__(*(tpm.REF), module_definitions);
        hierarchy += (top_modules[tpm].REF)->subhierarchy;

        // Append to hierarchy
        hierarchy += string_format("%.*s}", TAB, tab_str);
        //hierarchy["top_modules"][h_name] = tmp;
        if (tpm != (tpmmax - 1)){
            hierarchy += ",";
        }
        hierarchy += "\n";

    }

    // Close group
    hierarchy += "\t}\n";
    hierarchy += "}";

    std::cout << hierarchy << std::endl;

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

    // Set hierarchy in Design (private)
    Design::__hierarchy__ = h;

    // Remember to serialize
    h.serialize_hierarchy(root);

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


        /*
        if (std::strcmp(ref.c_str(), "module") != 0){

            if (!module_references.contains(ref)) {
                // not found
                hierarchy += string_format("%.*s\"ref\": \"%s\"\n",
                                           TAB, tab_str,
                                           ref.c_str());

            } else {
                // found
                VerilogBlock md = module_references.at(ref);

                // Place subhierarchy of module
                hierarchy += md.subhierarchy;

                // Reassign instances
                //tmp_instances = md.instances;
            }

*/
            // Try to find ref in module_definitions
            /*
            for (auto& md: module_definitions){
                if (strcmp(md.name.c_str(), ref.c_str()) == 0){
                    // Reassign params
                    //tmp_params = &md.parameters;
                    // Reassign ports
                    //tmp_ports = &md.ports;
                    // Reassign netwires
                    //tmp_netwires = &md.netwires;
                    // Reassign instances
                    tmp_instances = &md.instances;

                    // Place subhierarchy of module
                    hierarchy += (&md)->subhierarchy;
                }
            }
            */
/*
        } else {
            VerilogBlock md = module_references.at(name);

            // Place subhierarchy of module
            hierarchy += md.subhierarchy;

            // Reassign instances
            tmp_instances = md.instances;

        }

        bool print_instances = !tmp_instances.empty();


        // Now the instances
        if (print_instances) {
            int TAB_tmp = TAB;
            int itmax = tmp_instances.size();
            int itt = 0;
            // Loop thru instances
            for (std::map<std::string, std::vector<VerilogBlock>>::iterator it = tmp_instances.begin();
                 it != tmp_instances.end(); it++) {
                int ittt = 0;
                int itttmax = it->second.size();
                for (auto &itmp: it->second) {
                    hierarchy += string_format("%.*s\"%s\" : \n%.*s{\n", TAB, tab_str, itmp.name.c_str(),
                                               TAB, tab_str);
                    __recursive_seeker__(itmp, hierarchy, module_references, TAB_tmp + 1, tab_str);

                    // Append to hierarchy
                    hierarchy += string_format("%.*s}", TAB, tab_str);

                    if ((itt != (itmax - 1)) && (ittt != (itttmax - 1))) {
                        hierarchy += ",";
                    }
                    hierarchy += "\n";
                    ittt++;
                }
                itt++;
            }
        }


    } else {
        std::cout << "What kind of pokémon is this?" << std::endl;
    }

*/
        /*
        bool print_params = !(*tmp_params).empty();
        bool print_ports = !(*tmp_ports).input.empty() || !(*tmp_ports).output.empty() || !(*tmp_ports).inout.empty();
        bool print_netwires = !(*tmp_netwires).empty();


        // Place reference
        hierarchy += string_format("%.*s\"ref\" : \"%s\",\n", TAB, tab_str, ref.c_str());

        //tmp["ref"] = ref;

        // Parse parameters
        if (print_params){
            // Open parameters field
            hierarchy += string_format("%.*s\"parameters\" : \n%.*s{\n", TAB, tab_str, TAB, tab_str);
            int pmax = (*tmp_params).size();
            for (int p = 0; p < pmax; p++){
                hierarchy += string_format("%.*s\"%s\" : \"%s\"", TAB + 1, tab_str,
                                           (*tmp_params)[p].name.c_str(),
                                           (*tmp_params)[p].value.c_str());
                if (p != (pmax-1)){
                    hierarchy += ",";
                }
                hierarchy += "\n";
            }

            //for (auto& p: tmp_params){
           //     //tmp["parameters"][p.name] = p.value;
           //     hierarchy += string_format("%.*s\"%s\" : \"%s\",\n", TAB + 1, tab_str, p.name.c_str(),p.value.c_str());
            //}
            // Close parameters field
            hierarchy += string_format("%.*s}", TAB, tab_str);

            if (print_ports|print_netwires|print_instances){
                hierarchy += ",";
            }
            hierarchy += "\n";
        }

        // Ports
        // Open field
        if (print_ports){
            hierarchy += string_format("%.*s\"ports\" : \n%.*s{\n", TAB, tab_str, TAB, tab_str);
            // Inputs
            //Json::Value tmp_iports(Json::arrayValue);
            // Open inputs
            hierarchy += string_format("%.*s\"input\" : \n%.*s[\n", TAB + 1, tab_str, TAB + 1, tab_str);
            int pmax = (*tmp_ports).input.size();
            for (int p = 0; p < pmax; p++){
                hierarchy += string_format("%.*s\"%s\"", TAB + 2, tab_str, (*tmp_ports).input[p].c_str());
                if (p != (pmax-1)){
                    hierarchy += ",";
                }
                hierarchy += "\n";
            }

            //for (auto& p: tmp_ports.input){
            //    //tmp_iports.append(p);
            //    hierarchy += string_format("%.*s\"%s\",\n", TAB + 2, tab_str, p.c_str());
            //}

            //tmp["ports"]["input"] = tmp_iports;
            // Close input field
            hierarchy += string_format("%.*s],\n", TAB + 1, tab_str);

            // Open outputs
            hierarchy += string_format("%.*s\"output\" : \n%.*s[\n", TAB + 1, tab_str, TAB + 1, tab_str);
            pmax = (*tmp_ports).output.size();
            for (int p = 0; p < pmax; p++){
                hierarchy += string_format("%.*s\"%s\"", TAB + 2, tab_str, (*tmp_ports).output[p].c_str());
                if (p != (pmax-1)){
                    hierarchy += ",";
                }
                hierarchy += "\n";
            }
            // Close output field
            hierarchy += string_format("%.*s],\n", TAB + 1, tab_str);

            // Open inout
            hierarchy += string_format("%.*s\"inout\" : \n%.*s[\n", TAB + 1, tab_str, TAB + 1, tab_str);
            pmax = (*tmp_ports).inout.size();
            for (int p = 0; p < pmax; p++){
                hierarchy += string_format("%.*s\"%s\"", TAB + 2, tab_str, (*tmp_ports).inout[p].c_str());
                if (p != (pmax-1)){
                    hierarchy += ",";
                }
                hierarchy += "\n";
            }
            // Close inout field
            hierarchy += string_format("%.*s]\n", TAB + 1, tab_str);

            // // Outputs
            //Json::Value tmp_oports(Json::arrayValue);
            //for (auto& p: tmp_ports.output){
            //    tmp_oports.append(p);
            //}
            //tmp["ports"]["output"] = tmp_oports;


            // // Inouts
            //Json::Value tmp_ioports(Json::arrayValue);
            //for (auto& p: tmp_ports.inout){
            //    tmp_ioports.append(p);
            //}
            //tmp["ports"]["inout"] = tmp_ioports;

            hierarchy += string_format("%.*s}", TAB, tab_str);

            if (print_netwires|print_instances){
                hierarchy += ",";
            }
            hierarchy += "\n";
        }
        // Close ports field


        // Netwires
        if (print_netwires) {
            int nwmax = (*tmp_netwires).size();
            for (int nw = 0; nw < nwmax; nw++) {
                // Open netwire field
                hierarchy += string_format("%.*s\"%s\" : \n%.*s{\n", TAB, tab_str, (*tmp_netwires)[nw].name.c_str(),
                                           TAB, tab_str);

                // Construct values
                //tmp[nw.name]["type"] = nw.type;
                hierarchy += string_format("%.*s\"type\" : \"%s\",\n", TAB + 1, tab_str,
                                           (*tmp_netwires)[nw].type.c_str());

                //tmp[nw.name]["bitspan"] = nw.bitspan;
                hierarchy += string_format("%.*s\"bitspan\" : \"%s\",\n", TAB + 1, tab_str,
                                           (*tmp_netwires)[nw].bitspan.c_str());

                //tmp[nw.name]["arrayspan"] = nw.arrayspan;
                hierarchy += string_format("%.*s\"arrayspan\" : \"%s\",\n", TAB + 1, tab_str,
                                           (*tmp_netwires)[nw].arrayspan.c_str());

                //tmp[nw.name]["value"] = nw.value;
                hierarchy += string_format("%.*s\"value\" : \"%s\"\n", TAB + 1, tab_str,
                                           (*tmp_netwires)[nw].value.c_str());

                // Close netwire field
                hierarchy += string_format("%.*s}", TAB, tab_str);

                if ((nw != (nwmax - 1)) | print_instances) {
                    hierarchy += ",";
                }
                hierarchy += "\n";
            }
        }
        */


    //return tmp;



    /*
    for (int it = 0; it < itmax; it++) {

        // Create entry for this top_module

        hierarchy += string_format("%.*s\"%s\" : \n%.*s{\n", TAB, tab_str, tmp_instances[it].name.c_str(),
                                   TAB, tab_str);

        // Call recursive seeker on instance
        __recursive_seeker__(tmp_instances[it], hierarchy, module_references, TAB_tmp + 1, tab_str);
        //tmp[it.name] = __recursive_seeker__(it, hierarchy, module_definitions, TAB_tmp + 1);

        // Append to hierarchy
        hierarchy += string_format("%.*s}", TAB, tab_str);

        if (it != (itmax - 1)) {
            hierarchy += ",";
        }
        hierarchy += "\n";

    }
}
     */

//}