//
// Created by Manuel Blanco Valentin on 1/19/21.
//

#ifndef HLSPY_TASKERS_H
#define HLSPY_TASKERS_H

int generate_monitor_tasks(std::string FILENAME, std::vector<std::vector<std::string>>& subset);
int generate_see_tasks(std::string FILENAME, std::vector<std::vector<std::string>>& subset,
                       int simultaneous_pulses = 1, double MAX_UPSET_TIME = 10, double MIN_UPSET_TIME = 1);

#endif //HLSPY_TASKERS_H
