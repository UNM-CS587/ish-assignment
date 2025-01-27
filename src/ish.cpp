#include "parser.hpp"

#include <iostream>

namespace ish {
void repl(std::string command)
{
        std::vector<ish::command> output;
        parser::parse_command(command.begin(), command.end(), output);
        for (auto c: output) {
            std::cout << c << "\n";            
        }

}

} //namespace ish

int main(int argc, char *argv[])
{
        ish::repl("command1 argument1.1 argument1.2 > out/file1 < in/file2 & command2 argument2.1 argument2.2;");
        return 0;
}


