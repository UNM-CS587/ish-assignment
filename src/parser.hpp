/* 
 * Header-only Boost Spirit X3 parser for the CS587 ISH project
 * Author: Patrick Bridges <pat
 */

#pragma once

#include <deque>
#include <string> 
#include <iostream>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

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
    = x3::lexeme['"' >> *( escapes | ~x3::char_('"')) >> '"'];
auto const squote = x3::rule<class squote, std::string>() 
    = x3::lexeme['\'' >> *( escapes |  ~x3::char_('\'')) >> '\''];
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


/* Create a class to capture state of a redirection while parsing. Handed from the 
 * redirection handling to the construction of the command that uses the redirection. */
struct redirection {
    enum {REDIRECT_IN, REDIRECT_OUT} type;
    bool redirectErr, redirectAppend;
    std::string path;
};

/* The rule which turns a redirection into a redirectin structure*/
const x3::rule<class redirect_rule, ish::parser::redirection> redirect = "redirect";
auto const redirect_def = (x3::lexeme[(x3::string(">")
                           | x3::string(">&")
                           | x3::string(">>")
                           | x3::string(">>&")
                           | x3::string("<"))] 
                           >> token)[(
    [](auto &context)
    {
            const auto& redir = get<std::string>(at_c<0>(_attr(context)));
            auto path = at_c<1>(_attr(context));
            struct redirection r;

            r.redirectAppend = false;
            r.redirectErr = false;
            r.path = path;
            if (redir[0] == '>') {
                r.type = redirection::REDIRECT_OUT;
                if (redir.length() > 1) {
                    if (redir[1] == '>') {
                        r.redirectAppend = true;
                        if (redir.length() > 2) {
                            r.redirectErr = true;
                        }
                    } else {
                        r.redirectErr = true;
                    }
                } 
            } else if (redir[0] == '<') {
                r.type = redirection::REDIRECT_IN;
                if (redir.length() > 1) {
                    r.redirectErr = true;
                }
            }
            _val(context) = std::move(r);
    })];
BOOST_SPIRIT_DEFINE(redirect);

/* Almost there: The code to turn a command (a path followed by a list of either 
 * redirections or arguments into command structure. This is complicated because any
 * piece of the rest of the command line can be either a simple string (an argument) 
 * or a redirection symbol followed by a simple string (the filename). As a result,
 * we have to walk this list of arguments and check each if it's a redirection or 
 * argument, and then add it to the command, as encountered. */
const x3::rule<class command_rule, class ish::command> command = "command";
auto const command_def = (token >> *(token|redirect))[(
    [](auto& context)
    {
        auto& command_path = at_c<0>(_attr(context));
        const auto& redir_or_args_list = at_c<1>(_attr(context));
        ish::command cmd(command_path);

        for (const auto& redir_or_arg: redir_or_args_list) {
            if (redir_or_arg.type() == typeid(ish::parser::redirection)) {
                auto r = get<ish::parser::redirection>(redir_or_arg);
                if (r.type == redirection::REDIRECT_OUT) {
                    cmd.registerRedirectOut(r.path, r.redirectErr, r.redirectAppend);
                } else {
                    cmd.registerRedirectIn(r.path);
                }

            } else {
                auto args = get<std::string>(redir_or_arg);
                cmd.registerArgument(args);
            }
        }

        _val(context) = std::move(cmd);
    })];
BOOST_SPIRIT_DEFINE(command);


/* Final step:
 * Identify background jobs and ;-seperated sequences of jobs. 
 * The following rule gets three attributes:
 * 1) An ISH command
 * 2) A character separator (eoln/eoi, semicolon, or ampersand)
 * 3) A std:optional<std::vector<ish::comamand>>, the rest of the command.
 *
 * Basically, a command sequence is a command followed by a separator. That separator 
 * may be the end of input. We need to separator *following* a command, so we use 
 * right recursion to get the command, the following separator, and then the remainder 
 * of the list. As the recursive parser comes back up the list, we put the commands
 * that were construted on the heqad of the list. Note that we use a deque here and
 * not a vector because we need to insert at the begining because deques are compatible 
 * with vectors, the caller doesn't need to know that.
 */
auto const separator = x3::char_("&;");

/* We don't use the Boost Spirit sequence parser (%) because we care about what the 
 * seperator is. 
 */
const x3::rule<class commandseq_rule, std::deque<class ish::command>> commandseq = "commandseq";
auto const commandseq_def = (command >> -(separator >> -commandseq))[(
        [](auto &context) {
            auto& cmd = at_c<0>(_attr(context));
            const auto& seq_opt = at_c<1>(_attr(context));

            std::deque<ish::command> cmdlist;

            if (seq_opt.has_value()) {
                const char separator = at_c<0>(seq_opt.value());
                const auto& cmdlist_opt = at_c<1>(seq_opt.value());

                if (separator == '&') {
                    cmd.setBackground();
                }
                if (cmdlist_opt.has_value()) {
                    cmdlist = std::move(cmdlist_opt.value());
                }
            } 
            cmdlist.push_front(cmd);
            _val(context) = std::move(cmdlist);
        })];
BOOST_SPIRIT_DEFINE(commandseq);

auto const grammar = x3::rule<class grammar, std::deque<ish::command>>()
    = (-commandseq >> (eoln | x3::eoi))[(
        [](auto& context)
        {
            auto& cmd_opt = _attr(context);
            std::deque<ish::command> v;
            if (cmd_opt.has_value()) {
                v = cmd_opt.value();
            }
            _val(context) = std::move(v);
        })];


template <class Iterator>
bool parseCommands(Iterator &begin, Iterator &end, std::vector<ish::command> &output) 
{
    return x3::phrase_parse(begin, end, grammar, x3::ascii::space - x3::char_('\n'), output);
};  

} // Namespace parser
} // Namespace isp
