#include "translator/Code.hpp"
#include <filesystem>

int Code::labelCount = 0;
int Code::staticAddress = 16;

void Code::popToD()
{
    output << "@SP\n";
    output << "AM=M-1\n";
    output << "D=M\n";
}
void Code::popToM()
{
    output << "@SP\n";
    output << "AM=M-1\n";
}
void Code::pushTrue()
{
    output << "@SP\n";
    output << "A=M\n";
    output << "M=-1\n";
    output << "@SP\n";
    output << "M=M+1\n";
}
void Code::pushFalse()
{
    output << "@SP\n";
    output << "A=M\n";
    output << "M=0\n";
    output << "@SP\n";
    output << "M=M+1\n";
}

void Code::writeBinOp(const std::string &op)
{
    popToD();
    popToM();

    if (op == "+" || (op == "&") || (op == "|"))
    {
        output << "M=D" << op << "M\n";
    }
    else
    {
        output << "M=M" << op << "D\n";
    }
    output << "@SP\n";
    output << "M=M+1\n";
}

void Code::writeUnOp(const std::string &op)
{
    popToM();
    output << "M=" << op << "M\n";
    output << "@SP\n";
    output << "M=M+1\n";
}
void Code::writeCompLabel(const std::string &jump)
{

    ++labelCount;
    popToD();
    popToM();

    output << "D=M-D\n";
    output << "@TRUE" << labelCount << "\n";
    output << "D;" << jump << std::endl;

    pushFalse();
    output << "@END_COMP" << labelCount << "\n";
    output << "0;JMP\n";

    output << "(TRUE" << labelCount << ")\n";
    pushTrue();

    output << "(END_COMP" << labelCount << ")\n";
}
void Code::writeLabel(const std::string &label)
{
    output << "(" << currentFileName << "." << currentFunction << "$" << label
           << ")\n";
}

void Code::writeGoTo(const std::string &label)
{
    output << "@" << currentFileName << "." << currentFunction << "$" << label
           << "\n";
    output << "0;JMP\n";
}

void Code::writeIf(const std::string &label)
{
    popToD();
    output << "@" << currentFileName << "." << currentFunction << "$" << label
           << "\n";
    output << "D;JNE\n";
}

void Code::writeFunction(int nVariables)
{
    output << "(" << currentFileName << "." << currentFunction << ")\n";

    for (int i = 0; i < nVariables; i++)
    {
        writePushPop(Parser::CommandType::C_Push, "constant", 0);
    }
}

void Code::writeCall(const int &nArgs)
{
    std::string labelName;
    std::string jumpTarget;

    // 1. Determine the scopes
    if (currentFunction.find('.') != std::string::npos)
    {
        // If the function name already has a dot (like "Sys.init" or
        // "Math.multiply")
        jumpTarget = currentFunction;
        labelName = currentFunction + "$ret." + std::to_string(ret_add++);
    }
    else
    {
        // Standard call within the current file (like calling local function
        // "foo")
        jumpTarget = currentFileName + "." + currentFunction;
        labelName = currentFileName + "." + currentFunction + "$ret." +
                    std::to_string(ret_add++);
    }
    //---------save return address first
    output << "@" << labelName << "\n"
           << "D=A\n"
           << "@SP\n"
           << "A=M\n"
           << "M=D\n"
           << "@SP\n"
           << "M=M+1\n";

    output << "@LCL\n"
           << "D=M\n"
           << "@SP\n"
           << "A=M\n"
           << "M=D\n"
           << "@SP\n"
           << "M=M+1\n"
           //------------- now for arg
           << "@ARG\n"
           << "D=M\n"
           << "@SP\n"
           << "A=M\n"
           << "M=D\n"
           << "@SP\n"
           << "M=M+1\n"
           //------------- now for this
           << "@THIS\n"
           << "D=M\n"
           << "@SP\n"
           << "A=M\n"
           << "M=D\n"
           << "@SP\n"
           << "M=M+1\n"
           //------------- and that
           << "@THAT\n"
           << "D=M\n"
           << "@SP\n"
           << "A=M\n"
           << "M=D\n"
           << "@SP\n"
           << "M=M+1\n"
           //--------------- now go back to before stack frame
           << "@SP\n"
           << "D=M\n"
           << "@5\n"
           << "D=D-A\n"
           << "@" << nArgs << "\n"
           << "D=D-M\n"
           << "@ARG\n"
           << "M=D\n"
           << "@SP\n"
           << "D=M\n"
           << "@LCL\n"
           << "M=D\n"
           << "@" << jumpTarget << "\n"
           << "0;JMP\n";

    output << "(" << labelName << ")\n";
}

void Code::writeReturn()
{

    output << "@LCL\n"
           << "D=M\n"
           << "@R13\n"
           << "M=D\n"

           << "@5\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@R14\n"
           << "M=D\n"

           << "@SP\n"
           << "A=M-1\n"
           << "D=M\n"
           << "@ARG\n"
           << "A=M\n"
           << "M=D\n"
           << "@ARG\n"
           << "D=M+1\n"
           << "@SP\n"
           << "M=D\n"

           << "@R13\n"
           << "AM=M-1\n"
           << "D=M\n"
           << "@THAT\n"
           << "M=D\n"
           << "@R13\n"
           << "AM=M-1\n"
           << "D=M\n"
           << "@THIS\n"
           << "M=D\n"
           << "@R13\n"
           << "AM=M-1\n"
           << "D=M\n"
           << "@ARG\n"
           << "M=D\n"
           << "@R13\n"
           << "AM=M-1\n"
           << "D=M\n"
           << "@LCL\n"
           << "M=D\n"
           << "@R14\n"
           << "A=M\n"
           << "0;JMP\n";
}

Code::Code(const std::filesystem::path &outpath)
    : output(outpath), currentFileName(""), ret_add(0),
      table{{"local", "LCL"}, {"argument", "ARG"}, {"this", "THIS"},
            {"that", "THAT"}, {"pointer", ""},     {"constant", ""},
            {"temp", ""},     {"static", ""}},
      opTable{{"add", OpType::Add}, {"sub", OpType::Sub}, {"neg", OpType::Neg},
              {"eq", OpType::Eq},   {"gt", OpType::Gt},   {"lt", OpType::Lt},
              {"and", OpType::And}, {"or", OpType::Or},   {"not", OpType::Not}},
      segmentTable{
          {"constant", SegmentType::Constant}, {"local", SegmentType::Local},
          {"argument", SegmentType::Argument}, {"this", SegmentType::This},
          {"that", SegmentType::That},         {"temp", SegmentType::Temp},
          {"pointer", SegmentType::Pointer},   {"static", SegmentType::Static}}
{
}
void Code::init()
{
    output << "@256\nD=A\n@SP\nM=D\n";
    currentFunction = "Sys.init";
    writeCall(0);
}

void Code::writeArithmetic(const std::string &command)
{
    OpType op = opTable.at(command);

    switch (op)
    {
    // -------------------
    // binary operations
    // -------------------
    case OpType::Add:
        writeBinOp("+");
        break;

    case OpType::Sub:
        writeBinOp("-");
        break;

    case OpType::And:
        writeBinOp("&");
        break;

    case OpType::Or:
        writeBinOp("|");
        break;

    // -------------------
    // unary operations
    // -------------------
    case OpType::Neg:
        writeUnOp("-");
        break;

    case OpType::Not:
        writeUnOp("!");
        break;

    // -------------------
    // comparisons
    // -------------------
    case OpType::Eq:
        writeCompLabel("JEQ");
        break;

    case OpType::Gt:
        writeCompLabel("JGT");
        break;

    case OpType::Lt:
        writeCompLabel("JLT");
        break;
    }
}

void Code::writePushPop(Parser::CommandType command, const std::string &segment,
                        int index)
{

    std::string seg = table.at(segment);
    SegmentType type = segmentTable.at(segment);
    // going to do this in two phases, first is fetch cycle. getting
    // the value from the target address. and then manipulate data accordingly
    switch (type)
    {
    case SegmentType::Local:
    case SegmentType::Argument:
    case SegmentType::This:
    case SegmentType::That:
    {
        output << "@" << seg << "\n";
        output << "D=M\n";
        output << "@" << index << "\n";
        output << "D=D+A\n";
        break;
    }
    case SegmentType::Constant:
    {
        output << "@" << index << "\n";
        output << "D=A\n";

        break;
    }

    case SegmentType::Pointer:
    {
        if (index == 1)
            output << "@THAT\n";
        else if (index == 0)
        {
            output << "@THIS\n";
        }
        output << "D=A\n";
        break;
    }

    case SegmentType::Static:
    {
        output << "@" << currentFileName << "." << index << "\n";
        output << "D=A\n";

        break;
    }

    case SegmentType::Temp:
    {
        // index + 5 because temp lives in RAM[5 to 12]
        output << "@" << index + 5 << "\n";
        output << "D=A\n";

        break;
    }
    }
    output << "@R15\nM=D\n";

    if (command == Parser::CommandType::C_Push)
    {
        output << "@R15\n";
        if (type != SegmentType::Constant)
        {
            output << "A=M\n";
        }
        output << "D=M\n";

        output << "@SP\n";
        output << "A=M\n";
        output << "M=D\n";

        output << "@SP\n";
        output << "M=M+1\n";
    }

    if (command == Parser::CommandType::C_Pop)
    {
        // 1. Decrement SP and fetch the top value from the stack into D
        output << "@SP\n";
        output << "AM=M-1\n";
        output << "D=M\n";

        // 2. Write that value directly to the destination address waiting in
        // R15
        output << "@R15\n";
        output << "A=M\n";
        output << "M=D\n";
    }
}

void Code::updateFileName(const std::string &fileName)
{
    currentFileName = fileName;
    return;
}

void Code::updateFunctionName(const std::string &functionName)
{
    currentFunction = functionName;
    return;
}
void Code::close()
{
    if (output.is_open())
    {
        output.close();
    }
}
