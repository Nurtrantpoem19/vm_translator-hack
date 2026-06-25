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

    writer.writePushPop(Parser::CommandType::C_Push, seg, 7);
    writer.close();
    // 2. Define the theoretical output
    std::string expectedAssembly = "@7\n"
                                   "D=A\n"
                                   "@15\n"
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
    writer.writeArithmetic("add");

    // 2. Close the file so the contents flush to disk
    writer.close();
    std::string expectedAssembly = "@SP\n"
                                   "AM=M-1\n"
                                   "D=M\n"
                                   "@SP\n"
                                   "AM=M-1\n"
                                   "M=M+D\n"
                                   "@SP\n"
                                   "M=M+1\n";

    // 4. Compare actual output against theoretical output
    EXPECT_EQ(getOutputFileContent(), expectedAssembly);
}
