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
        :name(n), isForeground(true), redirAppend(false), redirError(false) {
        arguments = std::vector<std::string>();
    }

    command( ) 
        :isForeground(true), redirAppend(false), redirError(false) {
    }

    void registerArgument(std::string arg) {
        arguments.push_back(arg);
    }
    void setBackground() {
        isForeground = false;
    }
    void setForeground() {
        isForeground = true;
    }
    bool getForeground() const { return isForeground; }
    std::string getName() const { return name; }
    std::vector<std::string> getArguments() const { return arguments; }
};

std::ostream& operator<<(std::ostream& out, const ish::command& cmd);

} // namespace ish