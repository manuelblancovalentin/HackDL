//
// Created by Manuel Blanco Valentin on 1/8/21.
//

#ifndef HLSPY_DEFINITIONS_H
#define HLSPY_DEFINITIONS_H

#include <string>
#include <regex>

/*
    always         starts an  always begin ... end  sequential code block
    and            gate primitive, and
    assign         parallel continuous assignment
    automatic      a function attribute, basically reentrant and recursive
    begin          starts a block that ends with  end (no semicolon)
    buf            gate primitive, buffer
    bufif0         gate primitive, buffer if control==0
    bufif1         gate primitive, buffer if control==1
    case           starts a case statement
    casex          starts a case statement where x matches
    casez          starts a case statement where z matches
    cell           library, cell identifier, in configuration
    cmos           switch primitive, cmos
    config         starts a configuration
    deassign       stops the corresponding  assign  from accepting new values
    default        optional last clause in a case statement
    defparam       used to over-ride parameter values
    design         top level module, in configuration
    disable        a task or block
    edge           edge control specifier
    else           execute if no previous clause was true
    end            end of a block, paired with a begin
    endcase        end of a case statement
    endconfig      end of a configuration
    endfunction    end of a function definition
    endgenerate    end of a generate
    endmodule      end of a module definition
    endprimitive   end of a primitive definition
    endspecify     end of a specify
    endtable       end of a table definition
    endtask        end of a task definition
    event          data type
    for            starts a for statement
    force          starts net or variable assignment
    forever        starts a loop statement
    fork           begin parallel execution of sequential code
    function       starts a function definition
    generate       starts a generate block
    genvar         defines a generate variable
    highz0         drive strength 0
    highz1         drive strength 0
    if             starts an  if  statement, if(condition) ...
    ifnone         state dependent path declaration
    incdir         file path for library
    include        include file specification
    initial        starts an initial begin ... end sequential block
    inout          declares a port name to be both input and output
    input          declares a port name to be input
    instance       specify instance name, in configuration
    integer        variable data type, 32 bit integer
    join           end of a parallel fork
    large          charge strength, 4, of trireg
    liblist        library search order for modules, in configuration
    library        location of modules, libraries and files
    localparam     starts a local parameter statement, not over-ridden
    macromodule    same as module with possibly extra meanings
    medium         charge strength, 2, of trireg
    module         begin a module definition, also called a cell or component
    nand           gate primitive, nand
    negedge        event expression, negative edge
    nmos           switch primitive, nmos
    nor            gate primitive, nor
    noshowcancelledno report trailing edge precedes leading edge, in specify
    not            gate primitive, not
    notif0         gate primitive, not if control==0
    notif1         gate primitive, not if control==1
    or             gate primitive, or
    output         declares a port name to be an output
    parameter      starts a parameter statement
    pmos           switch primitive, pmos
    posedge        event expression, positive edge
    primitive      starts the definition of a primitive module
    pull0          drive strength 5
    pull1          drive strength 5
    pulldown       gate primitive
    pullup         gate primitive
    pulsestyle_oneventglitch detection, in specify
    pulsestyle_ondetectglitch detection, immediate change to x, in specify
    remos          switch primitive, remos
    real           variable data type, implementation defined floating point
    realtime       variable data type, floating point time
    reg            variable data type, starts a declaration of name(s)
    release        release a forced net or variable assignment
    repeat         starts a loop statement
    rnmos          switch primitive, rnmos
    rpmos          switch primitive, rpmos
    rtran          bidirectional switch primitive, rtran
    rtranif0       bidirectional switch primitive, rtranif0
    rtranif1       bidirectional switch primitive, rtranif1
    scalared       property of a vector type
    showcancelled  report trailing edge precedes leading edge, in specify
    signed         type modifier, reg signed
    small          charge strength, 1,  of trireg
    specify        starts a specify block
    specparam      starts a parameter statement for timing delays
    strong0        drive strength 6
    strong1        drive strength 6
    supply0        net data type, and drive strength 7
    supply1        net data type, and drive strength 7
    table          start a table definition in a primitive
    task           starts a task definition
    time           variable data type, 64 bit integer
    tran           bidirectional switch primitive, tran
    tranif0        bidirectional switch primitive, tranif0
    tranif1        bidirectional switch primitive, tranif1
    tri            net data type
    tri0           net data type, connected to VSS
    tri1           net data type, connected to VDD
    triand         net data type, tri state wired and
    trior          net data type, tri state wired or
    trireg         register data type associates capacitance to the net
    unsigned       type modifier, unsigned
    use            library, cell identifier, in configuration
    vectored       property of a vector type
    wait           starts a wait statement
    wand           net data type, wired and
    weak0          drive strength 3
    weak1          drive strength 3
    while          starts a sequential looping statement, while(condition)
    wire           net data type, a basic wire connection
    wor            net data type, wired or
    xnor           gate primitive, xnor not of exclusive or
    xor            gate primitive, xor exclusive or

 */

// Function/endfunction group
std::regex __VLOG_BLOCK_COMMENT_OPEN_REGEX__("(\\/\\*)");
std::regex __VLOG_BLOCK_COMMENT_END_REGEX__("(\\*\\/)");


// Gate Primitives
int __VLOG_NUM_GATE_PRIMITIVES__ = 14;
std::vector<std::string> __VLOG_GATE_PRIMITIVES__ = {"and ", "buf ", "bufif0 ", "bufif1 ", "nand ", "nor ", "not ",
                                                     "or ", "xnor ", "xor ", "pulldown ", "pullup ", "notif0 ",
                                                     "notif1 "};


// Switch primitive
int __VLOG_NUM_SWITCH_PRIMITIVES__ = 14;
std::vector<std::string> __VLOG_SWITCH_PRIMITIVES__ = {"cmos ", "nmos ", "pmos ", "remos ", "rnmos ", "rpmos ", "rtran ",
                                                       "rtranif0 ", "rtranif1 ", "tran ", "tranif0 ", "tranif1 "};


// Keywords that end with a semicolon
int __VLOG_NUM_SEMICOLON_STATEMENTS__ = 18;
std::vector<std::string> __VLOG_SEMICOLON_STATEMENTS__ = {"assign ", "deassign ", "defparam ", "design ", "default ",
                                                          "instance ", "disable ", "force ", "genvar ", "release ",
                                                          "event ", "wait ", "specparam ", "edge ", "showcancelled ",
                                                          "noshowcancelled ", "pulsestyle_oneventglitch ",
                                                          "pulsestyle_ondetectglitch "};


// NETWIRES
int __VLOG_NUM_PATTERNS_NETWIRES__ = 16;
std::vector<std::string> __VLOG_PATTERNS_NETWIRES__ = {"input", "output", "inout",
                                                       "integer ", "real ", "realtime ", "reg ", "time ",
                                                        "wire ", "supply0 ", "supply1 ", "tri ", "triand ", "trior ",
                                                        "tri0 ", "tri1 ", "wand ", "wor ", "trireg "};

// BEGIN/END STATEMENTS
std::regex __VLOG_BEGIN_REGEX__("\\b(begin)\\b");
std::regex __VLOG_END_REGEX__("\\b(end)\\b");
int __VLOG_NUM_PATTERNS_STATEMENTS__ = 11;
std::vector<std::string> __VLOG_PATTERNS_STATEMENTS__ = {"if ", "else ", "initial ", "always ", "for ",
                                              "while ", "do ", "repeat ", "forever ", "ifnone ", "repeat "};

// PARAMETERS (semicolon statements, but treated separatedly, as we DO want to parse them)
int __VLOG_NUM_PATTERNS_PARAMS__ = 2;
std::vector<std::string> __VLOG_PATTERNS_PARAMS__ = {"localparam ", "parameter "};



// Module|Macromodule/Endmodule group
int __VLOG_NUM_MODULE_NAMES__ = 2;
std::vector<std::string> __VLOG_MODULE_NAMES__ = {"module ", "macromodule "};

std::regex __VLOG_MODULE_REGEX__("\\b(module|macromodule)\\b");
std::regex __VLOG_ENDMODULE_REGEX__("\\b(endmodule)\\b");

// Groups
int __VLOG_NUM_GROUPS__ = 12;
std::vector<std::string> __VLOG_GROUPS__ = {"function ", "task ", "fork ", "case ", "casex ", "casez ",
                                           "generate ", "config ", "primitive ", "specify ", "table ", "task "};


// Function/endfunction group
std::regex __VLOG_FUNCTION_REGEX__("\\b(function)\\b");
std::regex __VLOG_ENDFUNCTION_REGEX__("\\b(endfunction)\\b");

// Fork/Join group
std::regex __VLOG_FORK_REGEX__("\\b(fork)\\b");
std::regex __VLOG_JOIN_REGEX__("\\b(join)\\b");

// Case(Z|X)/Endcase group
int __VLOG_NUM_CASES__ = 3;
std::vector<std::string> __VLOG_CASES__ = {"case ", "casex ", "casez "};

std::regex __VLOG_CASE_REGEX__("\\b(case)\\b");
std::regex __VLOG_CASEX_REGEX__("\\b(casex)\\b");
std::regex __VLOG_CASEZ_REGEX__("\\b(casez)\\b");

std::regex __VLOG_ENDCASE_REGEX__("\\b(endcase)\\b");

// Generate/EndGenerate Group
std::regex __VLOG_GENERATE_REGEX__("\\b(generate)\\b");
std::regex __VLOG_ENDGENERATE_REGEX__("\\b(endgenerate)\\b");

// Config/EndConfig Group
std::regex __VLOG_CONFIG_REGEX__("\\b(config)\\b");
std::regex __VLOG_ENDCONFIG_REGEX__("\\b(endconfig)\\b");

// Primitive/Endprimitive Group
std::regex __VLOG_PRIMITIVE_REGEX__("\\b(primitive)\\b");
std::regex __VLOG_ENDPRIMITIVE_REGEX__("\\b(endprimitive)\\b");

// Specify/EndSpecify Group
std::regex __VLOG_SPECIFY_REGEX__("\\b(specify)\\b");
std::regex __VLOG_ENDSPECIFY_REGEX__("\\b(endspecify)\\b");

// Table/EndTable Group
std::regex __VLOG_TABLE_REGEX__("\\b(table)\\b");
std::regex __VLOG_ENDTABLE_REGEX__("\\b(endtable)\\b");

// Task/EndTask Group
std::regex __VLOG_TASK_REGEX__("\\b(task)\\b");
std::regex __VLOG_ENDTASK_REGEX__("\\b(endtask)\\b");

// keywords in general (attributes)
int __VLOG_NUM_ATTRIBUTES__ = 2;
std::vector<std::string> __VLOG_ATTRIBUTES__ = {"automatic ", "cell "};


#endif //HLSPY_DEFINITIONS_H
