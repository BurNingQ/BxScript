/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025-12-15
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    符号定义
 */

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
