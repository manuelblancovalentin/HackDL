// Basic libs
#include <iostream>
#include <fstream>
#include <filesystem>

// Utilities
#include "./verilog/design.h"
#include "helpers/flags.h"
#include "./verilog/taskers.h"
#include "helpers/basic.h"
#include "./args/args.hxx"
#include "./verilog/TMR.h"

/* Here we only approximate the echo functionality */
int process(std::string NAME,
            FLAGS flags,
            std::vector <std::string> SOURCES, std::vector <std::string> LIB,
            std::string OUTPATH,
            std::vector<std::string> PATTERNS,
            int SIM_PULSES = 1, int MAX_UPSET_TIME = 10, int MIN_UPSET_TIME = 1,
            int TMR = 0, std::string TMR_SUFFIX = "TMR"){

    // Build outfile
    if (endsWith(OUTPATH,PATH_SEPARATOR)) {OUTPATH = OUTPATH.substr(0,OUTPATH.size()-1);}
    std::string OUTJSON = OUTPATH + PATH_SEPARATOR + NAME + "_hierarchy.json";
    std::string OUTTXT = OUTPATH + PATH_SEPARATOR + NAME + "_tree.txt";
    std::string OUTMONITOR = OUTPATH + PATH_SEPARATOR + NAME + "_monitors.v";
    std::string OUTSEE = OUTPATH + PATH_SEPARATOR + NAME + "_see.v";

    // Check if outpath exists, if not, create it
    if (!dir_exists(OUTPATH.c_str())){
        std::filesystem::create_directory(OUTPATH);
        std::cout << "[INFO] - Output directory did not exist. Creating it at " + OUTPATH << std::endl;
    }

    // Check if json file exists
    Hierarchy h;
    std::ifstream json_stream(OUTJSON);
    if (json_stream.fail() & !(flags & FLAGS::RELOAD)){
        // Init Design
        Design d(NAME, SOURCES, LIB, &flags);
        // Get hierarchy
        h = d.get_hierarchy();
        // Plot hierarchy to file
        h.save_json(OUTJSON);
    } else {
        // Load to obtain json hierarchy file
        h.load_json(OUTJSON);
    }

    // Print tree to console
    //h.print();
    // Print tree to file
    h.print_txt(OUTTXT);

    // Find patterns
    std::vector<std::vector<std::string>> subsets = h.subset(PATTERNS);

    // Triplication of modules
    if (flags & FLAGS::TMR) triplicate_modules(TMR, TMR_SUFFIX, subsets);

    // Monitor tasks
    if (flags & FLAGS::MONITOR) generate_monitor_tasks(OUTMONITOR,subsets);

    // SEE tassks
    if (flags & FLAGS::SEE) generate_see_tasks(OUTSEE, subsets, SIM_PULSES, MAX_UPSET_TIME, MIN_UPSET_TIME);

    return 0;
}


int main(int argc, char* argv[])
{

    // Define vars
    std::string NAME;
    std::vector <std::string> SOURCES = {};
    std::vector <std::string> LIB = {};
    std::string OUTPATH;
    std::vector<std::string> PATTERNS = {};
    int SIM_PULSES = 2;
    int MAX_UPSET_TIME = 10;
    int MIN_UPSET_TIME = 1;
    std::string TMR_SUFFIX = "TMR";
    int TMR = 0;

    // Setup flags
    FLAGS flags(FLAGS::NONE);

    args::ArgumentParser parser("HackDL program.", "For extra info visit: github.com/manuelblancovalentin/HackDL");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

    // Flags
    args::Flag reload(parser, "reload", "Reload the hierarchy in case found in disk", {"reload"});
    args::Flag auto_include(parser, "auto-include", "Automatically include and parse files when an `include directive is found", {"auto-include"});
    args::Flag monitor(parser, "monitor", "Generate monitor v files on subset defined by reg ex patterns", {"monitor"});
    args::Flag see(parser, "see", "Generate see tasks v files on subset defined by reg ex patterns", {"SEE"});

    // TMR
    args::ValueFlag<int> tmr(parser, "tmr", "Triplicate modules defined as subset by reg ex patterns\n\t0 -> No "
                                       "triplication "
                                  "(Default mode)\n\t1 -> Only Registers\n\t2 -> Registers and clock\n\t3 -> Full "
                                  "TMR\n\t4 -> Clock skewing TMR (beta)", {"TMR"});

    // Sources
    args::ValueFlagList<std::string> sources(parser, "source", "List of sources", {'s',"source"});
    args::ValueFlagList<std::string> libs(parser, "lib", "List of libs", {'l',"lib"});

    // Outpath
    args::ValueFlag<std::string> outpath(parser, "outpath", "Path of output results", {'o',"output"});

    // Regex patterns
    args::ValueFlagList<std::string> patterns(parser, "pattern", "Regular expressions to be matched", {'p',"pattern"});

    // SEE params
    args::Group seecommands(parser, "SEE parameters");
    args::ValueFlag<int> simultaneous_pulses(seecommands, "simultaneous-pulses", "Number of simultaneous pulses for SEE", {"sim-pulses"});
    args::ValueFlag<int> max_upset_time(seecommands, "max-upset-time", "Max upset time for upset pulses for SEE", {"max-upset-time"});
    args::ValueFlag<int> min_upset_time(seecommands, "min-upset-time", "Min upset time for upset pulses for SEE", {"min-upset-time"});

    // TMR params
    args::Group tmrcommands(parser, "TMR parameters");
    args::ValueFlag<std::string> tmr_suffix(tmrcommands, "tmr-suffix","Suffix to be added to the triplicated module "
                                                                      "definition",{"tmr-suffix"});


    //args::ValueFlag<int> integer(parser, "integer", "The integer flag", {'i'});
    //args::ValueFlagList<char> characters(parser, "characters", "The character flag", {'c'});
    args::ValueFlag<std::string> name(parser, "name", "Name of the design", {'n',"name"});
    //args::Positional<std::string> foo(parser, "foo", "The foo position");
    //args::PositionalList<double> numbers(parser, "numbers", "The numbers position list");
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (name) { NAME = args::get(name); }
    if (reload) { flags |= FLAGS::RELOAD; }
    if (auto_include) { flags |= FLAGS::AUTO_INCLUDE; }
    if (monitor) { flags |= FLAGS::MONITOR;}
    if (see) { flags |= FLAGS::SEE;}
    if (sources) { for (const auto s: args::get(sources)) {SOURCES.push_back(s);}; }
    if (outpath) {OUTPATH = args::get(outpath);}
    if (libs) { for (const auto l: args::get(libs)) {LIB.push_back(l);}; }
    if (patterns) { for (const auto p: args::get(patterns)) {PATTERNS.push_back(p);}; }

    // TMR params
    if (tmr) {
        flags |= FLAGS::TMR;
        TMR = args::get(tmr);
    }
    if (tmr_suffix) {TMR_SUFFIX = args::get(tmr_suffix);}

    // SEE params
    if (simultaneous_pulses) {SIM_PULSES = args::get(simultaneous_pulses);}
    if (max_upset_time) {MAX_UPSET_TIME = args::get(max_upset_time);}
    if (min_upset_time) {MIN_UPSET_TIME = args::get(min_upset_time);}

    //if (characters) { for (const auto ch: args::get(characters)) { std::cout << "c: " << ch << std::endl; } }
    //if (foo) { std::cout << "f: " << args::get(foo) << std::endl; }
    //if (numbers) { for (const auto nm: args::get(numbers)) { std::cout << "n: " << nm << std::endl; } }

    // Now pass to process
    return process(NAME, flags, SOURCES, LIB, OUTPATH, PATTERNS,
                   SIM_PULSES, MAX_UPSET_TIME, MIN_UPSET_TIME,
                   TMR, TMR_SUFFIX);

}

