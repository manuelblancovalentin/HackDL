//
// Created by mbv6231 on 4/9/21.
//

#ifndef HACKDL_TMR_H
#define HACKDL_TMR_H

void triplicate_modules(std::vector<std::string>& TMR,
                        std::vector<std::vector<std::string>>& subsets,
                        std::map<std::string,VerilogBlock>& module_references,
                        std::map<std::string, std::string> __serial_instances__,
                        std::map<std::string, std::string> __serial_sources__,
                        std::string TMR_SUFFIX = "TMR",
                        std::string OUTPATH = "");

#endif //HACKDL_TMR_H
