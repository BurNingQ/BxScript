//
// Created by BurNingLi on 2025/9/17.
//

#ifndef BXSCRIPT_KEYWORD_H
#define BXSCRIPT_KEYWORD_H
#include <string>
#include <unordered_set>


class KeyWord {
public:
    static bool isKeyword(const std::string &str) {
        return KeyWords.find(str) != KeyWords.end();
    }
private:
    static const std::unordered_set<std::string> KeyWords;
};

const inline std::unordered_set<std::string> KeyWord::KeyWords{
    "function","let","true","false","this","if","else",
    "return","null","for","break","continue","while",
    "import","as"
};

#endif //BXSCRIPT_KEYWORD_H