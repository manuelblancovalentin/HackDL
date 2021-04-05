//
// Created by Manuel Blanco Valentin on 1/8/21.
//

#ifndef HLSPY_BASIC_H
#define HLSPY_BASIC_H

#include <regex>

// Define Path separator to build dirs and files
#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif


template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}


int string_count(std::string mainStr, std::regex pattern);

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

bool stringStartsWith(std::string mainStr, std::string toMatch);
bool file_exists (const std::string& name);

bool endsWith(const std::string& str, const std::string& suffix);

bool startsWith(const std::string& str, const std::string& prefix);

std::vector<std::string> stringSplit(const std::string& s, char delimiter);


// This helps us check if an object is isntance of class or not
template<typename Base, typename T>
bool instanceof(const T*) {
    return std::is_base_of<Base, T>::value;
}

#endif //HLSPY_BASIC_H
