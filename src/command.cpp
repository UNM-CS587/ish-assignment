#include "command.hpp"

namespace ish {
    
std::ostream& operator<<(std::ostream& out, const ish::command& cmd)
{
    out << "command: " << cmd.getName() << "\n"
        << "args:\n";


    for (auto arg : cmd.getArguments())
    {
        out << "\t" << arg << "\n";
    }

    if (!cmd.getForeground()) out << "[background job]\n";
    return out;
}

} // namespace ish