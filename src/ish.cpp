#include "parser.hpp"

#include <iostream>
#include <ostream>
#include <istream>
#include <filesystem>

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
                
                if (cmd.getName() == "cd"){
                    //Figure out where we're going to cd to.
                    
                    //Check to see if cd has any arguments.
                    if (cmd.getArguments().size() == 0){
                        ostr << "No arguments for cd, I should cd home\n";
                        //How do I figure out the home directory? Usually that's one of the environment vars... 
                        std::string homedir = getenv("HOME");
                        //Now I just need to use a syscall
                        if(std::filesystem::exists(homedir.c_str())){
                            if(std::filesystem::is_directory(homedir.c_str())){
                                chdir(homedir.c_str());
                            } else {
                                ostr <<"Error: Path is not a directory\n";
                            }

                        } else {
                            ostr << "Error: Path does not exist\n";
                        }
                        continue;
                    }

                    //Check if cd has too many args.
                    if (cmd.getArguments().size() > 1){
                        ostr << "Too many arguments for cd. Usage: cd [dir]\n";
                        continue;
                    }

                    std::string target_dir = cmd.getArguments()[0];
                    ostr << "about to cd to: " << target_dir << "\n";
                    //gotta check to see if the path given is actually a directory and not just a regular file
                    //also gotta check to see if the path is real. Need to validate it somehow. 
                    if(std::filesystem::exists(target_dir.c_str())){
                        if(std::filesystem::is_directory(target_dir.c_str())){
                            chdir(target_dir.c_str());
                        } else {
                            ostr << "Error: Path is not a directory\n";
                        }

                    } else {
                        ostr << "Error: Path does not exist\n";
                    }
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


