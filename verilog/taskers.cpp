//
// Created by Manuel Blanco Valentin on 1/19/21.
//

#include <string>
#include <fstream>
#include <random>
#include "design.h"

// Monitor task generator
int generate_monitor_tasks(std::string FILENAME, std::vector<std::vector<std::string>>& subset){

    // Init string
    std::string task_str;

    // Include usage
    task_str += "/* USAGE EXAMPLE. Add this snippet to your testbench file:\n";
    task_str += string_format("\t`include \"%s\"\n",FILENAME.c_str());
    task_str += "// Monitor registers\n";
    task_str += "reg[100*8:0] monitor_prefix;\n";
    task_str += "always @(posedge clk) begin\n";
    task_str += "\tmonitor_prefix = \"pos\";\n\n";
    task_str += "\tmonitor_instances(monitor_prefix);\n";
    task_str += "end\n";
    task_str += "// Initial setups\n";
    task_str += "initial begin\n";
    task_str += "\t// Open file where we will safe the seeg logs (if doesn't exist, create it)\n";
    task_str += "\tseeg_log_file = $fopen(\"seeg.log\", \"w+\");\n";
    task_str += "\t// Print header with all monitored registers \n";
    task_str += "\tprint_header; // this function is defined in this file\n\n";
    task_str += "// Rest of your custom initial functions ... \n\n";
    task_str += "end\n";
    task_str += "IMPORTANT NOTE: REMEMBER TO CLOSE THE FILE USING \"$fclose(seeg_log_file);\" before calling $finish;";
    task_str += "*/\n\n";

    // Create task to print the header
    task_str += "// Print header with all the registers \n";
    task_str += "task print_header;\n";
    task_str += "\tbegin\n";
    task_str += "\t\t$fdisplay(seeg_log_file,\"time,id,";
    for (auto& g: subset){
        for (auto& gg: g){
            task_str += gg + ",";
        }
    }
    task_str = task_str.substr(0,task_str.size()-1);
    task_str += "\");\n";
    task_str += "\tend\n";
    task_str += "endtask;\n\n\n";

    // Monitor task
    task_str += "// Monitor registers task\n";
    task_str += "task monitor_instances;\n";
    task_str += "\tinput[100*8:0] prefix;\n";
    task_str += "\tbegin\n";
    task_str += "\t\t$fdisplay(seeg_log_file, \"@%04d,%s,";
    for (auto& g: subset){
        for (auto& gg: g){
            task_str += "%X,";
        }
    }
    task_str = task_str.substr(0,task_str.size()-1);
    task_str += "\",$time,prefix,";
    for (auto& g: subset){
        for (auto& gg: g){
            task_str += gg + ",";
        }
    }
    task_str = task_str.substr(0,task_str.size()-1);
    task_str += ");\n";

    // Close task
    task_str += "\tend\n";
    task_str += "endtask\n\n";

    // Open stream
    std::ofstream file_id;
    if (!file_id.fail()){
        file_id.open(FILENAME);
        file_id << task_str;

        // Close file
        file_id.close();

        // Print to console
        std::cout << "[INFO] - Verilog Monitor-task file successfully stored in " + FILENAME << std::endl;
    }

    return file_id.fail();
}


void shuffle_instances(std::vector<std::vector<std::string>>& instances, int simultaneous_pulses,
                       bool exclusive, bool sort, bool shuffle){

    size_t num_instances = instances.size();
    std::vector<int> tmp;
    for (auto& k: instances) tmp.push_back(k.size());
    int num_options = *max_element(std::begin(tmp), std::end(tmp));
    std::random_device rd;
    std::mt19937 rsh(rd());

    // Now divide by simultaneous pulses
    std::vector<std::vector<std::string>> out;

    if (!exclusive){
        std::vector<std::string> tmp;
        for (auto& g: instances){
            for (auto& gg: g){
                tmp.push_back(gg);
            }
        }

        // Check if we have to sort
        if (sort){
            std::sort(tmp.begin(),tmp.end());
        } else {
            // Check if we have to shuffle
            if (shuffle) std::shuffle(tmp.begin(), tmp.end(), rsh);
        }


        int kg = tmp.size() / simultaneous_pulses;
        int t = 0;
        for (int i = 0; i < kg; i++){
            std::vector<std::string> tmptmp;
            for (int j = 0; j < simultaneous_pulses; j++){
                tmptmp.push_back(tmp[t]);
                t++;
            }
            out.push_back(tmptmp);
        }

    } else {

        // Copy instances
        std::vector<std::vector<std::string>> instances_left = instances;

        while (instances_left.size() >= simultaneous_pulses){
            // shuffle instances left
            std::shuffle(instances_left.begin(), instances_left.end(), rsh);

            // Choose randomly from left instances
            int ip = 0;
            std::vector<std::string> inst_tmp;

            while (inst_tmp.size() < simultaneous_pulses){
                std::shuffle(instances_left[ip].begin(), instances_left[ip].end(), rsh);
                inst_tmp.push_back(instances_left[ip].front());
                instances_left[ip].erase(instances_left[ip].begin());
                if (instances_left[ip].size() > 0){
                    ip++;
                } else{
                    instances_left.erase(instances_left.begin());
                }

            }

            out.push_back(inst_tmp);
        }
    }
    instances = out;
}

// Monitor task generator
int generate_see_tasks(std::string FILENAME, std::vector<std::vector<std::string>>& subset,
                       int simultaneous_pulses, double MAX_UPSET_TIME, double MIN_UPSET_TIME) {

    // Shuffle instances
    shuffle_instances(subset, simultaneous_pulses, true, true, true);

    // Get timing
    std::vector<double> t;
    for (auto& s: subset){
        double f = (double)rand() / RAND_MAX;
        t.push_back(MIN_UPSET_TIME + f * (MAX_UPSET_TIME - MIN_UPSET_TIME));
    }

    // Start building verilog file
    std::string task_str = "/* USAGE EXAMPLE. Add this snippet to your testbench file:\n";
    task_str += "\t`include \"" + FILENAME + "\"\n\n";
    task_str += "\t// SEE (signal-event-effect) ENABLING SIGNAL (WHEN THIS IS TRUE AND (vectornum_total_next > ((`SIM_TEST_SIZE * `SIM_MODEL_COUNT)))) simulation finishes\n";
    task_str += "\n";
    task_str += "\treg see_en;\n";
    task_str += "\treg see_done;\n";
    task_str += "\treg see_hold;\n";
    task_str += "\treg see_ready;\n";
    task_str += "\tinput[2*8:0] mode = 'set';\n";
    task_str += "\tinteger SEECounter;\n";
    task_str += "\tinteger seeg_log_file; // File-variable reference where we will save the logs\n";
    task_str += string_format("\tinteger max_num_regs = %d;\n\n",subset.size());
    task_str += "\t//Initial log-file setup\n";
    task_str += "\tinitial begin\n";
    task_str += "\t\t//Open file where we will store the logs\n";
    task_str += "\t\tseeg_log_file = $fopen(\"seeg.log\", \"w+\");\n";
    task_str += "\t\tprint_header;\n";
    task_str += "\tend\n\n\n";
    task_str += "\t// SEE process\n\n";
    task_str += "\talways @(posedge clk) begin\n";
    task_str += "\t\tif (see_en == 1'b0) begin\n";
    task_str += "\t\t\t// Reset counter \n";
    task_str += "\t\t\tSEECounter = 1'b0;\n";
    task_str += "\t\tend else begin\n\n";
    task_str += "\t\t\t// Only proceed if not on hold\n";
    task_str += "\t\t\tif (see_hold == 1'b0) begin\n";
    task_str += "\t\t\t\tif (SEECounter < max_num_regs) begin\n";
    task_str += "\t\t\t\t\t// Call event pulse task\n";
    task_str += "\t\t\t\t\tsingle_event_pulse(SEECounter, `CLK_PERIOD, mode);\n\n";
    task_str += "\t\t\t\t\t// Check the outputs now \n";
    task_str += "\t\t\t\t\t#`CLK_PERIOD;\n";
    task_str += "\t\t\t\t\tcheck_outputs();\n\n";
    task_str += "\t\t\t\t\t// Increase counter\n";
    task_str += "\t\t\t\t\tSEECounter = SEECounter + 1;\n\n";
    task_str += "\t\t\t\t\t// See is ready now \n";
    task_str += "\t\t\t\t\t$display(\"@%04d INFO: SEE applied on registers [%04d]\", $time, SEECounter - 1);\n";
    task_str += "\t\t\t\t\tsee_ready = 1'b1;\n";
    task_str += "\t\t\t\t\tsee_hold = 1'b1;\n";
    task_str += "\t\t\t\tend else begin\n\n";
    task_str += "\t\t\t\t\t// Process is done\n";
    task_str += "\t\t\t\t\tsee_done = 1'b1;\n";
    task_str += "\t\t\t\t\tif (SEECounter == max_num_regs) begin\n";
    task_str += "\t\t\t\t\t\t// All registers have been upset\n";
    task_str += "\t\t\t\t\t\t$display (\"@%04d INFO: See process done\", $time);\n";
    task_str += "\t\t\t\t\tend\n";
    task_str += "\t\t\t\tend\n";
    task_str += "\t\t\tend\n";
    task_str += "\t\tend\n";
    task_str += "\tend\n";
    task_str += "*/\n\n\n";

    // Add control variables
    task_str += "//Max number of events (registers) we will toggle\n";
    task_str += "`ifndef MAX_NUM_EVENTS\n";
    task_str += string_format("`define MAX_NUM_EVENTS %d\n",subset.size());
    task_str += "`endif\n\n";

    // Clock timing vars
    task_str += "// Internal clock\n";
    task_str += "real INTERNAL_CLK_PERIOD = 0.0;\n\n";

    // Verbose flag
    task_str += "//Verbose flag\n";
    task_str += "`ifndef VERBOSE\n";
    task_str += "`define VERBOSE 1'b1\n";
    task_str += "`endif\n\n";

    // See mode parameters
    task_str += "// SEE mode parameters\n";
    task_str += "parameter SEE_MODES = 2;\n";
    task_str += "parameter SEE_MODE_SET = 2'b00;\n";
    task_str += "parameter SEE_MODE_SEU = 2'b01;\n\n";

    // Print header of log file
    task_str += "// Print header of log file \n";
    task_str += "task print_header;\n\tbegin\n";
    task_str += "\t\t$fdisplay(seeg_log_file, \"time, eventid,";
    for (int i = 0; i < simultaneous_pulses; i++) task_str += string_format("reg[%d],",i);
    task_str += "xor_data,xor_sum\");\n";
    task_str += "\tend\nendtask\n";

    // Log registers
    task_str += "// Print upset logs into log file \n";
    task_str += "task log_reg;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\tbegin\n\n";
    task_str += "\t\t// Print time and eventid first\n";
    task_str += "\t\t$fwrite(seeg_log_file, \"@%04d,%d,\", $time, eventid);\n\n";
    task_str += "\t\tcase (eventid)\n";

    // Loop thru instances
    for (int i = 0; i < subset.size(); i++){
        task_str += string_format("\t\t\t%d: begin\n",i);
        for (auto& k: subset[i]){
            task_str += "\t\t\t\t\t$fwrite(seeg_log_file,\"" + k + "\");\n";
        }
        task_str += "\t\t\t\t end\n";
    }

    // Finally close task
    task_str += "\t\tendcase\n";
    task_str += "\tend\n";
    task_str += "endtask\n\n\n";



    // Hold toggle for a while task
    task_str += "// Single-Event Transient Pre-clock hold (so that signal gets triggered right before clock) \n";
    task_str += "task pre_hold;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\treal HoldDuration;\n";
    task_str += "\tbegin\n\n";
    task_str += "\t\tcase (eventid)\n";
    // Loop thru instances
    for (int i = 0; i < subset.size(); i++){
        task_str += string_format("\t\t\t%d: HoldDuration = INTERNAL_CLK_PERIOD/2 - %f;\n",i,t[i]/2);
    }
    // Finally close task
    task_str += "\t\tendcase\n";
    task_str += "\t\t//Wait for HoldDuration\n";
    task_str += "\t\t#(HoldDuration);\n";
    task_str += "\tend\n";
    task_str += "endtask\n\n\n";


    // Toggle net
    task_str += "// Single-Event Transient toggle registers\n";
    task_str += "task force_toggle_net;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\tbegin\n\n";
    task_str += "\t\tcase (eventid)\n";

    // Loop thru instances
    for (int i = 0; i < subset.size(); i++){
        task_str += string_format("\t\t\t%d: begin\n",i);
        for (auto& k: subset[i]){
            task_str += string_format("\t\t\t\t\tforce %s = ~%s;\n",k.c_str(),k.c_str());
        }
        task_str += "\t\t\t\t end\n";
    }

    // Finally close task
    task_str += "\t\tendcase\n";
    task_str += "\tend\n";
    task_str += "endtask\n\n\n";


    // Display net
    task_str += "//Display task info (optional)\n";
    task_str += "task display_pulse_info;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\tinput see_mode;\n";
    task_str += "\treg [SEE_MODES-1:0] see_mode;\n";
    task_str += "\treg [8*3:1] mode;\n";
    task_str += "\tbegin\n\n";

    task_str += "\t\tmode = \"SE\";\n";
    task_str += "\t\tif (see_mode == SEE_MODE_SEU) begin\n";
    task_str += "\t\t\tmode = {mode,\"U\"};\n";
    task_str += "\t\tend else begin\n";
    task_str += "\t\t\tmode = {mode,\"T\"};\n";
    task_str += "\t\tend\n";
    task_str += "\t\tif (`VERBOSE == 1'b1)\n";
    task_str += "\t\tbegin\n";
    task_str += "\t\tcase (eventid)\n";

    // Loop thru instances
    for (int i = 0; i < subset.size(); i++){
        task_str += string_format("\t\t\t%d: begin\n",i);
        for (auto& k: subset[i]){
            task_str += "\t\t\t\t\t\t$display (\"@%04d INFO: %s on " + string_format("%s\", $time, mode);\n",k.c_str());
        }
        task_str += "\t\t\t\t end\n";
    }

    // Finally close task
    task_str += "\t\t\tendcase\n";
    task_str += "\t\tend\n";
    task_str += "\tend\n";
    task_str += "endtask\n\n\n";


    task_str += "//Single-Event Transient Delay (toggle up time)\n";
    task_str += "task hold_toggle;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\tinput see_mode;\n";
    task_str += "\treg [SEE_MODES-1:0] see_mode;\n";
    task_str += "\treal SEEDuration;\n";
    task_str += "\tbegin\n\n";
    task_str += "\t\tif (see_mode == SEE_MODE_SET) begin\n";
    task_str += "\t\t\tcase (eventid)\n";
    // Loop thru instances
    for (int i = 0; i < subset.size(); i++){
        task_str += string_format("\t\t\t\t%d: SEEDuration = %f;\n",i,t[i]);
    }
    // Finally close task
    task_str += "\t\t\tendcase\n";
    task_str += "\t\tend else begin\n";
    task_str += "\t\t\tSEEDuration = INTERNAL_CLK_PERIOD;\n\n";
    task_str += "\t\tend\n";
    task_str += "\t\t//Wait for SEEDuration\n";
    task_str += "\t\t#(SEEDuration);\n";
    task_str += "\tend\n";
    task_str += "endtask\n\n\n";


    // Release net
    task_str += "// Single-Event Transient Release\n";
    task_str += "task release_net;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\tbegin\n\n";
    task_str += "\t\tcase (eventid)\n";

    // Loop thru instances
    for (int i = 0; i < subset.size(); i++){
        task_str += string_format("\t\t\t%d: begin\n",i);
        for (auto& k: subset[i]){
            task_str += string_format("\t\t\t\t\trelease %s;\n",k.c_str());
        }
        task_str += "\t\t\t\t end\n";
    }

    // Finally close task
    task_str += "\t\tendcase\n";
    task_str += "\tend\n";
    task_str += "endtask\n\n\n";


    // Finally, global pulse task
    task_str += "//Setup pulse on registers according to task number\n";
    task_str += "task single_event_pulse;\n";
    task_str += "\tinput eventid;\n";
    task_str += "\tinteger eventid;\n";
    task_str += "\tinput CLK_PERIOD;\n";
    task_str += "\treal CLK_PERIOD;\n";
    task_str += "\tinput [SEE_MODES-1:0] see_mode;\n";
    task_str += "\tbegin\n";
    task_str += "\t\t//Avoid overflowing eventid\n";
    task_str += "\t\tif (eventid >= `MAX_NUM_EVENTS)\n";
    task_str += "\t\t\teventid = (eventid%`MAX_NUM_EVENTS);\n\n";
    task_str += "\t\t//Set clock period value\n";
    task_str += "\t\tINTERNAL_CLK_PERIOD = CLK_PERIOD;\n";
    task_str += "\t\t//Prehold so signal starts right before clock edge\n";
    task_str += "\t\tpre_hold(eventid);\n";
    task_str += "\t\t//Force toggle\n";
    task_str += "\t\tforce_toggle_net(eventid);\n";
    task_str += "\t\t//Display info\n";
    task_str += "\t\tdisplay_pulse_info(eventid, see_mode);\n";
    task_str += "\t\t//Hold value\n";
    task_str += "\t\thold_toggle(eventid, see_mode);\n";
    task_str += "\t\t//Release toggle\n";
    task_str += "\t\trelease_net(eventid);\n";
    task_str += "\tend\n";
    task_str += "endtask\n";


    // Open stream
    std::ofstream file_id;
    if (!file_id.fail()){
        file_id.open(FILENAME);
        file_id << task_str;

        // Close file
        file_id.close();

        // Print to console
        std::cout << "[INFO] - Verilog SEE-task file successfully stored in " + FILENAME << std::endl;
    }

    return file_id.fail();



}