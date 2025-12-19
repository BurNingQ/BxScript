/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/19
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    FontKit
 */
#ifndef BXSCRIPT_FONTKIT_H
#define BXSCRIPT_FONTKIT_H

#include <cstddef>

extern "C" {
    extern const unsigned char bx_font_start[];
    extern const unsigned char bx_font_end[];
}

class FontKit {
public:
    struct FontData {
        const void* data;
        size_t size;
    };

    static FontData GetEmbeddedFont() {
        return {
            bx_font_start,
            static_cast<size_t>(bx_font_end - bx_font_start)
        };
    }
};

#endif //BXSCRIPT_FONTKIT_H
