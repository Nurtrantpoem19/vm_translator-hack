#include "translator/Parser.hpp"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

Parser::Parser(const std::filesystem::path &input)
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
        tokens.clear();

        std::size_t slash = currentCommand.find("//");
        if (slash != std::string::npos)
        {
            currentCommand = currentCommand.substr(0, slash);
        }

        std::istringstream line(currentCommand);
        std::string token;

        while (tokens.size() < 3 && line >> token)
        {
            tokens.push_back(token);
        }
        if (tokens.empty())
        {

            continue;
        }
        return true;
    }
    return false;
}

std::string Parser::arg1()
{
    if (commandType() == CommandType::C_Arithmetic)
    {
        return currentCommand;
    }
    return tokens[1];
}

int Parser::arg2() { return std::stoi(tokens[2]); }

Parser::CommandType Parser::commandType()
{
    auto it = lookup.find(tokens[0]);
    if (it != lookup.end())
    {

        return it->second;
    }
    return CommandType::C_Arithmetic;
}

std::string Parser::getFunctionName() { return currentFunction; }
