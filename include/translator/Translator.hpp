#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "Code.hpp"
#include "Parser.hpp"

class Translator
{
  protected:
    Parser reader;
    Code decode;
};
#endif // TRANSLATOR_HPP
