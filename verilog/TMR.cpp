//
// Created by mbv6231 on 4/9/21.
//

#include <string>
#include <fstream>
#include <random>
#include "design.h"
#include <algorithm>

void triplicate_modules(int TMR,
                        std::vector<std::vector<std::string>>& subsets,
                        std::map<std::string,VerilogBlock>& module_references,
                        std::map<std::string, std::string> __serial_instances__,
                        std::map<std::string, std::string> __serial_sources__,
                        std::string TMR_SUFFIX = "TMR",
                        std::string OUTPATH = ""){

    // Init out module def and instance
    std::string task_str;

    std::map<std::string,std::vector<std::string>> mod_files;
    std::map<std::string,std::string> replacements;

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
                        replacements.insert(std::pair<std::string,std::string>(ref,rep));
                    } else {
                        std::cout << "\t\tInstance " << ref << " was previously replaced with " << rep << std::endl;
                    }
                }
            }
        }
        // Now let's create the triplicated modules
        for (auto& el: replacements){
            std::string ref = el.first;
            std::string rep = el.second;
            std::vector<std::string> sfx = {"A","B","C"};

            // Get the ports for this reference
            if (module_references.contains(ref)){
                Ports ports = module_references[ref].ports;
                std::vector<NetWire> netwires = module_references[ref].netwires;

                // Start building the string
                std::string DEF = string_format("module %s (\n", rep.c_str());
                std::string FANS = "";
                std::string VOTERS = "";
                std::string gen_INST = "";
                std::string TRIP_SIGNALS = "";

                // Loop through inputs
                size_t ninputs = ports.input.size();
                size_t noutputs = ports.output.size();

                // Inputs
                DEF += "\t// Input signals\n";

                for (int i=0; i < ninputs; i++){
                    std::string name = ports.input[i];

                    // Try to find the netwire entry for this port
                    std::string type,bitspan,arrayspan,value = "";

                    for (auto& el: netwires){
                        if (el.name == name){
                            type = el.type;
                            if (el.bitspan != "[0:0]") bitspan = el.bitspan;
                            if (el.arrayspan != "[0:0]") arrayspan = el.arrayspan;
                            value = el.value;
                        }
                    }

                    std::string IP = "";
                    if (!type.empty()) IP += string_format("%s", type.c_str());
                    if (!bitspan.empty()) IP += string_format(" %s", bitspan.c_str());
                    if (!arrayspan.empty()) IP += string_format("%s", arrayspan.c_str());
                    IP += string_format(" %s", name.c_str());
                    if (!value.empty()) IP += string_format(" = %s", value.c_str());
                    DEF += "\tinput " + IP + ";\n";

                    // Triplicated inputs registers
                    for (auto& sf: sfx){
                        TRIP_SIGNALS += "\t" + IP + sf + ";\n";
                    }

                    // We have to triplicate each input signal using a fanout
                    FANS += string_format("\tfanout #(.WIDTH(1)) %s_fanout(\n",name.c_str());
                    FANS += string_format("\t\t.in(%s),\n",name.c_str());
                    FANS += string_format("\t\t.outA(%sA),\n",name.c_str());
                    FANS += string_format("\t\t.outB(%sB),\n",name.c_str());
                    FANS += string_format("\t\t.outC(%sC)\n",name.c_str());
                    FANS += "\t);\n\n";

                    // Append to gen_INST
                    gen_INST += string_format("\t\t.%s\t(%s$)", name.c_str(), name.c_str());
                    if ((i < (ninputs - 1)) | (noutputs > 0)) gen_INST += ",";
                    gen_INST += "\n";
                }

                // Outputs
                DEF += "\n\t// Output signals\n";

                for (int i=0; i < noutputs; i++){
                    std::string name = ports.output[i];

                    // Try to find the netwire entry for this port
                    std::string type,bitspan,arrayspan,value = "";

                    for (auto& el: netwires){
                        if (el.name == name){
                            type = el.type;
                            if (el.bitspan != "[0:0]") bitspan = el.bitspan;
                            if (el.arrayspan != "[0:0]") arrayspan = el.arrayspan;
                            value = el.value;
                        }
                    }

                    std::string OP = "";
                    if (!type.empty()) OP += string_format("%s", type.c_str());
                    if (!bitspan.empty()) OP += string_format(" %s", bitspan.c_str());
                    if (!arrayspan.empty()) OP += string_format("%s", arrayspan.c_str());
                    OP += string_format(" %s", name.c_str());
                    if (!value.empty()) OP += string_format(" = %s", value.c_str());
                    DEF += "\toutput " + OP + ";\n";

                    // Triplicated output registers
                    for (auto& sf: sfx){
                        TRIP_SIGNALS += "\t" + OP + sf + ";\n";
                    }

                    // We have to create a majorityVoter for each output
                    VOTERS += string_format("\tmajorityVoter #(.WIDTH(1)) %s_Voter(\n",name.c_str());
                    VOTERS += string_format("\t\t.inA(%sA),\n",name.c_str());
                    VOTERS += string_format("\t\t.inB(%sB),\n",name.c_str());
                    VOTERS += string_format("\t\t.inC(%sC),\n",name.c_str());
                    VOTERS += string_format("\t\t.out(%s)\n",name.c_str());
                    VOTERS += "\t);\n\n";

                    // Append to gen_INST
                    gen_INST += string_format("\t\t.%s\t(%s$)", name.c_str(), name.c_str());
                    if ((i < (noutputs - 1))) gen_INST += ",";
                    gen_INST += "\n";

                }

                DEF += "\n";

                // Append triplicated signals
                DEF += "\t//Triplicated signals\n";
                DEF += TRIP_SIGNALS;
                DEF += "\n";

                // Create the three instances of the reference module
                DEF += "\t// Module instances replication\n";
                for (auto& sf: sfx){
                    std::string INST = gen_INST;
                    size_t pos;
                    while ((pos = INST.find("$")) != std::string::npos) {
                        INST.replace(pos, 1, sf);
                    }
                    DEF += string_format("\t%s inst_%s (\n", ref.c_str(), sf.c_str()) + INST;
                    DEF += "\t);\n\n";
                }

                // Include fanouts
                DEF += "\t// Fanouts\n";
                DEF += FANS;

                // Include voters
                DEF += "\t// Voters\n";
                DEF += VOTERS;

                // Close module def
                DEF += ");\n";

                // Finally, write to file
                std::string FILEPATH = OUTPATH + PATH_SEPARATOR + ref + TMR_SUFFIX + ".v";
                std::ofstream of;
                of.open (FILEPATH);
                of << DEF;
                of.close();

                // Print to console
                std::cout << "[INFO] - FULL-TMR successfully applied to module " + ref + " and stored in " + FILEPATH << std::endl;

            }

        }

    } else {
        std::cout << "[ERROR] - TMR class unrecognized or not implemented yet: " << TMR << std::endl;
    }

}


