//
// Created by Manuel Blanco Valentin on 1/7/21.
//

#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <regex>
#include <sys/stat.h>

// Function to count occurrences of a certain pattern in string
int string_count(std::string mainStr, std::regex pattern){
    // Initialize occurrences
    int occurrences = 0;

    // finding all the match.
    for (std::sregex_iterator it = std::sregex_iterator(mainStr.begin(), mainStr.end(), pattern);
         it != std::sregex_iterator(); it++) {
        std::smatch match;
        match = *it;
        /*
        cout << "\nMatched  string is = " << match.str(0)
             << "\nand it is found at position "
             << match.position(0) << endl;
        cout << "Capture " << match.str(1)
             << " at position " << match.position(1) << endl;
        */
        occurrences += 1;
    }

    return occurrences;
}


// trim from start (in place)
void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


bool stringStartsWith(std::string mainStr, std::string toMatch)
{
    // std::string::find returns 0 if toMatch is found at starting
    if(mainStr.find(toMatch) == 0)
        return true;
    else
        return false;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}



// This function was obtained from: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
bool file_exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

