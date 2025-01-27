/* 
 * Header-only Boost Spirit X3 parser for the CS587 ISH project
 * Author: Patrick Bridges <pat
 */

#pragma once

#include <deque>
#include <string> 
#include <iostream>
#include <boost/spirit/home/x3.hpp>

#include "command.hpp"

namespace ish {

/*
 * A Parsed command line is a vector of ish::command structures. 
 * (1) The top level is the list of commands to execute - there may be 
 *     more than one entry in this list for ';' seperated lists of commands
 * (2) The actual IShH commands are a Command struct as defined below  
 *
 * Credit: The general struture of the Boost::Spirit::X3 recursive decent
 * parser was inspired by the ppsh parser (https://github.com/peter-facko/ppsh).
 * (1) Users the parser to capture whether the command passed is a builtin or   
 * (2) Only captures the name of the file to redirect output to or from, and
 * (3) We don't parse pipelines
 * (4) We only parse up to a newline, not to end of input. The higher-level code
 *     invoking the parser has to handle end of input
 * (5) We handle background commands
 * 
 * Another reference I used when building this parser are the slides of an X3
 * tutorial workshop found here: 
 * https://ciere.com/assets/uploads/cppnow15/using_x3.h-16280204.pdf
 */



namespace parser {

namespace x3 = boost::spirit::x3;


/* Development step:
 * Figure out the basic tokens in the input stream and what characters 
 * are in each of them
 */
auto const eoln = x3::eol;
auto const ishpunct = x3::char_(",./`!@#$%^*=+") | x3::char_('-') | x3::char_('_');

// Escape handling courtesy of 
// https://stackoverflow.com/questions/61695235/creating-a-boostspiritx3-parser-for-quoted-strings-with-escape-sequence-hand
auto const escapes = "\\n" >> x3::attr('\n')
    | "\\b" >> x3::attr('\b')
    | "\\f" >> x3::attr('\f')
    | "\\t" >> x3::attr('\t')
    | "\\v" >> x3::attr('\v')
    | "\\0" >> x3::attr('\0')
    | "\\r" >> x3::attr('\r')
    | "\\n" >> x3::attr('\n')
    | "\\"  >> x3::char_("\"\\");
auto const dquote = x3::rule<class dquote, std::string>() 
    = '"' >> x3::lexeme[ *( escapes | ~x3::char_('"'))] >> '"';
auto const squote = x3::rule<class squote, std::string>() 
    = '\'' >> x3::lexeme[ *( escapes |  ~x3::char_('\''))] >> '\'';
auto const backspecial = '\\' >> x3::char_;

// A token is a what the shell actually uses for arguments and filenames and such
// Need an action to make a std::string out its contents
auto const token = x3::rule<class token, std::string>()
    = x3::lexeme[+(x3::char_("a-zA-Z0-9") | backspecial | ishpunct | dquote | squote) ] [(
        [](auto& context)
        {
            std::string tk;

            // A little complicated because some of the alternatives have character
            // but some (backspecial, squote, dquote) have string attributes. 
            for (auto c : _attr(context))
            {
                if (c.type() == typeid(char)) {
                    tk.push_back(get<char>(c));
                }
                else if ( c.type() == typeid(std::string)) {
                    tk.append(get<std::string>(c));
                }
            }

            _val(context) = std::move(tk);
        })];

/* Development step: 
 * First, the grammar is a list of ISH tokens and so what comes out
 * is a vector of strings
 */
// auto const grammar = +token;

/* Development step:
 * Next, a command is command structure that includes a command name
 * and a list of arguments 
 */

const x3::rule<class command_rule, class ish::command> command = "command";
auto const command_def = (token >> *token)[(
        [](auto& context)
        {
            auto& command_path = at_c<0>(_attr(context));
            const auto& arguments = at_c<1>(_attr(context));
            ish::command cmd(command_path);

            for (auto& args: arguments) {
                /* if (auto * const redirection = std::get_if<ish::redirection>(&*redir_or_arg) ) {

                } else  */
                cmd.registerArgument(args);
            }

            _val(context) = std::move(cmd);
        })];
BOOST_SPIRIT_DEFINE(command);




/* Development step:
 * Identify background jobs and ;-seperated sequences of jobs
 */
auto const background = x3::char_('&');
auto const separator = x3::char_("&;") | eoln | x3::eoi;

/* We don't use the Boost Spirit sequence parser (%) because we care about what the 
 * seperator is. The following rule gets three attributes:
 * 1) An ISH command
 * 2) A character separator (eoln/eoi, semicolon, or ampersand)
 * 3) A std:optional<std::vector<ish::comamand>>, the rest of the command.
 * Note that we use a deque here not a vector because we need to insert at the begining
 * but because deques are compatible with vectors, the caller doesn't need to know that
 */
const x3::rule<class commandseq_rule, std::deque<class ish::command>> commandseq = "commandseq";
auto const commandseq_def = (command >> separator >> -commandseq)[(
        [](auto &context) {
            auto& cmd = at_c<0>(_attr(context));
            const auto& separator = at_c<1>(_attr(context));
            const auto& cmdlist_opt = at_c<2>(_attr(context));

            if (separator == '&') {
                cmd.setBackground();
            } else {
                cmd.setForeground();
            }

            if (cmdlist_opt.has_value()) {
                std::deque<ish::command> cmdlist = std::move(cmdlist_opt.value());
                cmdlist.push_front(cmd);
                _val(context) = std::move(cmdlist);
            } else {
                std::deque<ish::command> cmdlist;
                cmdlist.push_front(cmd);
                _val(context) = std::move(cmdlist);
            } 
        })];
BOOST_SPIRIT_DEFINE(commandseq);
auto const grammar = commandseq;


/* Development step 6:
 * Identify file redireection
 */

/*
auto token = squote | dquote | +(~ishspace);
        auto commandname = token;
        auto arguments = *word;
        auto redirectin = char_('<') >> word;
        auto redirecttruc = char_('>') >> word;
        auto redirectappend = string(">>") >> word;
        auto redirecttruncerr = string(">&") >> word;
        auto redirectappenderr = string(">>&") >> word;
        auto redirectout = redirecttrunc | redirectappend | redirecttruncerr | redirectappenderr;

        auto builtin = x3::lexeme(string("cd") | string("exit") | string("fg") | string())
        auto command = commandname >> *x3::space >> arguments >> -redirectin >> -redirectout >> -background;
        auto commands = command % ';'
*/



template <class Iterator>
auto parse_command(Iterator iter, Iterator end_iter, std::vector<ish::command> &output) 
{
    auto r = x3::phrase_parse(iter, end_iter, grammar, x3::ascii::space, output);
    if (iter != end_iter) {
        auto distance = end_iter - iter;
        std::cout << "Failed: didn't parse everything\n";
        std::cout << "stopped " << distance << " characters from the end "
                  << "( '" << *iter << "' )\n";
        return 1;
    } else if (r) {
        std::cout << "Good input\n";
        return 0;
    } else {
        std::cout << "Parse failed\n";
        return 1;
    }
}

} // Namespace parser
} // Namespace isp
