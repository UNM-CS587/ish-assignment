#include "parser.hpp"

#include <iostream>
#include <ostream>
#include <istream>

namespace ish {

void issuePrompt(std::ostream &out)
{
    out << "> " << std::flush;
}

void processCommands(std::istream &istr, std::ostream& ostr, bool interactive)
{
    while (!istr.eof() ) {
        std::string input;
        std::vector<ish::command> output;

        if (interactive) {
            issuePrompt(ostr);
        }

        std::getline(istr, input);

        auto f = input.begin(),
             l = input.end();
        if (ish::parser::parseCommands(f, l, output))  {
            for (auto &cmd: output) {
                //Debug point: prints out the entire command set, arguments and any parsed redirections. 
                //ostr << cmd << "\n";

                //THIS IS WHERE ALL THE BUILT-INS NEED TO BE HANDLED.
                //This is how  you get the command name
                ostr << cmd.getName() << "\n";
                if (cmd.getName() == "quit"){
                    exit(0);
                }

            }               
        } else {
            ostr << "Parse error: " << input << "\n";
        }
    }
}


} //namespace ish

int main(int argc, char *argv[])
{
    ish::processCommands(std::cin, std::cout, true);
    return 0;
}


