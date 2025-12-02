//
// Created by BurNingLi on 2025/9/17.
//

#ifndef BXSCRIPT_SYMBOLS_H
#define BXSCRIPT_SYMBOLS_H
#include <string>
#include <unordered_set>


class Symbols {
public:
    static bool isSymbols(const std::string &str) {
        return SymbolList.find(str) != SymbolList.end();
    }
private:
    static const std::unordered_set<std::string> SymbolList;
};

const inline std::unordered_set<std::string> Symbols::SymbolList{
    "{","}",
    "(",")",
    "[","]",
    ".",
    ",",
    ";",
    "+","-","*","/","%","=",
    "&","|","!","<",">",":"
};

#endif //BXSCRIPT_SYMBOLS_H
