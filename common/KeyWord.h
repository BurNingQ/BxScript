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
 * @brief    关键字声明
 */

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