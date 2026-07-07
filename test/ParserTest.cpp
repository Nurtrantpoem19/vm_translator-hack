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
    std::string input = "push\npop\nlabel\ngoto\nfunction\nreturn\ncall\nadd\n";
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

// --- Arithmetic / Logical Commands ---

TEST_F(ParserTest, HandlesArithmeticSub)
{
    CreateTestFile("sub");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Arithmetic);
    EXPECT_EQ(parser.arg1(), "sub");
}

TEST_F(ParserTest, HandlesArithmeticEq)
{
    CreateTestFile("eq");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Arithmetic);
    EXPECT_EQ(parser.arg1(), "eq");
}

// --- Memory Segment Commands ---

TEST_F(ParserTest, HandlesPushCommand)
{
    CreateTestFile("push constant 5");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Push);
    EXPECT_EQ(parser.arg1(),
              "constant"); // arg1 for push/pop is the segment name
}

TEST_F(ParserTest, HandlesPopCommand)
{
    CreateTestFile("pop local 2");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Pop);
    EXPECT_EQ(parser.arg1(), "local");
}

// --- Program Flow Commands ---

TEST_F(ParserTest, HandlesLabelCommand)
{
    CreateTestFile("label LOOP_START");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Label);
    EXPECT_EQ(parser.arg1(), "LOOP_START");
}

TEST_F(ParserTest, HandlesGotoCommand)
{
    CreateTestFile("goto END_FUNCTION");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Goto);
    EXPECT_EQ(parser.arg1(), "END_FUNCTION");
}

TEST_F(ParserTest, HandlesIfGotoCommand)
{
    CreateTestFile("if-goto COMPUTE_AGAIN");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_If);
    EXPECT_EQ(parser.arg1(), "COMPUTE_AGAIN");
}

// --- Function and Call Commands ---

TEST_F(ParserTest, HandlesFunctionCommand)
{
    CreateTestFile("function Main.fibonacci 2");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Function);
    EXPECT_EQ(parser.arg1(), "Main.fibonacci");
}

TEST_F(ParserTest, HandlesCallCommand)
{
    CreateTestFile("call Sys.init 0");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Call);
    EXPECT_EQ(parser.arg1(), "Sys.init");
}

TEST_F(ParserTest, HandlesReturnCommand)
{
    CreateTestFile("return");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Return);
    // Note: Per Nand2Tetris spec, arg1() should not be called if commandType is
    // C_Return.
}

TEST_F(ParserTest, HandlesArg1AsFunctionLabel)
{
    CreateTestFile("function foo 0\nlabel bar\nfunction foo2 1\n");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Function);
    EXPECT_EQ(parser.arg2(), 0);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Label);
    EXPECT_EQ(parser.arg1(), "bar");

    parser.advance();
}

TEST_F(ParserTest, HandlesWhiteSpace)
{
    CreateTestFile("\tadd\t\nlabel      bar\n");
    Parser parser(tempFile);

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Arithmetic);
    EXPECT_EQ(parser.arg1(), "add");

    parser.advance();
    EXPECT_EQ(parser.commandType(), Parser::CommandType::C_Label);
    EXPECT_EQ(parser.arg1(), "bar");
}
