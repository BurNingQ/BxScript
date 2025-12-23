/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/23
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    ColorKit
 */
#ifndef BXSCRIPT_COLORKIT_H
#define BXSCRIPT_COLORKIT_H
#include <cstring>
#include <cstdlib>
#include <cstdint>


class ColorKit {
public:
    struct ScriptColor {
        uint8_t r, g, b, a;
    };

    static ScriptColor HexToRgba(const char *hex) {
        ScriptColor col = {0, 0, 0, 255};
        if (!hex || *hex == '\0') return col;
        const char *p = hex;
        if (*p == '#') p++;
        else if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;
        const size_t len = strlen(p);
        char *end_ptr;
        const unsigned long long value = strtoull(p, &end_ptr, 16);
        if (len >= 8) {
            col.r = static_cast<uint8_t>((value >> 24) & 0xFF);
            col.g = static_cast<uint8_t>((value >> 16) & 0xFF);
            col.b = static_cast<uint8_t>((value >> 8) & 0xFF);
            col.a = static_cast<uint8_t>((value) & 0xFF);
        } else if (len >= 6) {
            col.r = static_cast<uint8_t>((value >> 16) & 0xFF);
            col.g = static_cast<uint8_t>((value >> 8) & 0xFF);
            col.b = static_cast<uint8_t>((value) & 0xFF);
            col.a = 255;
        }
        return col;
    }
};


#endif //BXSCRIPT_COLORKIT_H
