/* 
 * Header defining the structure for ISH commands and redirections used
 * by the CS587 ISH project parser
 * Author: Patrick Bridges <patrickb@unm.edu>
 */

#pragma once

#include <string> 
#include <iostream>
#include <boost/spirit/home/x3.hpp>


namespace ish {
    
class redirection {
    int foo;
};

class command {
    std::string name;
    std::vector<std::string> arguments;
    bool isForeground;

    // Information about rediretions in this command
    std::optional<std::string> inputFile;
    bool redirAppend;
    bool redirError;      
    std::optional<std::string> outputFile;


public:
    command( std::string n ) 
        :name(n) {
        arguments = std::vector<std::string>();
    }

    command( ) {}

    void register_argument(std::string arg) {
        arguments.push_back(arg);
    }
    std::string getName() const { return name; }
    std::vector<std::string> getArguments() const { return arguments; }
};

std::ostream& operator<<(std::ostream& out, const ish::command& cmd);

} // namespace ish