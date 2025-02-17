#include "parser.hpp"

#include <iostream>
#include <ostream>
#include <istream>
#include <filesystem>
#include <sys/types.h>
#include <pwd.h>

namespace ish {

void issuePrompt(std::ostream &out)
{
    //TODO change this to "hostname% " and get the hostname somehow...
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
                        //How do I figure out the home directory? Usually that's one of the environment vars... we can't use getenv, but we can use getpwuid()...
                        int uid = getuid();
                        ostr << "uid: " << uid << "\n";
                        std::string homedir = getpwuid(uid)->pw_dir;
                        ostr << "homedir: " << homedir << "\n";
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
                    //TODO also need to check to see if we have permissions to read that file. 
                    if(std::filesystem::exists(target_dir.c_str())){
                        if(std::filesystem::is_directory(target_dir.c_str())){
                            chdir(target_dir.c_str());
                        } else {
                            //TODO Does this need to be a perror()?
                            ostr << "Error: Path is not a directory\n";
                        }

                    } else {
                        ostr << "Error: Path does not exist\n";
                    }
                }

                if (cmd.getName() == "setenv"){
                    //TODO
                }

                if (cmd.getName() == "unsetenv"){
                    //TODO
                }

                if (cmd.getName() == "alias"){
                    //TODO
                }

                if (cmd.getName() == "unalias"){
                    //TOOD
                }

                if (cmd.getName() == "bg"){
                    //README makes it sound like we don't have to implement this yet. 
                }

                if (cmd.getName() == "fg"){
                    //README makes it sound like we don't have to implement this yet. 
                }

                if (cmd.getName() == "jobs"){
                    //README makes it sound like we don't have to implement this yet. 
                }

                if (cmd.getName() == "kill"){
                    //README makes it sound like we don't have to implement this yet. 
                }

                //TODO If we get here, the command is not a builtin, and we should assume it's a command the user specified, check the file exists, check the permissions to see if its executable, fork exec etc. 

            }               
        } else {
            ostr << "Parse error: " << input << "\n";
        }
    }
}


} //namespace ish

int main(int argc, char *argv[])
{
    //TODO create a structure of some kind to manage the environment variables. 

    //TODO read in PATH environment var, put that into the structure above. TODO do we assume that PATH is going to come from .ishrc, or are we allowed to use getenv for this particular environment var?
    //I asked my question in the discord. 

    //TODO need to setup handling of the TERM signal

    //TODO need to read in the ./ishrc file, maybe call "processCommands" on this file.
    ish::processCommands(std::cin, std::cout, true);
    return 0;
}


