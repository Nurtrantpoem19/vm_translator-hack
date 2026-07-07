#include "translator/Code.hpp"
#include "translator/Parser.hpp"
#include <fstream>

#include <gtest/gtest.h>

class CodeTest : public ::testing::Test
{
  protected:
    std::filesystem::path outpath{"TestOutput.asm"};
    Code writer{outpath};

    void TearDown() override
    {
        if (std::filesystem::exists(outpath))
        {
            std::filesystem::remove(outpath);
        }
    }

    // Helper to abstract away the file reading boilerplate
    std::string getOutputFileContent()
    {
        std::ifstream inFile(outpath.string());
        if (!inFile.is_open())
            return "";

        std::string content, line;
        while (std::getline(inFile, line))
        {
            content += line + "\n";
        }
        return content;
    }
};

TEST_F(CodeTest, PushConstantSeven)
{
    // 1. Generate the assembly
    std::string seg = "constant";

    writer.updateFileName("TestOutput");
    writer.writePushPop(Parser::CommandType::C_Push, seg, 7);
    writer.close();
    // 2. Define the theoretical output
    std::string expectedAssembly = "@7\n"
                                   "D=A\n"
                                   "@R15\n"
                                   "M=D\n"
                                   "@R15\n"
                                   "D=M\n"
                                   "@SP\n"
                                   "A=M\n"
                                   "M=D\n"
                                   "@SP\n"
                                   "M=M+1\n";

    // 3. Compare actual output against theoretical output
    EXPECT_EQ(getOutputFileContent(), expectedAssembly);
}

TEST_F(CodeTest, WriteArithmeticAdd)
{
    // 1. Generate the assembly for an "add" operation
    writer.updateFileName("TestOutput");
    writer.writeArithmetic("add");

    // 2. Close the file so the contents flush to disk
    writer.close();
    std::string expectedAssembly = "@SP\n"
                                   "AM=M-1\n"
                                   "D=M\n"
                                   "@SP\n"
                                   "AM=M-1\n"
                                   "M=D+M\n"
                                   "@SP\n"
                                   "M=M+1\n";

    // 4. Compare actual output against theoretical output
    EXPECT_EQ(getOutputFileContent(), expectedAssembly);
}

TEST_F(CodeTest, writeLabelsUnderFunctions)
{
    std::string segment = "label bar";
    writer.updateFileName("TestOutput");
    writer.updateFunctionName("foo");
    writer.writeLabel("bar");

    writer.close();

    std::string expectedAssembly = "(TestOutput.foo$bar)\n";

    EXPECT_EQ(getOutputFileContent(), expectedAssembly);
}

TEST_F(CodeTest, writeGoTo_IfGOTO)
{
    writer.updateFileName("TestOutput");
    writer.updateFunctionName("foo");
    writer.writeGoTo("bar");

    writer.writeIf("bar");

    writer.close();
    std::string expectedAssembly = "@TestOutput.foo$bar\n"
                                   "0;JMP\n"
                                   "@SP\n"
                                   "AM=M-1\n"
                                   "D=M\n"
                                   "@TestOutput.foo$bar\n"
                                   "D;JNE\n";

    // Read the file and assert
    EXPECT_EQ(getOutputFileContent(), expectedAssembly);
}

TEST_F(CodeTest, writeFunctionTest)
{
    //
    // writer.writeFunction("foo", 2);
    // writer.close();
    //
    // std::cout << "\n--- Generated Assembly Output ---\n"
    //           << getOutputFileContent()
    //           << "---------------------------------\n";
}

TEST_F(CodeTest, writeCallTest)
{

    writer.updateFunctionName("foo");
    writer.writeCall(2);
    writer.close();

    std::cout << "\n--- Generated Assembly Output ---\n"
              << getOutputFileContent()
              << "---------------------------------\n";
}
TEST_F(CodeTest, writeSysCallTest)
{
    writer.updateFunctionName("Sys.init");
    writer.writeCall(0);
    writer.close();

    std::cout << "\n--- Generated Assembly Output ---\n"
              << getOutputFileContent()
              << "---------------------------------\n";
}

TEST_F(CodeTest, writeFullTest)
{
    writer.init();
    writer.updateFunctionName("foo");
    writer.writeCall(2);
    writer.writeReturn();
    writer.close();

    std::cout << "\n--- Generated Assembly Output ---\n"
              << getOutputFileContent()
              << "---------------------------------\n";
}

TEST_F(CodeTest, testSysInitFunction)
{

    writer.init();
    writer.updateFunctionName("Sys.init");
    writer.writeFunction(0);

    std::cout << "\n--- Generated Assembly Output ---\n"
              << getOutputFileContent()
              << "---------------------------------\n";

    writer.close();

    std::string expected = "";
}
