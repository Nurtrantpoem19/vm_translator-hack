#include "translator/Code.hpp"
#include "translator/Parser.hpp"
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: Missing input file argument.\n"
                  << "Usage: " << argv[0] << " <path-to-vm-file>\n";
        return 1;
    }

    std::filesystem::path inPath = {argv[1]};

    if (!std::filesystem::exists(inPath))
    {
        std::cerr << "Error: The system cannot find the path specified:\n"
                  << "  Given path: " << inPath << "\n"
                  << "  Absolute:   " << std::filesystem::absolute(inPath)
                  << "\n";
        return 1;
    }
    if (!std::filesystem::is_directory(inPath))
    {
        std::cerr << "Error, need directory as input\n";
        return 1;
    }

    std::filesystem::path dirName = inPath.filename();
    if (dirName.empty())
    {
        dirName = inPath.parent_path().filename();
    }

    std::filesystem::path outPath = inPath / dirName;
    outPath.replace_extension(".hack");

    std::cout << "Translating: " << dirName.string() << " -> "
              << outPath.filename().string() << "\n";

    Code writer(outPath);

    bool initialize = true;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-no-init" || arg == "--no-init")
        {
            initialize = false;
        }
    }
    if (initialize)
    {
        writer.init();
    }

    int lineCount = 0;
    for (auto &it : std::filesystem::directory_iterator(inPath))
    {
        if (it.path().extension() != ".vm")
        {
            continue;
        }
        Parser reader(it.path());
        writer.updateFileName(it.path().filename().string());
        while (reader.advance())
        {
            ++lineCount;
            Parser::CommandType type = reader.commandType();

            try
            {
                if (type == Parser::CommandType::C_Arithmetic)
                {
                    writer.writeArithmetic(reader.arg1());
                }
                else if (type == Parser::CommandType::C_Pop ||
                         type == Parser::CommandType::C_Push)
                {
                    writer.writePushPop(type, reader.arg1(), reader.arg2());
                }
                else if (type == Parser::CommandType::C_Function)
                {
                    writer.updateFunctionName(reader.arg1());
                    writer.writeFunction(reader.arg2());
                }
                else if (type == Parser::CommandType::C_Call)
                {
                    writer.updateCalleeName(reader.arg1());
                    writer.writeCall(reader.arg2());
                }
                else if (type == Parser::CommandType::C_Goto)
                {
                    writer.writeGoTo(reader.arg1());
                }
                else if (type == Parser::CommandType::C_If)
                {
                    writer.writeIf(reader.arg1());
                }
                else if (type == Parser::CommandType::C_Label)
                {
                    writer.writeLabel(reader.arg1());
                }
                else if (type == Parser::CommandType::C_Return)
                {
                    writer.writeReturn();
                }
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "\n[!] Lookup Error on line " << lineCount << "!\n"
                          << "    Command Type: " << static_cast<int>(type)
                          << "\n"
                          << "    arg1:        '" << reader.arg1() << "'\n"
                          << "    arg2:        '" << reader.arg2() << "'\n"
                          << "    Error Details: " << e.what() << "\n\n";
                return 1; // Exit early so you can see the trace
            }
        }
    }
}
