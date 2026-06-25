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

    void writeArithmetic(const std::string &command);
    void writePushPop(Parser::CommandType command, std::string &segment,
                      int index);
    void writePush(std::string segment, int index);
    // write helper methods for writepushpop because it's gonnoa be too hefty if
    // i implement everyting inside writepushpop
    void fetch(SegmentType type, int index);
    void close();
};

#endif
