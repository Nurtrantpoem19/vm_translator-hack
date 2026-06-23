#include "translator/Parser.hpp"
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;

class ParserTest : public ::testing::Test
{
  protected:
    fs::path tempFile;

    // Helper method to create a file with specific contents for a test case
    void CreateTestFile(const std::string &content)
    {
        tempFile = fs::temp_directory_path() / "temp_vm_test.vm";
        std::ofstream out(tempFile);
        out << content;
        out.close();
    }

    void TearDown() override
    {
        if (fs::exists(tempFile))
        {
            fs::remove(tempFile);
        }
    }
};

TEST_F(ParserTest, HandlesEmptyFile)
{
    CreateTestFile("");
    Parser parser(tempFile);

    EXPECT_FALSE(parser.advance());
}

TEST_F(ParserTest, SkipsCommentsAndEmptyLines)
{
    std::string input = "// This is a comment\n"
                        "   \n"
                        "push constant 7\n"
                        "// Another comment\n"
                        "pop local 0\n";

    CreateTestFile(input);
    Parser parser(tempFile);

    // First valid command
    EXPECT_TRUE(parser.advance());
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Push);
    // Second valid command
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Pop);
    // After advancing past 'pop local 0', the file reaches EOF
    EXPECT_FALSE(parser.advance());
}

TEST_F(ParserTest, InlineCommentsAreStripped)
{
    CreateTestFile("push constant 10 // inline comment here");
    Parser parser(tempFile);

    parser.advance();
    // If inline comments are stripped correctly, commandType shouldn't choke
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Push);
}

// testing commandType
//
TEST_F(ParserTest, IdentifiesCorrectCommandTypes)
{
    std::string input =
        "push\npop\nlabel\ngoto\nfunction\nreturn\nconstant\nadd\n";
    CreateTestFile(input);
    Parser parser(tempFile);

    // push
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Push);

    // pop
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Pop);

    // label
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Label);

    // goto
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Goto);

    // function
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Function);

    // return
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Return);

    // call
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Call);

    // arithmetic default (e.g., add)
    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Arithmetic);
}

TEST_F(ParserTest, ParsesArgumentsCorrectly)
{
    CreateTestFile("push local 5");
    Parser parser(tempFile);

    parser.advance();

    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Push);

    EXPECT_EQ(parser.arg1(), "local");

    EXPECT_EQ(parser.arg2(), 5);
}

TEST_F(ParserTest, HandlesArithmeticArg1)
{
    // For arithmetic operations like "add", arg1() usually returns the command
    // itself
    CreateTestFile("add");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Arithmetic);
    EXPECT_EQ(parser.arg1(), "add");
}
