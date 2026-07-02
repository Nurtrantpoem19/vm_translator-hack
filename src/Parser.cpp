#include "translator/Parser.hpp"
#include <string>
#include <unordered_map>

Parser::Parser(std::filesystem::path &input)
    : reader(input), currentCommand(""), currentFunction(""),
      lookup{{"push", CommandType::C_Push},
             {"pop", CommandType::C_Pop},
             {"label", CommandType::C_Label},
             {"goto", CommandType::C_Goto},
             {"function", CommandType::C_Function},
             {"return", CommandType::C_Return},
             {"call", CommandType::C_Call},
             {"if-goto", CommandType::C_If}}

{
}

bool Parser::advance()
{
    while (std::getline(reader, currentCommand))
    {

        std::size_t slash = currentCommand.find("//");
        if (slash != std::string::npos)
        {
            currentCommand = currentCommand.substr(0, slash);
        }

        if (!currentCommand.empty())
        {
            if (currentCommand.find_first_not_of(" \t\r\n") ==
                std::string::npos)
            {
                continue;
            }
            else
            {

                firstSpace = currentCommand.find(' ');

                secondSpace = currentCommand.find(' ', firstSpace + 1);

                return true;
            }
        }
    }
    return false;
}
// should only be called if it's not C_Return
std::string Parser::arg1()
{
    if (commandType() == CommandType::C_Arithmetic)
    {
        return currentCommand;
    }
    if (commandType() == CommandType::C_Function)
    {
        currentFunction =
            currentCommand.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        return currentFunction;
    }
    return currentCommand.substr(firstSpace + 1, secondSpace - firstSpace - 1);
}

// only for C_Pop, Push, Function and Call
int Parser::arg2() { return std::stoi(currentCommand.substr(secondSpace + 1)); }

Parser::CommandType Parser::commandType()
{
    auto it = lookup.find(currentCommand.substr(0, firstSpace));
    if (it != lookup.end())
    {

        return it->second;
    }
    return CommandType::C_Arithmetic;
}
