#include <filesystem>
#include <fstream>

class Parser
{
  private:
    std::ifstream reader;
    std::string currentCommand;
    std::size_t firstSpace;
    std::size_t secondSpace;

  public:
    Parser(std::filesystem::path &input);
    bool advance();
    enum class CommandType
    {
        C_Arithmetic,
        C_Pop,
        C_Push,
        C_Label,
        C_Goto,
        C_Function,
        C_Return,
        C_Call
    };
    CommandType commandType();
    std::string arg1();
    int arg2();
};
