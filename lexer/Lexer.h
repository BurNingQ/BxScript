//
// Created by BurNingLi on 2025/9/17.
//

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
