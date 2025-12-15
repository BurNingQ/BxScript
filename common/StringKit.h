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
 * @brief    字符串工具
 */

#ifndef BXSCRIPT_STRINGKIT_H
#define BXSCRIPT_STRINGKIT_H
#include <string>


class StringKit {
public:
    static std::u32string Utf8ToU32(const std::string &s) {
        std::u32string result;
        result.reserve(s.length());
        size_t i = 0;
        while (i < s.length()) {
            const auto c = static_cast<unsigned char>(s[i]);
            char32_t code_point = 0;
            if (c < 0x80) {
                code_point = c;
                i += 1;
            } else if ((c & 0xE0) == 0xC0) {
                if (i + 1 >= s.length()) break;
                const auto c2 = static_cast<unsigned char>(s[i + 1]);
                code_point = ((c & 0x1F) << 6) | (c2 & 0x3F);
                i += 2;
            } else if ((c & 0xF0) == 0xE0) {
                if (i + 2 >= s.length()) break;
                const auto c2 = static_cast<unsigned char>(s[i + 1]);
                const auto c3 = static_cast<unsigned char>(s[i + 2]);
                code_point = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                i += 3;
            } else if ((c & 0xF8) == 0xF0) {
                if (i + 3 >= s.length()) break;
                const auto c2 = static_cast<unsigned char>(s[i + 1]);
                const auto c3 = static_cast<unsigned char>(s[i + 2]);
                const auto c4 = static_cast<unsigned char>(s[i + 3]);
                code_point = ((c & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
                i += 4;
            } else {
                i++;
                continue;
            }
            result.push_back(code_point);
        }
        return result;
    }

    static std::string Char32ToUtf8(char32_t wc) {
        std::string res;
        if (wc < 0x80) {
            res += static_cast<char>(wc);
        } else if (wc < 0x800) {
            res += static_cast<char>(0xC0 | (wc >> 6));
            res += static_cast<char>(0x80 | (wc & 0x3F));
        } else if (wc < 0x10000) {
            res += static_cast<char>(0xE0 | (wc >> 12));
            res += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
            res += static_cast<char>(0x80 | (wc & 0x3F));
        } else {
            res += static_cast<char>(0xF0 | (wc >> 18));
            res += static_cast<char>(0x80 | ((wc >> 12) & 0x3F));
            res += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
            res += static_cast<char>(0x80 | (wc & 0x3F));
        }
        return res;
    }

    static std::string U32ToUtf8(const std::u32string &u) {
        std::string res;
        res.reserve(u.length() * 3);
        for (const char32_t c: u) {
            res += Char32ToUtf8(c);
        }
        return res;
    }

    static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
        if (from.empty()) return str;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
};


#endif //BXSCRIPT_STRINGKIT_H
