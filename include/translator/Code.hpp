#ifndef CODE_HPP
#define CODE_HPP

#include "Parser.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

class Code
{
  private:
    std::string currentFileName;
    std::string currentFunction;
    std::string currentCallee;
    int ret_add;
    enum class OpType
    {
        Add,
        Sub,
        Neg,
        Eq,
        Gt,
        Lt,
        And,
        Or,
        Not
    };
    enum class SegmentType
    {
        Constant,
        Local,
        Argument,
        This,
        That,
        Temp,
        Pointer,
        Static
    };
    static int labelCount;
    static int staticAddress;
    std::ofstream output;
    std::unordered_map<std::string, std::string> table;
    std::unordered_map<std::string, OpType> opTable;
    std::unordered_map<std::string, SegmentType> segmentTable;
    void popToD();
    void popToM();
    void pushTrue();
    void pushFalse();
    void writeBinOp(const std::string &op);
    void writeUnOp(const std::string &op);
    void writeCompLabel(const std::string &jump);

  public:
    Code(const std::filesystem::path &outpath);
    void writeGoTo(const std::string &label);
    void writeLabel(const std::string &label);
    void writeIf(const std::string &label);
    void writeFunction(int nVariables);
    void writeCall(const int &nArgs);
    void writeReturn();

    void writeArithmetic(const std::string &command);
    void writePushPop(Parser::CommandType command, const std::string &segment,
                      int index);
    void updateFileName(const std::string &fileName);
    void updateFunctionName(const std::string &functionName);
    void updateCalleeName(const std::string &calleName);
    void init();
    void close();
};

#endif
