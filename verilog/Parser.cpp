//
// Created by Manuel Blanco Valentin on 1/6/21.
//

#include "Parser.h"
#include "definitions.h"
#include "blocks.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <filesystem>
#include <algorithm>
#include <string>
#include <regex>


// Parse file
VerilogBlock Parser::__parse_file__(std::vector <VerilogBlock>& module_definitions,
                                    std::map<std::string,VerilogBlock>& module_references,
                                    std::vector <std::string>& sources,
                                    std::vector <std::string>& lib,
                                    std::map<std::string,std::vector<std::string>>& orphan_instances,
                                    std::string FILENAME,
                                    std::string NAME){

    // Open FILE
    std::ifstream stream(FILENAME);

    // This var will tell us which source we have to use to pick the next line (stream)
    int nlines;
    int start_line = -1;

    // Init ancestors/children vector
    std::vector <std::string> ancestors = {};
    std::vector <std::string> children = {};

    // Try to open file
    if (FILENAME.empty()) {
        throw std::invalid_argument("Empty FILENAME and TEXT passed to parser. We cannot continue.");
    } else {

        // Check if file is null and exit
        if (stream.fail()) {
            printf("[ERROR] - %s NULL FILE", FILENAME.c_str());
            exit(EXIT_FAILURE);
        }

        // Count number of lines in file (so we can setup the progress bar properly later on)
        nlines = __count_number_lines__(FILENAME.c_str());

        // Print header
        std::cout << std::endl << "\n" + TAB << "[INFO] - Reading source file: '" << FILENAME << "' with a total of " << nlines
                  << " lines" << std::endl;

    }

    // Set in place
    Parser::num_lines = nlines;

    // Create a progress bar
    progressBar pbar(nlines);

    // Now we can call parse with the stream we just got
    VerilogBlock vlogtmp = Parser::__parse__(module_definitions, module_references, sources, lib, orphan_instances,
                                             stream, pbar,
                                             start_line, ancestors, children,
                                             "",
                                             TAB + "\t", NAME, "",
                                             FILENAME);

    // Close stream
    stream.close();

    return vlogtmp;
}

// Parse function
VerilogBlock Parser::__parse__(std::vector <VerilogBlock>& module_definitions,
                               std::map<std::string,VerilogBlock>& module_references,
                               std::vector <std::string>& sources,
                               std::vector <std::string>& lib,
                               std::map<std::string,std::vector<std::string>>& orphan_instances,
                               std::ifstream& stream,
                               progressBar& pbar,
                               int& start_line,
                               std::vector <std::string>& ancestors,
                               std::vector <std::string>& children,
                               std::string prev_block,
                               std::string TAB,
                               std::string NAME,
                               std::string REF,
                               std::string SRC) {


    int delta = (int) ((double) pbar.total * 0.01);
    if (delta < 1) delta = 1;

    // Init vars
    std::string line;
    std::string message;
    bool force_print = false;
    std::smatch match_pattern;

    // Init holders for verilog objects (netwires, ports, etc.)
    struct Ports ports = {{},
                          {},
                          {}};
    int port_type = -1;
    // We must initialize everything here
    VerilogBlock vblock;
    std::vector<NetWire> netwires;
    std::vector<Parameter> parameters;
    //std::map<std::string, std::vector<VerilogBlock*>> instances;
    //std::vector<VerilogBlock> instances;
    std::vector<std::string> children_tmp;
    std::vector<std::string> mod_defs_tmp_str;
    std::vector<VerilogBlock> mod_defs_tmp;
    std::map<std::string,VerilogBlock> module_references_tmp;
    std::map<std::string,std::vector<std::string>> orphans_tmp;

    //std::string subhierarchy_tmp;

    // Populate tmp ancestors
    std::vector<std::string> ancestors_tmp;
    for (auto& a: ancestors) ancestors_tmp.push_back(a);
    // Obviously add this reference to ancestors
    if (!REF.empty() && (strcmp(REF.c_str(),"module")!=0)) ancestors_tmp.push_back(REF);
    if (!NAME.empty() && (strcmp(NAME.c_str(),"module")!=0)) ancestors_tmp.push_back(NAME);

    // Start chrono
    auto start = std::chrono::high_resolution_clock::now();

    // Initialize vars to keep track of line_cursor
    int end_line = start_line;

    // Let's say that we detect the initial of a multiline comment.
    // We know that we will have to keep appending lines to the content,
    // until another block is opened (like an always, module, etc.)
    // Now, if we just append the lines, that means that every iteration
    // the number of lines to be parsed will increase linearly. E.g.,
    // the algorithm will have to process 1 line in the first iteration,
    // 2 lines in the next iteration, 3 in the following, etc.
    // If we don't do something about it, the processing time will increase
    // unnecessarily (because we already know that the previous 0..i-1 lines
    // are comments. So let's use an integer as an index of the starting point
    // of the current line at which we must start parsing. In this case, this
    // will make that at iteration "i" we only parse the new line (that hasn't been
    // appended yet), instead of the cumulative text. This is expected to
    // speed up computation time DRAMATICALLY, specially cause we are using regex.
    // This also means that we have to keep track of the group we are currently parsing
    // (like, comment group, always group, etc.)
    int st = 0;
    _VLOG_GROUPS_ CURRENT_GROUP(_VLOG_GROUPS_::INVALID);

    // Loop until we reach end of lines
    while (!stream.eof()) //!stream.eof()
    {

        if (prev_block.empty()){
            // If the previous block is empty we for sure need to process the new line
            getline(stream, line);

            // Increase iline
            end_line++;
            start_line = end_line;

            // Set state to invalid (cause we don't know what it is yet)
            CURRENT_GROUP = _VLOG_GROUPS_::INVALID;

        } else {
            // Line is prev_block if block is invalid, else read new line
            if (CURRENT_GROUP & _VLOG_GROUPS_::INVALID){
                line = prev_block;

                // Remove prev_block
                prev_block = "";
            } else if (CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE){
                // Read line but do not append
                getline(stream, line);

                // Increase iline
                end_line++;
                start_line = end_line;

            } else {
                getline(stream, line);

                // Increase iline
                end_line++;
                start_line = end_line;

                line = prev_block + "\n" + line;
            }


        }


        // Append previous line to this line
        //if (prev_block.empty()) {
        //    start_line = end_line;
        //}
        //line = prev_block + "\n" + line;

        // Make sure to erase prev_block (this could be a potential cause of error down the line)
        //prev_block = "";

        // Left-trim the line
        ltrim(line);
        force_print = false;

        // If empty string, go to next line
        if (line == "") continue;

        if (LineChecker(line, {"typedef"},1)) {
            // Remove typedef
            std::size_t pos = line.find("typedef ");
            if (pos != std::string::npos) {
                pos += 8;
                line = line.substr(pos);
            }

        }

        // Check different options
        if (stringStartsWith(line, "//")) {
            // This is a single-line comment
            start_line = end_line;

            message = string_format("Comment found at line %d (ignored)", end_line + line_offset);
            //prev_block = ""; // Just by not assigning this to prev_block, we are ignoring it
            line = "";
            force_print = false;

            // Group tracker (appender)
            CURRENT_GROUP = _VLOG_GROUPS_::INVALID;

        } else if (stringStartsWith(line, "`")) {
            if (stringStartsWith(line, "`include")) {
                start_line = end_line;

                // Let's get this source file out of the line so we can push it back to sources and parse it later
                line = line.substr(8); // 9 characters in "`include " (notice the space)
                trim(line);

                // Find "
                std::size_t pos = line.find('"');
                message = string_format("Include directive %s found at line %d NOT FOUND IN SYSTEM "
                                        "NOR LIBRARIES PROVIDED (ignoring)",
                                        line.c_str(), end_line + line_offset);
                force_print = false;

                if (pos != std::string::npos) {
                    line = line.substr(pos+1);
                    ltrim(line);

                    pos = line.find('"');

                    if (pos != std::string::npos) {
                        line = line.substr(0,pos);
                        trim(line);

                        // Now, before we actually push it into the sources, we gotta make sure
                        // that this file exists. Otherwise, we will just ignore it.
                        if (file_exists(line)){
                            // Also, let's make sure this file has not been already added
                            if (find(sources.begin(),sources.end(),line)==sources.end()){
                                // We have two options here. Either we put the file into the sources list
                                // and parse it independently OR we parse it now. That's what the AUTO_INCLUDE
                                // flag does. Let's check now
                                if (Parser::flags & FLAGS::AUTO_INCLUDE){
                                    // Create parser
                                    std::cout << std::endl;
                                    VerilogBlock vlogtmp = Parser::__parse_file__(mod_defs_tmp,
                                                                                  module_references_tmp,
                                                                                  sources, lib,
                                                                                  orphans_tmp,
                                                                                  line, line);

                                    // Push into instances
                                    vblock.push(vlogtmp.name, vlogtmp);

                                    // Add subhierarchy
                                    //subhierarchy_tmp += vlogtmp.subhierarchy;

                                    //instances.push_back(vlogtmp);
                                    //for (auto& mtp: vlogtmp.inner_modules) mod_defs_tmp.push_back(mtp);
                                    for (auto& mtp: vlogtmp.inner_moddefs)
                                        if (find(mod_defs_tmp_str.begin(), mod_defs_tmp_str.end(), mtp) == mod_defs_tmp_str.end())
                                            mod_defs_tmp_str.push_back(mtp);
                                    for (auto& mtp: vlogtmp.children)
                                        if (find(children_tmp.begin(), children_tmp.end(), mtp) == children_tmp.end())
                                            children_tmp.push_back(mtp);

                                    message = "";
                                    force_print = false;
                                    break;

                                } else {
                                    sources.push_back(line);
                                    message = string_format("Include directive %s found at line %d added to global sources",
                                                            line.c_str(), end_line + line_offset);
                                }

                            } else {
                                message = string_format("Include directive %s found at line %d but IGNORED (as source file was already parsed)",
                                                        line.c_str(), end_line + line_offset);
                            }
                            force_print = true;


                        } else {
                            // Let's check if this is an absolute path or just a file
                            std::size_t sep_pos = line.rfind(PATH_SEPARATOR);

                            if (sep_pos != std::string::npos){
                                // Get only the filename, without dir
                                line = line.substr(sep_pos+1);
                            }

                            // Let's now try to find this file in the lib
                            for (auto& dir: lib){
                                std::filesystem::path fs_dir (dir);
                                std::filesystem::path fs_file (line);
                                std::filesystem::path full_path = fs_dir / fs_file;

                                // Now check if exists
                                if (file_exists(full_path)){
                                    if (Parser::flags & FLAGS::AUTO_INCLUDE) {
                                        // Create parser
                                        std::cout << std::endl;
                                        VerilogBlock vlogtmp = Parser::__parse_file__(mod_defs_tmp, module_references_tmp,
                                                                                      sources, lib,
                                                                                      orphans_tmp,
                                                                                      full_path, full_path);

                                        vblock.push(vlogtmp.name, vlogtmp);
                                        // Add subhierarchy
                                        //subhierarchy_tmp += vlogtmp.subhierarchy;

                                        //for (auto& mtp: vlogtmp.inner_modules) mod_defs_tmp.push_back(mtp);
                                        for (auto& mtp: vlogtmp.inner_moddefs)
                                            if (find(mod_defs_tmp_str.begin(), mod_defs_tmp_str.end(), mtp) == mod_defs_tmp_str.end())
                                                mod_defs_tmp_str.push_back(mtp);
                                        for (auto& mtp: vlogtmp.children)
                                            if (find(children_tmp.begin(), children_tmp.end(), mtp) == children_tmp.end())
                                                children_tmp.push_back(mtp);

                                        message = "";
                                        force_print = false;
                                        break;
                                    } else {
                                        sources.push_back(full_path);
                                        message = string_format("Include directive %s found at line %d found at "
                                                                "library %s and added to global sources",
                                                                line.c_str(), end_line + line_offset, dir.c_str());
                                        force_print = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                prev_block = "";
                line = "";

                // Group tracker (appender)
                CURRENT_GROUP = _VLOG_GROUPS_::INVALID;



            } else {
                start_line = end_line;

                message = string_format("Directive found at line %d (ignored)", end_line + line_offset);
                prev_block = "";
                line = "";
                // Group tracker (appender)
                CURRENT_GROUP = _VLOG_GROUPS_::INVALID;

                force_print = false;
            }

            // Check if this is a block comment
        } else if (stringStartsWith(line, "/*") || (CURRENT_GROUP & _VLOG_GROUPS_::COMMENT)) {

            // Initialize Comment parser
            BlockCommentParser block(stream, line, end_line);

            // Parse (until "*\")
            prev_block = block.parse(pbar, TAB);
            start_line = block.end_line;
            end_line = start_line;

        } else {

            // Here there's a check we have to perform before parsing the specific type of string.
            // We gotta look how many non-nestable blocks we have in our line so far.
            // As their name suggests, non-nestable blocks cannot be nested, which means that in case
            // this line contains more than one non-nestable blocks, we'll have to divide this first.
            // This is a consequence of verilog/systemverilog not having a consistent way to declare blocks
            // a.k.a, you can declare a block in between begin/end statements, or without them, or whatever.
            // IEEE guys, fix this

            // initialize next_line tmp var
            std::string next_line = "";

            if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_PATTERNS_NON_NESTABLE_KEYWORDS__, __VLOG_NUM_PATTERNS_NON_NESTABLE_KEYWORDS__)) {

                // Count number (n) of non-nestable (nn) statements (blocks)
                int nnnblocks = string_count(line, __VLOG_REGEX_NON_NESTABLE_KEYWORDS__);

                if (nnnblocks > 1){
                    // Split. We will only parse the first statement (everything before the second non-nestable block)
                    // The rest of it will be parsed later, in the next iteration

                    int iip = 0;
                    for (std::sregex_iterator it = std::sregex_iterator(line.begin(), line.end(), __VLOG_REGEX_NON_NESTABLE_KEYWORDS__);
                         it != std::sregex_iterator(); it++) {
                        std::smatch match = *it;

                        if (iip == 1){
                            int cut = match.position(0);
                            //next_line = line.substr(cut);
                            //line = line.substr(0,cut);
                            line = line.substr(cut);
                            break;
                        }
                        iip++;

                        /*
                        cout << "\nMatched  string is = " << match.str(0)
                             << "\nand it is found at position "
                             << match.position(0) << endl;
                        cout << "Capture " << match.str(1)
                             << " at position " << match.position(1) << endl;
                        */
                    }
                }
            }


            // Now check for everything
            if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & stringStartsWith(line, "endmodule")) {
                // We found end of module definition, wrap things up and exit function
                prev_block = next_line;
                // Group tracker (appender)
                CURRENT_GROUP = _VLOG_GROUPS_::INVALID;
                break;


                // Module|Macromodule Definition
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_MODULE_NAMES__, __VLOG_NUM_MODULE_NAMES__) || CURRENT_GROUP & _VLOG_GROUPS_::MODULE) {

                // Make sure there's a ; in the definition
                std::size_t semicolon_pos = line.find(';');
                std::string post_semicolon = "";

                if (semicolon_pos != std::string::npos) {

                    // We found a module, so let's parse its content by recursively passing it to be
                    // __parse__ function

                    // first of all, let's get the name out of it

                    // Divide semicolon
                    post_semicolon = line.substr(semicolon_pos + 1);
                    line = line.substr(0, semicolon_pos);

                    // Trim
                    ltrim(post_semicolon);
                    rtrim(line);

                    // Get module name and params
                    // Get rid of keyword "module"
                    std::size_t pos = line.find("module ");
                    pos += 7;
                    line = line.substr(pos);
                    ltrim(line);

                    // Now we can parse the name of the module
                    pos = line.find("(");
                    if (pos != std::string::npos) {
                        // Try to find ;
                        line = line.substr(0, pos);
                    } else {
                        pos = line.find(";");
                        line = line.substr(0, pos);
                    }

                    // What's left is simply the name of the module
                    std::string module_name = line;
                    module_name = std::regex_replace(module_name, std::regex(R"(\\)"), R"(\\)");
                    trim(module_name);

                    // Print message before creating module
                    message = string_format("Module %s found from line %d to %d", module_name.c_str(),
                                            start_line + line_offset, end_line + line_offset);
                    pbar.update(end_line, message, TAB);

                    std::string TAB_tmp = TAB + "\t";

                    // Pass stream to parser to process module content
                    VerilogBlock vlogtmp = Parser::__parse__(mod_defs_tmp, module_references_tmp,
                                                             sources, lib,
                                                             orphans_tmp,
                                                             stream, pbar,
                                                             start_line,
                                                             ancestors_tmp,
                                                             children_tmp,
                                                             post_semicolon,
                                                             TAB_tmp,
                                                             module_name,
                                                             "module",
                                                             SRC);

                    // build subhierarchy before returning block
                    vlogtmp.__build_subhierarchy__(TAB_tmp);

                    // Push into mod_definitions
                    mod_defs_tmp.push_back(vlogtmp);
                    mod_defs_tmp_str.push_back(module_name);

                    //for (auto& mtp: vlogtmpmod.inner_modules) mod_defs_tmp.push_back(mtp);
                    for (auto& mtp: vlogtmp.inner_moddefs)
                        if (find(mod_defs_tmp_str.begin(), mod_defs_tmp_str.end(), mtp) == mod_defs_tmp_str.end())
                            mod_defs_tmp_str.push_back(mtp);
                    for (auto& mtp: vlogtmp.children)
                        if (find(children_tmp.begin(), children_tmp.end(), mtp) == children_tmp.end())
                            children_tmp.push_back(mtp);

                    // Module references maps
                    module_references_tmp.insert(std::pair<std::string,VerilogBlock>(module_name,vlogtmp));

                    if (find(children_tmp.begin(), children_tmp.end(), module_name) == children_tmp.end())
                        children_tmp.push_back(module_name);


                    //mod_defs_tmp.push_back(&vlogtmpmod);
                    //children_tmp.push_back(module_name);
                    // Add to mod_defs
                    //mod_defs_tmp_str.push_back(module_name);

                    // Push into children
                    /*
                    for (auto& c: children) {
                        if (std::find(children_tmp.begin(), children_tmp.end(), c) == children_tmp.end()){
                            children_tmp.push_back(c);
                        }
                    }
                     */

                    // Setup lines
                    end_line = start_line;

                    // Clean tmp vars
                    prev_block = next_line;
                    line = "";

                    // Setup message
                    message = "";
                    force_print = false;

                    // Group tracker (appender)
                    CURRENT_GROUP = _VLOG_GROUPS_::INVALID;

                } else {
                    // Keep appending lines until we find a semicolon
                    prev_block = line + next_line;
                    line = "";
                    message = string_format("Appending data from line %d to Module declaration found at line %d",
                                            end_line + line_offset, start_line + line_offset);

                    // Group tracker (appender)
                    CURRENT_GROUP = _VLOG_GROUPS_::MODULE;

                }

                // Begin/End statement
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_PATTERNS_STATEMENTS__, __VLOG_NUM_PATTERNS_STATEMENTS__) || CURRENT_GROUP & _VLOG_GROUPS_::BEGINEND) {

                // Initialize Begin/End parser
                BlockBeginEnd block(stream, line, end_line);

                // Parse (until end (this block is autorecursive, meaning that any nested begin/end blocks will
                // create new BlockBeginEnd objects))
                prev_block = block.parse(pbar, TAB);
                start_line = block.end_line;
                end_line = start_line;

                // Check if this is a netwire
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_PATTERNS_NETWIRES__, __VLOG_NUM_PATTERNS_NETWIRES__)) {

                /* Let's parse this bad boy now. The format for reg/wires/etc. in pseudo-regex is:
                (input|output|inout)? (reg|wire|tri|integer|real|...)? ([<bit_size>])? <NAME> ([<array_size>])? (= <VALUE>)?

                where "?" means optional arguments. So some examples would be:

                input reg Q;
                output wire D;
                reg [2:0] Q = 0;
                reg [2:0] Q [1:0];
                input Q;

                */


                // The first thing we must do is check if we can split this sentence into several statements
                // separated by ;
                std::size_t semicolon_pos = line.find(';');
                std::string post_line = "";
                if (semicolon_pos != std::string::npos) {
                    // Divide semicolon
                    post_line = line.substr(semicolon_pos + 1);
                    line = line.substr(0, semicolon_pos);

                    // First of all, check if port is specified (input/output/inout)
                    port_type = -1;
                    if (stringStartsWith(line, "input ")) {
                        // Append this port to input
                        // ports.inputs.push_back(name);
                        port_type = 0;
                        // Trim string to get rid of word "input " (with space)
                        std::size_t pos = line.find("input ");
                        pos += 6;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "output ")) {
                        // Append this port to input
                        // ports.outputs.push_back(name);
                        port_type = 1;

                        // Trim string to get rid of word "input " (with space)
                        std::size_t pos = line.find("output ");
                        pos += 7;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "inout ")) {
                        // Append this port to input
                        // ports.inouts.push_back(name);
                        port_type = 2;

                        // Trim string to get rid of word "input " (with space)
                        std::size_t pos = line.find("inout ");
                        pos += 6;
                        line = line.substr(pos);
                        ltrim(line);
                    }

                    // Check type of netwire
                    std::string type = "wire";
                    if (stringStartsWith(line, "integer")) {
                        type = "integer";
                        // Trim string to get rid of word "integer " (with space)
                        std::size_t pos = line.find("integer ");
                        pos += 8;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "real")) {
                        type = "real";
                        // Trim string to get rid of word "real " (with space)
                        std::size_t pos = line.find("real ");
                        pos += 5;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "realtime")) {
                        type = "realtime";
                        // Trim string to get rid of word "realtime " (with space)
                        std::size_t pos = line.find("realtime ");
                        pos += 9;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "reg")) {
                        type = "reg";
                        // Trim string to get rid of word "reg " (with space)
                        std::size_t pos = line.find("reg ");
                        pos += 4;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "time")) {
                        type = "time";
                        // Trim string to get rid of word "time " (with space)
                        std::size_t pos = line.find("time ");
                        pos += 5;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "wire")) {
                        type = "wire";
                        // Trim string to get rid of word "wire " (with space)
                        std::size_t pos = line.find("wire ");
                        pos += 5;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "supply0")) {
                        type = "supply0";
                        // Trim string to get rid of word "supply0 " (with space)
                        std::size_t pos = line.find("supply0 ");
                        pos += 7;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "supply1")) {
                        type = "supply1";
                        // Trim string to get rid of word "supply1 " (with space)
                        std::size_t pos = line.find("supply1 ");
                        pos += 7;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "tri")) {
                        type = "tri";
                        // Trim string to get rid of word "tri " (with space)
                        std::size_t pos = line.find("tri ");
                        pos += 4;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "triand")) {
                        type = "triand";
                        // Trim string to get rid of word "triand " (with space)
                        std::size_t pos = line.find("triand ");
                        pos += 7;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "trior")) {
                        type = "trior";
                        // Trim string to get rid of word "trior " (with space)
                        std::size_t pos = line.find("trior ");
                        pos += 6;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "tri0")) {
                        type = "tri0";
                        // Trim string to get rid of word "tri0 " (with space)
                        std::size_t pos = line.find("tri0 ");
                        pos += 5;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "tri1")) {
                        type = "tri1";
                        // Trim string to get rid of word "tri0 " (with space)
                        std::size_t pos = line.find("tri1 ");
                        pos += 5;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "wand")) {
                        type = "wand";
                        // Trim string to get rid of word "wand " (with space)
                        std::size_t pos = line.find("wand ");
                        pos += 5;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "wor")) {
                        type = "wor";
                        // Trim string to get rid of word "wor " (with space)
                        std::size_t pos = line.find("wor ");
                        pos += 4;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "int")) {
                        type = "int";
                        // Trim string to get rid of word "int " (with space)
                        std::size_t pos = line.find("int ");
                        pos += 4;
                        line = line.substr(pos);
                        ltrim(line);

                    } else if (stringStartsWith(line, "logic")) {
                        type = "logic";
                        // Trim string to get rid of word "logic " (with space)
                        std::size_t pos = line.find("logic ");
                        pos += 6;
                        line = line.substr(pos);
                        ltrim(line);

                    }
                    else if (stringStartsWith(line, "string")) {
                        type = "string";
                        // Trim string to get rid of word "logic " (with space)
                        std::size_t pos = line.find("string ");
                        pos += 7;
                        line = line.substr(pos);
                        ltrim(line);

                    }
                    else {
                        type = "wire";
                        // We don't have to trim anything here
                    }

                    // Now let's check if we have the bitspan
                    std::string bitspan = "[0:0]";
                    if (stringStartsWith(line, "[")) {
                        std::size_t pos = line.find("]");
                        pos += 1;
                        bitspan = line.substr(0, pos);

                        // Now trim bitspan
                        line = line.substr(pos);
                        ltrim(line);

                    }

                    // Before we get the names, we will try to get the value
                    std::size_t pos = line.find('=');
                    std::string value;
                    if (pos != std::string::npos) {
                        // There is a value definition, let's parse it now
                        value = line.substr(pos+1);
                        // Trim value from line definition
                        line = line.substr(0, pos);
                        rtrim(line);
                        ltrim(line);
                        std::replace( line.begin(), line.end(), '\"', '\'');
                        rtrim(value);
                        ltrim(value);
                        std::replace(value.begin(), value.end(), '\"', '\'');

                    }

                    // Now we can try to get the array span, in case it exists
                    std::string arrayspan = "[0:0]";
                    std::size_t pos_bracket = line.find('[');
                    if (pos_bracket != std::string::npos) {
                        // Get arrayspan
                        arrayspan = line.substr(pos_bracket);
                        std::replace(arrayspan.begin(), arrayspan.end(),'\n',' ');
                        arrayspan = std::regex_replace(arrayspan, std::regex(R"(\\)"), R"(\\)");

                        // Now trim arrayspan from line
                        line = line.substr(0, pos_bracket);
                        rtrim(line);
                    }

                    // Now we have everything that might define this group of netwires.
                    // The only problem now is that each definition of netwire might be
                    // declared in the same statement, separated by commas (something like
                    // reg clk, a,  b,c,d; (notice the different number of spaces, etc)
                    // So we must parse this taking that into consideration and initialize
                    // a NetWire object (struct) for each instance.
                    // Make sure we first trim line
                    ltrim(line);

                    // Initialize array of NetWires
                    std::string names_msg;
                    bool first_time = true;
                    // Store beginning and end of sentences
                    std::string::const_iterator start = line.begin();
                    std::string::const_iterator end = line.end();
                    std::string::const_iterator next = std::find(start, end, ',');
                    std::string name;
                    // Loop thru all splits until we find end of line
                    while (next != end) {
                        // Get name
                        name = std::string(start, next);
                        name = std::regex_replace(name, std::regex(R"(\\)"), R"(\\)");

                        // Make sure to trim name
                        trim(name);

                        // Add to names_msg so we display this in our message later
                        if (first_time)
                            names_msg = name;
                        else
                            names_msg += ", " + name;

                        first_time = false;

                        // Now Init Netwire object
                        NetWire nw(name, type, bitspan, arrayspan, value,SRC);
                        netwires.push_back(nw);

                        // Check if we have to append this to ports
                        switch (port_type) {
                            case 0:
                                ports.input.push_back(name);
                                break;
                            case 1:
                                ports.output.push_back(name);
                                break;
                            case 2:
                                ports.inout.push_back(name);
                                break;
                        }

                        // Increase start
                        start = next + 1;

                        // Find next
                        next = std::find(start, end, ',');
                    }

                    // Last element
                    // Get name
                    name = std::string(start, next);
                    name = std::regex_replace(name, std::regex(R"(\\)"), R"(\\)");

                    // Make sure to trim name
                    trim(name);

                    // Add to names_msg so we display this in our message later
                    if (first_time)
                        names_msg = name;
                    else
                        names_msg += ", " + name;

                    // Now Init Netwire object
                    NetWire nw(name, type, bitspan, arrayspan, value,SRC);
                    netwires.push_back(nw);

                    // Check if we have to append this to ports
                    switch (port_type) {
                        case 0:
                            ports.input.push_back(name);
                            break;
                        case 1:
                            ports.output.push_back(name);
                            break;
                        case 2:
                            ports.inout.push_back(name);
                            break;
                    }

                    // Setup message
                    message = string_format("%s(s) (%s) %s %s found at line %d", type.c_str(),
                                            names_msg.c_str(), bitspan.c_str(), arrayspan.c_str(), end_line + line_offset);
                    force_print = true;

                    // Assign post_line to line
                    line = post_line + next_line;
                    prev_block = "";

                } else {
                    // We need to keep appending lines until we find a semicolon
                    prev_block = line + next_line;
                    line = "";
                    message = string_format("Appending data to netwires statement found at line %d",
                                            end_line + line_offset);
                }

                // Check if this is a parameter definition
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_PATTERNS_PARAMS__, __VLOG_NUM_PATTERNS_PARAMS__)) {

                // The first thing we must do is check if we can split this sentence into several statements
                // separated by ;
                std::size_t semicolon_pos = line.find(';');
                std::string post_line;
                if (semicolon_pos != std::string::npos) {
                    // Separate post_line first
                    post_line = line.substr(semicolon_pos + 1);
                    line = line.substr(0, semicolon_pos);

                    // Trim keyword parameter/localparameter
                    std::size_t pos = line.find("parameter ");
                    if (pos != std::string::npos) {
                        pos += 10;
                        line = line.substr(pos);
                    } else {
                        pos = line.find("localparam ");
                        if (pos != std::string::npos) {
                            pos += 11;
                            line = line.substr(pos);
                        }
                    }

                    // Trim line
                    ltrim(line);

                    // Now parse value of parameters
                    pos = line.find('=');
                    std::string value;
                    if (pos != std::string::npos) {
                        // There is a value definition, let's parse it now
                        value = line.substr(pos);
                        // Trim value from line definition
                        line = line.substr(0, pos);
                        rtrim(line);
                    }

                    // Now parse all the names
                    // Make sure we first trim line
                    ltrim(line);

                    // Initialize array of parameters
                    std::string names_msg;
                    bool first_time = true;
                    // Store beginning and end of sentences
                    std::string::const_iterator start = line.begin();
                    std::string::const_iterator end = line.end();
                    std::string::const_iterator next = std::find(start, end, ',');
                    std::string name;
                    // Loop thru all splits until we find end of line
                    while (next != end) {
                        // Get name
                        name = std::string(start, next);
                        name = std::regex_replace(name, std::regex(R"(\\)"), R"(\\)");

                        // Make sure to trim name
                        trim(name);

                        // Add to names_msg so we display this in our message later
                        if (first_time)
                            names_msg = name;
                        else
                            names_msg += ", " + name;

                        first_time = false;

                        // Now Init Netwire object
                        Parameter par = {name, value};
                        parameters.push_back(par);

                        // Increase start
                        start = next + 1;

                        // Find next
                        next = std::find(start, end, ',');
                    }

                    // Find last one
                    // Get name
                    name = std::string(start, next);
                    name = std::regex_replace(name, std::regex(R"(\\)"), R"(\\)");

                    // Make sure to trim name
                    trim(name);

                    // Add to names_msg so we display this in our message later
                    if (first_time)
                        names_msg = name;
                    else
                        names_msg += ", " + name;

                    first_time = false;

                    // Now Init Netwire object
                    Parameter par = {name, value};
                    parameters.push_back(par);

                    // Setup message
                    message = string_format("Parameter(s) (%s) found at line %d",
                                            names_msg.c_str(), end_line + line_offset);
                    force_print = true;

                    // Assign post_line to line
                    line = post_line + next_line;
                    prev_block = "";

                } else {
                    // We need to keep appending lines until we find a semicolon
                    prev_block = line + next_line;
                    line = "";
                    message = string_format("Appending data to parameter statement found at line %d",
                                            end_line + line_offset);
                }

                // Check if this is an assign statement (which we will ignore)
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_SEMICOLON_STATEMENTS__, __VLOG_NUM_SEMICOLON_STATEMENTS__)) {

                // The first thing we must do is check if we can split this sentence into several statements
                // separated by ;
                std::size_t semicolon_pos = line.find(';');
                std::string post_line;
                if (semicolon_pos != std::string::npos) {
                    // Separate post_line first
                    prev_block = line.substr(semicolon_pos + 1) + next_line;
                    line = "";

                    // Setup message
                    message = string_format("General statement found at line %d ignored", end_line + line_offset);
                    force_print = false;

                } else {
                    // We need to keep appending lines until we find a semicolon
                    prev_block = line + next_line;
                    line = "";
                    message = string_format("Appending data to Statement found at line %d", end_line + line_offset);
                }


                // Check if this is a typedef
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & stringStartsWith(line, "struct")) {

                // The first thing we must do is check if we can split this sentence into several statements
                // separated by ;
                std::size_t semicolon_pos = line.find('}');
                std::string post_line;
                if (semicolon_pos != std::string::npos) {
                    // Separate post_line first
                    prev_block = line.substr(semicolon_pos + 1);

                    std::smatch match;

                    if (std::regex_search(line, match, __VLOG_STRUCT_PATTERN__))
                    {
                        std::string name = match[4];
                        name = std::regex_replace(name, std::regex(R"(\\)"), R"(\\)");
                    }

                    post_line = line.substr(semicolon_pos + 1);
                    std::size_t semicolon_pos = post_line.find(';');
                    prev_block = post_line.substr(semicolon_pos + 1) + next_line;

                    // Setup message
                    message = string_format("Struct def found at line %d", end_line + line_offset);
                    force_print = false;

                } else {
                    // We need to keep appending lines until we find a } character
                    prev_block = line + next_line;
                    line = "";
                    message = string_format("Appending data to struct def found at line %d", end_line + line_offset);
                }


                // Check if this is a function definition
            } else if (!(CURRENT_GROUP & _VLOG_GROUPS_::INSTANCE) & LineChecker(line, __VLOG_GROUPS__, __VLOG_NUM_GROUPS__)) {

                // The first thing we must do is calculate the number of opening and ending statements
                // (that is, begin/ends). We have to keep appending lines until nbegins == nends.
                // If for some reason we get at any point nends > nbegins, cancel this, as something went wrong.

                // Initialize variable used to check if all statements have been closed
                bool statements_open = false;
                bool statements_error = false;

                int nbegins_function = string_count(line, __VLOG_FUNCTION_REGEX__);
                int nends_function = string_count(line, __VLOG_ENDFUNCTION_REGEX__);
                int nbegins_tot = nbegins_function;
                int nends_tot = nends_function;
                statements_open |= (nbegins_function > nends_function);
                statements_error |= (nbegins_function < nends_function);

                // Task begin/ends
                int nbegins_task = string_count(line, __VLOG_TASK_REGEX__);
                int nends_task = string_count(line, __VLOG_ENDTASK_REGEX__);
                nbegins_tot += nbegins_task;
                nends_tot += nends_task;
                statements_open |= (nbegins_task > nends_task);
                statements_error |= (nbegins_task < nends_task);

                // Fork begins/ends
                int nbegins_fork = string_count(line, __VLOG_FORK_REGEX__);
                int nends_fork = string_count(line, __VLOG_JOIN_REGEX__);
                nbegins_tot += nbegins_fork;
                nends_tot += nends_fork;
                statements_open |= (nbegins_fork > nends_fork);
                statements_error |= (nbegins_fork < nends_fork);

                // Case(X|Z) begins/ends
                int nbegins_case = string_count(line, __VLOG_CASE_REGEX__) +
                                   string_count(line, __VLOG_CASEX_REGEX__) +
                                   string_count(line, __VLOG_CASEZ_REGEX__);
                int nends_case = string_count(line, __VLOG_ENDCASE_REGEX__);
                nbegins_tot += nbegins_case;
                nends_tot += nends_case;
                statements_open |= (nbegins_case > nends_case);
                statements_error |= (nbegins_case < nends_case);

                // Generate begins/ends
                int nbegins_generate = string_count(line, __VLOG_GENERATE_REGEX__);
                int nends_generate = string_count(line, __VLOG_ENDGENERATE_REGEX__);
                nbegins_tot += nbegins_generate;
                nends_tot += nends_generate;
                statements_open |= (nbegins_generate > nends_generate);
                statements_error |= (nbegins_generate < nends_generate);

                // Config begins/ends
                int nbegins_config = string_count(line, __VLOG_CONFIG_REGEX__);
                int nends_config = string_count(line, __VLOG_ENDCONFIG_REGEX__);
                nbegins_tot += nbegins_config;
                nends_tot += nends_config;
                statements_open |= (nbegins_config > nends_config);
                statements_error |= (nbegins_config < nends_config);

                // Primitive begins/ends
                int nbegins_primitives = string_count(line, __VLOG_PRIMITIVE_REGEX__);
                int nends_primitives = string_count(line, __VLOG_ENDPRIMITIVE_REGEX__);
                nbegins_tot += nbegins_primitives;
                nends_tot += nends_primitives;
                statements_open |= (nbegins_primitives > nends_primitives);
                statements_error |= (nbegins_primitives < nends_primitives);

                // Specify begins/ends
                int nbegins_specify = string_count(line, __VLOG_SPECIFY_REGEX__);
                int nends_specify = string_count(line, __VLOG_ENDSPECIFY_REGEX__);
                nbegins_tot += nbegins_specify;
                nends_tot += nends_specify;
                statements_open |= (nbegins_specify > nends_specify);
                statements_error |= (nbegins_specify < nends_specify);

                // Table begins/ends
                int nbegins_table = string_count(line, __VLOG_TABLE_REGEX__);
                int nends_table = string_count(line, __VLOG_ENDTABLE_REGEX__);
                nbegins_tot += nbegins_table;
                nends_tot += nends_table;
                statements_open |= (nbegins_table > nends_table);
                statements_error |= (nbegins_table < nends_table);

                statements_open |=  (nbegins_tot == 0 && nends_tot == 0);

                // Check if there's any open nested statement
                if (statements_open) {
                    // This means that at least one statement is still open, so let's keep appending lines
                    prev_block = line + next_line;
                    line = "";
                    message = string_format(
                            "Appending data from line %d to Statement found previously at line %d",
                            end_line + line_offset, start_line + line_offset);

                } else if (statements_error) {
                    // Something went wrong, cause nends should NEVER be > nbegins, so let's just raise an exception here
                    throw std::invalid_argument("Number of ENDS keywords in block surpasses the number of BEGINS "
                                                "found, which is impossible. Raising error.");
                } else {
                    // Then all statements have been properly closed, so let's wrap this up
                    prev_block = next_line;
                    line = "";

                    // Setup message
                    message = string_format("Statement found from line %d to %d (ignored)",
                                            start_line + line_offset, end_line + line_offset);
                    force_print = true;

                    // Setup lines
                    start_line = end_line;
                }

                // Check if this module instance (either primitive or not, it doesn't matter)
            } else if (line.find(";") != std::string::npos){

                // Build whole instance def
                line = prev_block + line;

                if (std::regex_search(line, match_pattern, __VLOG_INSTANCE_PATTERN__)) {

                    std::string module_ref_name = match_pattern[1];
                    std::string instance_parameters = match_pattern[2];
                    std::string instance_name = match_pattern[3];
                    std::string instance_ports = match_pattern[4];

                    module_ref_name = std::regex_replace(module_ref_name, std::regex(R"(\\)"), R"(\\)");
                    instance_name = std::regex_replace(instance_name, std::regex(R"(\\)"), R"(\\)");

                    // Module name cleaning
                    // Trim the shit outta everything
                    trim(module_ref_name);

                    // Instance parameters (if any)
                    trim(instance_parameters);
                    // Now see if we have any parameters in the instantiation
                    std::vector<Parameter> inst_params;
                    std::size_t hash_pos = instance_parameters.find("#");
                    if (hash_pos != std::string::npos) {
                        // Discard "#" and look for opening parentheses
                        instance_parameters = instance_parameters.substr(hash_pos + 1);
                        ltrim(instance_parameters);

                        // Get parameters inside parentheses
                        std::size_t par_pos = instance_parameters.find("(");
                        if (par_pos != std::string::npos) {
                            // Discard opening parentheses
                            instance_parameters = instance_parameters.substr(par_pos + 1);
                            ltrim(instance_parameters);

                            // Look for closing parentheses
                            par_pos = instance_parameters.find(")");

                            if (par_pos != std::string::npos) {
                                std::string inst_params_str = instance_parameters.substr(0, par_pos - 1);
                                instance_parameters = instance_parameters.substr(par_pos + 1);
                                // Trim
                                ltrim(instance_parameters);
                                trim(inst_params_str);

                                // At this point we can process the parameters

                                // Parse parameters, which are separated by commas (and spaces along with the commas)
                                // Initialize array of NetWires
                                std::string inst_params_msg = "";
                                bool first_time = true;
                                // Store beginning and end of sentences
                                std::string::const_iterator start = inst_params_str.begin();
                                std::string::const_iterator end = inst_params_str.end();
                                std::string::const_iterator next = std::find(start, end, ',');
                                std::string param_def, param_name, param_value = "";

                                // Loop thru all splits until we find end of line
                                while (next != end) {
                                    // Get name and value
                                    param_def = std::string(start, next);
                                    // Make sure to trim def
                                    trim(param_def);
                                    // Split between name and value
                                    std::size_t pos_eq = param_def.find("=");
                                    if (pos_eq != std::string::npos) {
                                        // Get name
                                        param_name = param_def.substr(0, pos_eq - 1);
                                        rtrim(param_name);

                                        // Get value
                                        param_value = param_def.substr(pos_eq + 1);
                                        ltrim(param_value);

                                        // Initialize parameter
                                        Parameter ptmp{param_name, param_value};

                                        // Put in place
                                        inst_params.push_back(ptmp);


                                        // Add to names_msg so we display this in our message later
                                        if (first_time)
                                            inst_params_msg = param_name + " = " + param_value;
                                        else
                                            inst_params_msg += ", " + param_name + " = " + param_value;

                                        first_time = false;

                                    }

                                    // Increase start
                                    start = next + 1;

                                    // Find next
                                    next = std::find(start, end, ',');
                                }

                                // Remember to repeat process for last instance
                                // Get name and value
                                param_def = std::string(start, next);
                                // Make sure to trim def
                                trim(param_def);
                                // Split between name and value
                                std::size_t pos_eq = param_def.find("=");
                                if (pos_eq != std::string::npos) {
                                    // Get name
                                    param_name = param_def.substr(0, pos_eq - 1);
                                    rtrim(param_name);

                                    // Get value
                                    param_value = param_def.substr(pos_eq + 1);
                                    ltrim(param_value);

                                    // Initialize parameter
                                    Parameter ptmp{param_name, param_value};

                                    // Put in place
                                    inst_params.push_back(ptmp);


                                    // Add to names_msg so we display this in our message later
                                    if (first_time)
                                        inst_params_msg = param_name + " = " + param_value;
                                    else
                                        inst_params_msg += ", " + param_name + " = " + param_value;

                                    first_time = false;

                                }

                                // Increase start
                                start = next + 1;

                                // Find next
                                next = std::find(start, end, ',');

                            }

                        }
                    }

                    // Instance name
                    trim(instance_name);

                    // Instance ports (if any)
                    trim(instance_ports);

                    // Get the ports first
                    std::size_t par_pos = instance_ports.find("(");
                    std::string inst_ports = "";
                    if (par_pos != std::string::npos) {
                        inst_ports = instance_ports.substr(par_pos + 1);
                        instance_ports = instance_ports.substr(0, par_pos);
                        rtrim(instance_ports);
                        ltrim(inst_ports);

                        par_pos = instance_ports.find(")");
                        if (par_pos != std::string::npos) {
                            inst_ports = instance_ports.substr(par_pos - 1);
                            rtrim(inst_ports);
                        } else {
                            inst_ports = "";
                        }
                    }

                    // Create instance
                    VerilogBlock instance_tmp;

                    // check if module exists
                    if (module_references.contains(module_ref_name)) {
                        instance_tmp = module_references.at(module_ref_name);
                        instance_tmp.parameters = inst_params;
                        instance_tmp.ancestors = ancestors_tmp;
                        instance_tmp.ref = module_ref_name;
                        instance_tmp.name = instance_name;
                        instance_tmp.sourcefile_ = SRC;
                        // Push into vector
                        vblock.push(module_ref_name, instance_tmp);

                        // Add to subhierarchy
                        //subhierarchy_tmp += instance_tmp.subhierarchy;
                    } else {
                        std::string subhierarchy_tmp = string_format("%s\"ref\": \"%s\"\n",
                                                                     TAB.c_str(), module_ref_name.c_str());
                        instance_tmp.ref = module_ref_name;
                        instance_tmp.name = instance_name;
                        instance_tmp.parameters = inst_params;
                        instance_tmp.ancestors = ancestors_tmp;
                        instance_tmp.subhierarchy = subhierarchy_tmp;
                        instance_tmp.sourcefile_ = SRC;

                        // Push into vector
                        vblock.push(module_ref_name, instance_tmp);

                        if (!orphans_tmp.contains(module_ref_name)) {
                            orphans_tmp.insert(
                                    std::pair<std::string, std::vector<std::string>>(module_ref_name, {instance_name}));
                        } else {
                            orphans_tmp.at(module_ref_name).push_back({instance_name});
                        }

                    }


                    // Push into children vector
                    if (std::find(children_tmp.begin(), children_tmp.end(), module_ref_name) ==
                        children_tmp.end())
                        children_tmp.push_back(module_ref_name);

                    // See if we can find this instance in the module definition vector
                    /*
                    std::__wrap_iter<VerilogBlock *> index_ref = find_module_def(module_definitions.begin(), module_definitions.end(), module_ref_name);
                    if (index_ref != module_definitions.end())
                    {
                        // Element in vector.
                        index_ref;

                    }
                     */


                    // Print message before creating module
                    message = string_format("Instance '%s' of module '%s' found from line %d to %d",
                                            instance_name.c_str(),
                                            module_ref_name.c_str(),
                                            start_line + line_offset,
                                            end_line + line_offset);
                    force_print = false;

                    prev_block = next_line;
                    line = "";
                    CURRENT_GROUP = _VLOG_GROUPS_::INVALID;
                } else {
                    // Discard this whole definition
                    prev_block = next_line;
                    line = "";
                    CURRENT_GROUP = _VLOG_GROUPS_::INVALID;
                }

            } else {

                // We don't know what this is, but it's not parseable
                //message = string_format("Undefined content at line %d ignored\n", end_line + line_offset);
                //prev_block = "";
                //line = "";

                // We need to keep appending lines until we find a semicolon
                prev_block = prev_block + line + next_line;
                message = string_format("Appending data to Instance definition (size: %d) found at line %d",
                                        line.size(), end_line + line_offset);
                line = "";
                force_print = true;
                CURRENT_GROUP = _VLOG_GROUPS_::INSTANCE;
            }

        }


        // Update progress bar
        if (((end_line % delta == 0) || force_print) && (message != "")) {
            pbar.update(end_line, message, TAB);
            message = "";
        }
    }

    // Get final time
    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate time taken
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    // Populate VerilogBlock
    if (REF.empty()) REF = NAME;
    vblock.ref = REF;
    vblock.name = NAME;
    //vblock.instances = instances;
    vblock.netwires = netwires;
    vblock.ports = ports;
    vblock.parameters = parameters;
    vblock.ancestors = ancestors_tmp;
    vblock.children = children_tmp;
    vblock.inner_modules = mod_defs_tmp;
    vblock.inner_moddefs = mod_defs_tmp_str;
    vblock.orphans = orphans_tmp;
    vblock.sourcefile_ = SRC;
    // build subhierarchy before returning block
    vblock.__build_subhierarchy__(TAB);


    // Add module_definitions
    for (auto& mdt: mod_defs_tmp) {
        bool found_it = false;
        for (int i = 0; i < module_definitions.size(); i++){
            //std::cout << module_definitions[i].name << " & " << &(module_definitions[i]);
            //std::cout << "\n " << mdt.name << " & " << &mdt << std::endl;
            if (&module_definitions[i] == &mdt){
                found_it = true;
            }
        }
        if (!found_it) {
            module_definitions.push_back(mdt);
            module_references.insert(std::pair<std::string,VerilogBlock>(mdt.name,mdt));
        }
    }


    // Make sure we append children_tmp to children
    for (auto& c: children_tmp){
        if (std::find(children.begin(), children.end(), c) == children.end()) children.push_back(c);
    }

    // To get the value of duration use the count()
    // member function on the duration object

    int seconds = duration.count();
    int minutes = seconds / 60;
    int hours = minutes / 60;
    seconds = (seconds%60);

    std::basic_string message_out = "";
    message_out = string_format("%s#--- Completed in %02d:%02d:%02d (hh:mm:ss) --- #",TAB.substr(0,TAB.length()-3).c_str(),hours,minutes,seconds);
    std::cout << std::endl << message_out;

    // Return block
    return vblock;
}


int Parser::__count_number_lines__(const char *FILENAME) {

    // Count total number of lines in file
    std::ifstream f(FILENAME);
    std::string l;
    size_t nlines = 0; while (std::getline (f, l)) nlines++;

    return nlines;
}


// Create a general array matcher. This will return one if ANY of the
// strings in the input array is present in the input string
bool LineChecker(std::string mainStr, std::vector<std::string> patterns, int n_patterns) {
    bool out = false;
    int m = mainStr.length();
    for (int i =0; i < n_patterns; i++){
        int n = patterns[i].length();
        bool out_tmp = stringStartsWith(mainStr, patterns[i]);
        if (!out_tmp){
            if (m > n){
                out_tmp &= !(std::isalnum(mainStr[n]));
            }
        }
        out |= out_tmp;

        // At any time, if we found a single true case, break and return.
        if (out) break;
    }
    return out;
}


// BlockComment parser
std::string BlockCommentParser::parse(progressBar& pbar, std::string TAB){

    std::string line = "";
    std::string message = "";
    end_line = start_line;

    // Our duty is to keep reading lines until we find the character "*\"
    while (!stream.eof()) //!stream.eof()
    {
        // If the previous block is empty we for sure need to process the new line
        getline(stream, line);

        // Increase iline
        end_line++;

        // Left-trim the line
        ltrim(line);

        // If empty string, go to next line
        if (line == "") continue;

        int nends = string_count(line, __VLOG_BLOCK_COMMENT_END_REGEX__);

        if (!content.empty()) content = content + '\n' + line; else content = line;

        if (nends > 0) {
            // We closed the comment. Discard now
            message = string_format("Block comment found from line %d to line %d (ignored)",
                                    start_line, end_line);

            // Update progress bar
            pbar.update(end_line, message, TAB);

            return "";
        }
    }
}




// Block BeginEnd parser
std::string BlockBeginEnd::parse(progressBar& pbar, std::string TAB){

    std::string line = "";
    std::string message = "";
    end_line = start_line;
    std::string prev_block = "";

    // If we find another BEGIN create a new parser object, if not, end and return
    while (!stream.eof()) //!stream.eof()
    {
        // If the previous block is empty we for sure need to process the new line
        getline(stream, line);

        // Increase iline
        end_line++;

        // Left-trim the line
        ltrim(line);

        // If empty string, go to next line
        if (line == "") continue;

        int nbegins = string_count(line, __VLOG_BEGIN_REGEX__);
        int nends = string_count(line, __VLOG_END_REGEX__);

        // If nbegins > nends (usually nbegins == 1 and nends == 0), start new block
        if (nbegins > nends){
            BlockBeginEnd block(stream,"",end_line);
            prev_block = block.parse(pbar, TAB + "\t");
            end_line = block.end_line;

        } else if (nbegins < nends) {
            // We found the fnal end, let's close this and return
            message = string_format("Begin/End statement found from line %d to %d (ignored)",
                                    start_line, end_line);

            // Update progress bar
            pbar.update(end_line, message, TAB);

            return prev_block;
        } else if (LineChecker(line, __VLOG_PATTERNS_NON_NESTABLE_KEYWORDS__, __VLOG_NUM_PATTERNS_NON_NESTABLE_KEYWORDS__)) {
            // We found the fnal end, let's close this and return
            message = string_format("Begin/End statement found from line %d to %d (ignored)",
                                    start_line, end_line);

            // Update progress bar
            pbar.update(end_line, message, TAB);

            return line;
        }
    }
}
