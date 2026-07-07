#ifndef PARSER_HPP
#define PARSER_HPP

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>
class Parser
{
  private:
    std::ifstream reader;
    std::string currentCommand;
    std::size_t firstSpace;
    std::size_t secondSpace;
    std::vector<std::string> tokens;

  public:
    enum class CommandType
    {
        C_Arithmetic,
        C_Pop,
        C_Push,
        C_Label,
        C_Goto,
        C_Function,
        C_Return,
        C_If,
        C_Call
    };

    Parser(const std::filesystem::path &input);
    std::unordered_map<std::string, CommandType> lookup;
    bool advance();
    CommandType commandType();
    std::string arg1();
    int arg2();
};

#endif
