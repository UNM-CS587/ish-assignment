#include "command.hpp"

namespace ish {
    
std::ostream& operator<<(std::ostream& out, const ish::command& cmd)
{
   
    out << "[ Command: " << "\n";
    out << "  name: " << cmd.getName() << "\n"
        << "  args:\n";

    for (auto arg : cmd.getArguments())
    {
        out << "\t" << arg << "\n";
    }
    std::string path;
    bool err, append;
    if (cmd.getRedirectOutput(path, err, append)) {
        if (append) out << "  Append Output";
        else out << "  Redirect Output";

        if (err) out << "and Error:" << path << "\n";
        else out << ": " << path << "\n";
    }

    if (cmd.getRedirectInput(path)) {
        out << "  Redirect Input: " << path << "\n";
    }

    if (!cmd.getForeground()) out << "  Background Job\n";
    out << "]";
    return out;
}

} // namespace ish