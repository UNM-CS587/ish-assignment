// Unit tests for the ish command-line parser (src/parser.hpp) against the
// lexical structure, redirection, and command-list syntax described in
// ish.man.pdf. Pipelines and job control (the & separator) are required for
// full credit per README.md, but the provided grammar does not parse them
// yet, so they are not covered here.

#include "parser.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace {

// Parses one line the same way ish.cpp does: std::getline() strips the
// trailing newline before the line ever reaches the parser, so most cases
// below omit it too and rely on the end-of-input branch of the grammar.
bool ParseLine(std::string line, std::vector<ish::command> &commands) {
    auto begin = line.begin();
    auto end = line.end();
    return ish::parser::parseCommands(begin, end, commands);
}

}  // namespace

// ---------------------------------------------------------------------------
// Basic command structure
// ---------------------------------------------------------------------------

TEST(ParserBasicCommands, EmptyLineProducesNoCommands) {
    std::vector<ish::command> commands;
    EXPECT_TRUE(ParseLine("", commands));
    EXPECT_TRUE(commands.empty());
}

TEST(ParserBasicCommands, WhitespaceOnlyLineProducesNoCommands) {
    std::vector<ish::command> commands;
    EXPECT_TRUE(ParseLine("   \t  ", commands));
    EXPECT_TRUE(commands.empty());
}

TEST(ParserBasicCommands, BareCommandNameHasNoArguments) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("ls", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getName(), "ls");
    EXPECT_TRUE(commands[0].getArguments().empty());
}

TEST(ParserBasicCommands, FullPathCommandName) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("/bin/echo", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getName(), "/bin/echo");
}

TEST(ParserBasicCommands, SingleArgument) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("/bin/echo hello", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"hello"}));
}

TEST(ParserBasicCommands, MultipleArguments) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("/bin/echo hello world I am here!", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(),
              std::vector<std::string>({"hello", "world", "I", "am", "here!"}));
}

TEST(ParserBasicCommands, ArgumentsSeparatedByTabsParseTheSameAsSpaces) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("printf\ta\tb", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a", "b"}));
}

TEST(ParserBasicCommands, RepeatedWhitespaceBetweenArgumentsIsCollapsed) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo   a   b", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a", "b"}));
}

TEST(ParserBasicCommands, CommandDefaultsToForeground) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("ls", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_TRUE(commands[0].getForeground());
}

TEST(ParserBasicCommands, BuiltinLikeNameParsesAsAnOrdinaryCommand) {
    // Builtin recognition happens after parsing (man page, "Command
    // Execution"), so the parser must not special-case builtin names.
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("setenv TERM xterm", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getName(), "setenv");
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"TERM", "xterm"}));
}

// ---------------------------------------------------------------------------
// Quoting  ("Strings enclosed in double quotes ... or single quotes ...
// form part or all of a single word. Special characters inside of strings
// do not form separate words.")
// ---------------------------------------------------------------------------

TEST(ParserQuoting, DoubleQuotedArgumentKeepsEmbeddedSpaces) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo \"hello world\" foo", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(),
              std::vector<std::string>({"hello world", "foo"}));
}

TEST(ParserQuoting, SingleQuotedArgumentKeepsEmbeddedSpaces) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo 'a b' c", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a b", "c"}));
}

TEST(ParserQuoting, SpecialCharactersInsideDoubleQuotesDoNotSeparateWords) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo \"a;b\" c", commands));
    ASSERT_EQ(commands.size(), 1u);  // Not two commands split on ';'.
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a;b", "c"}));
}

TEST(ParserQuoting, AdjacentQuotedAndUnquotedTextFormOneWord) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo foo\"bar\"baz", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"foobarbaz"}));
}

// ---------------------------------------------------------------------------
// Escaping  ("Special characters preceded by a backslash character are not
// treated as special characters.")
// ---------------------------------------------------------------------------

TEST(ParserEscaping, BackslashEscapedSpaceStaysInOneWord) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo a\\ b", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a b"}));
}

TEST(ParserEscaping, BackslashEscapedSemicolonIsNotACommandSeparator) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo a\\;b", commands));
    ASSERT_EQ(commands.size(), 1u);  // Not two commands.
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a;b"}));
}

TEST(ParserEscaping, BackslashEscapedRedirectionCharIsNotARedirection) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo a\\>b", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"a>b"}));
    EXPECT_TRUE(commands[0].getRedirections().empty());
}

// ---------------------------------------------------------------------------
// I/O redirection
// ---------------------------------------------------------------------------

TEST(ParserRedirection, OutputRedirectionOverwrite) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd > out.txt", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 1u);
    EXPECT_EQ(commands[0].getRedirections()[0].type, ish::REDIRECT_OUT);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "out.txt");
    EXPECT_TRUE(commands[0].getArguments().empty());
}

TEST(ParserRedirection, OutputRedirectionAppend) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd >> log.txt", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 1u);
    EXPECT_EQ(commands[0].getRedirections()[0].type, ish::REDIRECT_APPEND);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "log.txt");
}

TEST(ParserRedirection, OutputAndErrorRedirection) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd >& both.txt", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 1u);
    EXPECT_EQ(commands[0].getRedirections()[0].type, ish::REDIRECT_OUTERR);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "both.txt");
}

TEST(ParserRedirection, AppendOutputAndErrorRedirection) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd >>& both.txt", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 1u);
    EXPECT_EQ(commands[0].getRedirections()[0].type, ish::REDIRECT_APPENDERR);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "both.txt");
}

TEST(ParserRedirection, InputRedirection) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd < in.txt", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 1u);
    EXPECT_EQ(commands[0].getRedirections()[0].type, ish::REDIRECT_IN);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "in.txt");
}

TEST(ParserRedirection, CombinedInputAndOutputRedirection) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cat < in.txt > out.txt", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 2u);
    EXPECT_EQ(commands[0].getRedirections()[0].type, ish::REDIRECT_IN);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "in.txt");
    EXPECT_EQ(commands[0].getRedirections()[1].type, ish::REDIRECT_OUT);
    EXPECT_EQ(commands[0].getRedirections()[1].path, "out.txt");
    EXPECT_TRUE(commands[0].getArguments().empty());
}

TEST(ParserRedirection, RedirectionTargetIsNotAlsoRecordedAsAnArgument) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd arg1 > out.txt arg2", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"arg1", "arg2"}));
    ASSERT_EQ(commands[0].getRedirections().size(), 1u);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "out.txt");
}

TEST(ParserRedirection, DuplicateOutputRedirectionsAreBothCapturedByTheParser) {
    // The grammar itself does not reject ambiguous redirection (e.g. two
    // output targets on one command) -- it hands both entries to the
    // caller in order. Detecting and reporting the ambiguity, per
    // README.md, is the shell's job, not the parser's.
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("cmd > out1 > out2", commands));
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_EQ(commands[0].getRedirections().size(), 2u);
    EXPECT_EQ(commands[0].getRedirections()[0].path, "out1");
    EXPECT_EQ(commands[0].getRedirections()[1].path, "out2");
}

// ---------------------------------------------------------------------------
// Command lists  (";" separates a sequence of commands on one line)
// ---------------------------------------------------------------------------

TEST(ParserCommandLists, SemicolonSeparatesTwoCommands) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo hi; echo bye", commands));
    ASSERT_EQ(commands.size(), 2u);
    EXPECT_EQ(commands[0].getArguments(), std::vector<std::string>({"hi"}));
    EXPECT_EQ(commands[1].getArguments(), std::vector<std::string>({"bye"}));
}

TEST(ParserCommandLists, SemicolonSeparatesThreeCommands) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo a; echo b; echo c", commands));
    ASSERT_EQ(commands.size(), 3u);
}

TEST(ParserCommandLists, EmptyCommandBetweenTwoSemicolonsFailsToParse) {
    std::vector<ish::command> commands;
    EXPECT_FALSE(ParseLine("echo a;;echo b", commands));
}

TEST(ParserCommandLists, TrailingNewlineAlsoTerminatesTheList) {
    // ish.cpp's own read loop strips the trailing newline with getline()
    // before calling the parser, but the grammar accepts an explicit
    // newline as a list terminator too.
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("echo hi\n", commands));
    ASSERT_EQ(commands.size(), 1u);
}

// ---------------------------------------------------------------------------
// Lexical token characters
// ---------------------------------------------------------------------------

TEST(ParserTokenCharacters, FilenameLikeTokensParse) {
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("gcc -o my-file_name.txt src/file.c", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(),
              std::vector<std::string>({"-o", "my-file_name.txt", "src/file.c"}));
}

TEST(ParserTokenCharacters, ColonSeparatedPathValueParses) {
    // PATH values are colon-separated (man page, "Environment Variables"),
    // and README.md requires "PATH searching works." The token rule's
    // ishpunct set in parser.hpp already includes ':', so a colon-separated
    // value parses as one token.
    std::vector<ish::command> commands;
    ASSERT_TRUE(ParseLine("setenv PATH /bin:/usr/bin", commands));
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].getArguments(),
              std::vector<std::string>({"PATH", "/bin:/usr/bin"}));
}

// ---------------------------------------------------------------------------
// Features the provided grammar does not parse yet
// ---------------------------------------------------------------------------

TEST(ParserUnimplementedFeatures, BareAmpersandDoesNotParseWithoutJobControlSupport) {
    // & (background execution) is required for full credit on the job
    // control tests (README.md). The provided grammar does not recognize it
    // as written; a student adding job control will need to extend the
    // grammar and should update or remove this test then.
    std::vector<ish::command> commands;
    EXPECT_FALSE(ParseLine("ls &", commands));
}
