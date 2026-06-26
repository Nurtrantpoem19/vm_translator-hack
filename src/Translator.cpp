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

    std::filesystem::path outPath = inPath;
    outPath.replace_extension(".hack");
    std::cout << "Translating: " << inPath.filename() << " -> "
              << outPath.filename() << "\n";

    Parser reader(inPath);
    Code writer(outPath);
    int lineCount = 0;
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
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "\n[!] Lookup Error on line " << lineCount << "!\n"
                      << "    Command Type: " << static_cast<int>(type) << "\n"
                      << "    arg1:        '" << reader.arg1() << "'\n"
                      << "    arg2:        '" << reader.arg2() << "'\n"
                      << "    Error Details: " << e.what() << "\n\n";
            return 1; // Exit early so you can see the trace
        }
    }
}
