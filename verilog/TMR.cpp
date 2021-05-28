//
// Created by mbv6231 on 4/9/21.
//

#include <string>
#include <fstream>
#include <random>
#include "design.h"
#include <algorithm>
#include <filesystem>

void triplicate_modules(std::vector<std::string>& TMR,
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

    // Get the flags for TMR
    bool fullTMR = false;
    bool inTMR = false;
    bool clkTMR = false;
    bool logicTMR = false;
    bool outTMR = false;
    for (const auto& s: TMR){
        fullTMR |= (s == "full");
        inTMR |= (s == "in") | fullTMR;
        clkTMR |= (s == "clk") | fullTMR;
        logicTMR |= (s == "logic") | fullTMR | clkTMR;
        outTMR |= (s == "out") | fullTMR;
    }

    // vars
    std::string FILEPATH_OUT;

    // Parse type of TMR
    if (inTMR | clkTMR | logicTMR | outTMR){
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
            int nsfxs = sfx.size();

            // Get the ports for this reference
            if (module_references.contains(ref)){
                Ports ports = module_references[ref].ports;
                std::vector<NetWire> netwires = module_references[ref].netwires;

                // Substitutions we have to make to our submodule
                std::map<std::string,std::map<std::string,std::string>> subports;

                // Start building the string
                std::string DEF = string_format("module %s (\n\n", rep.c_str());
                std::string FANS = "";
                std::string VOTERS = "";
                std::string ori_INST = "";
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

                    std::string IP2 = IP + string_format(" %s", name.c_str());
                    if (!value.empty()) IP2 += string_format(" = %s", value.c_str());

                    // Only define if this is not a clk signal
                    if (!name.starts_with("clk") | !clkTMR){

                        // inTMR == 0 means module only gets 1 input signal
                        // inTMR == 1 means module gets 3 input signals
                        if (!inTMR){
                            // Don't triplicate input (but still name it with the first suffix, for consistency)
                            DEF += string_format("\tinput %s%s;\n",name.c_str(),sfx[0].c_str());

                            // We have to triplicate each input signal using a fanout (in case logic is triplicated)
                            if (logicTMR){
                                // Fanout
                                FANS += string_format("\tfanout #(.WIDTH(1)) %s_fanout(\n", name.c_str());
                                FANS += string_format("\t\t.in(%s%s),\n", name.c_str(), sfx[0].c_str());
                                for (int j = 0; j < nsfxs; j++) {
                                    FANS += "\t\t.out" + sfx[j] + "(net_" + name + sfx[j] + ")";
                                    if (j < (nsfxs-1)) FANS += ",";
                                    FANS += "\n";
                                }
                                FANS += "\t);\n\n";

                                // And triplicated internal signals for fanout
                                for (auto &sf: sfx) {
                                    std::string IP3 = IP + string_format(" net_%s", name.c_str());
                                    if (!value.empty()) IP3 += string_format(" = %s", value.c_str());

                                    TRIP_SIGNALS += "\t" + IP3 + sf + ";\n";

                                    if (!subports.contains(sf)) subports[sf] = {};
                                    subports[sf][name] = "net_" + name + sf;
                                }


                            } else {
                                std::string sf = sfx[0];
                                if (!subports.contains(sf)) subports[sf] = {};
                                subports[sf][name] = name + sf;
                            }


                        } else {

                            // Triplicated inputs registers
                            for (auto &sf: sfx) {
                                DEF += "\tinput";
                                if (!bitspan.empty()) DEF += string_format(" %s", bitspan.c_str());
                                if (!arrayspan.empty()) DEF += string_format("%s", arrayspan.c_str());
                                DEF += " " + name + sf + ";\n";
                            }

                            // If logic is not going to be triplicated, but we triplicated this input, we need a voter
                            if (!logicTMR){
                                // We have to create a majorityVoter for this input
                                VOTERS += string_format("\tmajorityVoter #(.WIDTH(1)) %s_Voter(\n", name.c_str());
                                for (int k = 0; k < nsfxs; k++) {
                                    VOTERS += string_format("\t\t.in%s(%s%s)", sfx[k].c_str(), name.c_str(), sfx[k].c_str());
                                    if (k < (nsfxs-1)) VOTERS += ",";
                                    VOTERS += "\n";
                                }
                                // Use first suffix for consistency
                                VOTERS += string_format("\t\t.out(net_%s%s)\n", name.c_str(), sfx[0].c_str());
                                VOTERS += "\t);\n\n";

                                // And triplicated internal signals for voter (for consistency)
                                TRIP_SIGNALS += "\t net_" + name + sfx[0] + ";\n";

                                std::string sf = sfx[0];
                                if (!subports.contains(sf)) subports[sf] = {};
                                subports[sf][name] = "net_" + name + sf;

                            } else {

                                for (auto &sf: sfx){
                                    if (!subports.contains(sf)) subports[sf] = {};
                                    subports[sf][name] = name + sf;
                                }

                            }

                        }

                    } else {

                        // Clock Triplication (if we are here it's because logicTMR is true. We forced this to be true)
                        for (auto &sf: sfx){

                            DEF += "\tinput";
                            if (!bitspan.empty()) DEF += string_format(" %s", bitspan.c_str());
                            if (!arrayspan.empty()) DEF += string_format("%s", arrayspan.c_str());
                            DEF += " " + name + sf + ";\n";

                            if (!subports.contains(sf)) subports[sf] = {};
                            subports[sf][name] = name + sf;
                        }

                    }

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

                    std::string OP2 = OP + string_format(" %s", name.c_str());
                    if (!value.empty()) OP2 += string_format(" = %s", value.c_str());

                    // outTMR == 0 means module spits only 1 output signal
                    // outTMR == 1 means module spits 3 output signals
                    if (!outTMR){

                        // Only one output for this module (suffix 0 for consistency)
                        DEF += "\toutput " + OP2 + sfx[0] + ";\n";

                        // If we triplicated the logic, we will need a majority voter
                        if (logicTMR){
                            VOTERS += string_format("\tmajorityVoter #(.WIDTH(1)) %s_Voter(\n", name.c_str());
                            for (int k = 0; k < nsfxs; k++) {
                                VOTERS += string_format("\t\t.in%s(%s%s)", sfx[k].c_str(), name.c_str(), sfx[k].c_str());
                                if (k < (nsfxs-1)) VOTERS += ",";
                                VOTERS += "\n";
                            }
                            VOTERS += string_format("\t\t.out(net_%s%s)\n", name.c_str(), sfx[0].c_str());
                            VOTERS += "\t);\n\n";

                            // And triplicated internal signals for voter (for consistency)
                            TRIP_SIGNALS += "\t" + OP + string_format(" net_%s%s", name.c_str(), sfx[0].c_str());
                            if (!value.empty()) TRIP_SIGNALS += string_format(" = %s", value.c_str());
                            TRIP_SIGNALS += ";\n";

                        }

                        std::string sf = sfx[0];
                        if (!subports.contains(sf)) subports[sf] = {};
                        subports[sf][name] = name + sf;

                    } else {

                        // Triplicate outputs
                        for (auto &sf: sfx) {
                            DEF += "\toutput " + OP + string_format(" %s%s", name.c_str(), sf.c_str());
                            if (!value.empty()) DEF += string_format(" = %s", value.c_str());
                            DEF += ";\n";
                        }

                        // Triplicate internal signals (if we triplicated the logic)
                        if (logicTMR){
                            // We need voter for each logic + inner signals
                            for (auto &sf: sfx){
                                VOTERS += string_format("\tmajorityVoter #(.WIDTH(1)) %s%s_Voter(\n", name.c_str(), sf.c_str());
                                for (int k = 0; k < nsfxs; k++) {
                                    VOTERS += string_format("\t\t.in%s(%s%s)", sfx[k].c_str(), name.c_str(), sfx[k].c_str());
                                    if (k < (nsfxs-1)) VOTERS += ",";
                                    VOTERS += "\n";
                                }
                                // Use first suffix for consistency
                                VOTERS += string_format("\t\t.out(net_%s%s)\n", name.c_str(), sf.c_str());
                                VOTERS += "\t);\n\n";
                            }

                            // Create inner signals now and replacemnts
                            for (auto &sf: sfx) {
                                TRIP_SIGNALS += "\t" + OP + string_format(" net_%s%s", name.c_str(), sf.c_str());
                                if (!value.empty()) TRIP_SIGNALS += string_format(" = %s", value.c_str());
                                TRIP_SIGNALS += ";\n";

                                if (!subports.contains(sf)) subports[sf] = {};
                                subports[sf][name] = "net_" + name + sf;
                            }

                        } else {
                            // We need a fanout
                            FANS += string_format("\tfanout #(.WIDTH(1)) %s_fanout(\n", name.c_str());
                            FANS += string_format("\t\t.in(net_%s%s),\n", name.c_str(), sfx[0].c_str());
                            for (int j = 0; j < nsfxs; j++) {
                                FANS += "\t\t.out" + sfx[j] + "(" + name + sfx[j] + ")";
                                if (j < (nsfxs-1)) FANS += ",";
                                FANS += "\n";
                            }
                            FANS += "\t);\n\n";

                            // Create inner signals now and replacemnts
                            std::string sf = sfx[0];
                            TRIP_SIGNALS += "\t" + OP + string_format(" net_%s%s", name.c_str(), sf.c_str());
                            if (!value.empty()) TRIP_SIGNALS += string_format(" = %s", value.c_str());
                            TRIP_SIGNALS += ";\n";

                            if (!subports.contains(sf)) subports[sf] = {};
                            subports[sf][name] = "net_" + name + sf;
                        }

                    }

                }


                DEF += "\n";
                // Append triplicated signals
                DEF += "\t//Triplicated signals\n";
                DEF += TRIP_SIGNALS;
                DEF += "\n";

                // Create the three instances of the reference module
                DEF += "\t// Module instances replication\n";
                for (auto& sfp: subports){
                    std::string sf = sfp.first;
                    DEF += string_format("\t%s inst_%s (\n", ref.c_str(), sf.c_str());

                    int nsfp = sfp.second.size();
                    int k = 0;
                    for (auto& ssp: sfp.second){
                        DEF += string_format("\t\t.%s(%s)",ssp.first.c_str(),ssp.second.c_str());
                        if (k < (nsfp-1)) DEF += ",";
                        DEF += "\n";
                    }
                    DEF += "\t);\n\n";
                }

                // Include fanouts
                if (!FANS.empty()) DEF += "\t// Fanouts\n";
                DEF += FANS;

                // Include voters
                if (!VOTERS.empty()) DEF += "\t// Voters\n";
                DEF += VOTERS;

                // Close module def
                DEF += ");\n";

                // Finally, write to file
                FILEPATH_OUT = OUTPATH + PATH_SEPARATOR + ref + TMR_SUFFIX + ".v";
                std::ofstream of;
                of.open (FILEPATH_OUT);
                of << DEF;
                of.close();

            } else {
                // Just copy the original file over
                std::string FILEPATH_IN = OUTPATH + PATH_SEPARATOR + ref + ".v";
                FILEPATH_OUT = OUTPATH + PATH_SEPARATOR + ref + TMR_SUFFIX + ".v";
                std::filesystem::copy(FILEPATH_IN, FILEPATH_OUT);

            }

            // Print to console
            std::string pref = "TMR of ";
            std::vector<std::string> p;
            for (auto& kp: std::map<std::string, bool>({{"inputs",inTMR},{"clock",clkTMR},{"logic",logicTMR},{"outputs",outTMR}})) if (kp.second) p.push_back(kp.first);
            int nps = p.size();
            for (int k = 0; k < nps; k++){
                pref += p[k];
                if (k < (nps-1)) pref += ", ";
            }
            if (fullTMR) pref = "Full-TMR";
            std::cout << "[INFO] - " + pref + " successfully applied to module " + ref + " and stored in " + FILEPATH_OUT << std::endl;

        }

    } else {
        std::cout << "[ERROR] - TMR class(es) unrecognized or not implemented yet: ";
        for (const auto& s: TMR) std::cout << s;
        std::cout << std::endl;
    }

}


