#include "parser.hpp"

#include <iostream>
#include <iterator>

namespace ish {
void repl(std::string command)
{
        std::vector<ish::command> output;
        parser::parse_command(command.begin(), command.end(), output);
        for (auto c: output) {
            std::cout << c;            
        }

}

} //namespace ish

int main(int argc, char *argv[])
{
        ish::repl("cd hello/world");
        return 0;
}


