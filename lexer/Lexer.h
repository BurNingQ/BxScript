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
 * @brief    Lexer定义
 */

#ifndef BXSCRIPT_LEXER_H
#define BXSCRIPT_LEXER_H
#include <sstream>
#include <string>
#include <vector>

#include "Token.h"

class Lexer {
public:

    Lexer() = delete;

    explicit Lexer(const std::string &sourceCode)  {
        SplitSourceCode(sourceCode);
        if (mSourceCode.empty()) {
            throw std::runtime_error("源码为空");
        }

        RowNum = 0;
        RowPos = 0;
        mRowBuffer = mSourceCode.at(0);
        EndOfFile = false;
    }

    void SplitSourceCode(const std::string &code) {
        std::stringstream ss(code);
        std::string line;
        while (std::getline(ss, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            mSourceCode.push_back(line);
        }
    }

    ~Lexer() = default;

    std::string NextChar();

    void RollBack() {
        RowPos--;
    }

    [[nodiscard]] bool IsEndOfFile() const {
        return EndOfFile;
    }

    Token NextToken();

private:
    std::vector<std::string> mSourceCode;
    std::string mRowBuffer;
    int RowPos = 0;
    int RowNum = 0;
    bool EndOfFile = false;
};

#endif //BXSCRIPT_LEXER_H
