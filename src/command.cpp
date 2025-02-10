#include "command.hpp"
#include <unistd.h>
#include <sys/wait.h>

namespace ish {
    
std::ostream& operator<<(std::ostream& out, const ish::command& cmd)
{
   
    out << "[ Command: " << "\n";
    out << "  name: " << cmd.getName() << "\n"
        << "  args:\n";
    // Get all arguments out of cmd. Meaning cmd holds all args?
    // Use cmd.getName(), take advantage of the fact that we can make a command from that
    // May need binding from what is entered into the command?
    // What is command and what is the built in?
    //
    std::vector<std::string> arg_str;
    for (auto arg : cmd.getArguments())
    {
        out << "\t" << arg << "\n";
        arg_str.push_back(arg);
    }

    for (auto arg_s : arg_str) {
        out << "\t" << arg_s << "\n";
    }
    std::string path;
    bool err, append;
    out << "  redirections:\n";
    for (auto redir: cmd.getRedirections()) {
        switch(redir.type) {
        case REDIRECT_IN:
            out << "\tinput: ";
            break;
        case REDIRECT_OUT:
            out << "\toutput: ";
            break;
        case REDIRECT_APPEND:
            out << "\tappend output: ";
            break;
        case REDIRECT_OUTERR:
            out << "\toutput and error: ";
            break;
        case REDIRECT_APPENDERR:
            out << "\tappend output and error: ";
            break;
        }
        out << redir.path << "\n\n\n";
    }
        if (!cmd.getForeground()) out << "  Background Job\n";
    // out << "]";


    int rc = fork();
    if (rc < 0) {
        std::cerr << "Fork error out!\n";
        exit(1);
    } else if (rc == 0) {
        std::string output = "Child process ";
        output += std::to_string(getpid());
        out << output;
        char *myargs[1];
        myargs[0] = "-al";
        char *myenvs[0];

        std::string command_str = "/usr/bin/" + cmd.getName();
        char *cmd_lit = command_str.data();
        int rc_exec = execve(cmd_lit, myargs, myenvs);
    } else {
        int rc_wait = wait(NULL);
        out << "parent waiting";
    }
    //out << "args" << cmd.getArguments();
    //int return_call = execve("/usr/bin/ls", cmd.getArguments(), NULL);


    return out;
}

} // namespace ish
