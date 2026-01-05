/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/5
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Color
 */
#ifndef BXSCRIPT_COLOR_H
#define BXSCRIPT_COLOR_H

#include <cstdint>

class Color {
    uint32_t m_value;

public:
    // 默认构造 (黑色)
    Color() : m_value(0) {
    }

    // 从原始值构造
    explicit Color(uint32_t value) : m_value(value) {
    }

    // ======================== 工厂方法 ========================

    /**
     * 从 RGB 分量创建颜色。
     */
    static Color RGB(uint8_t r, uint8_t g, uint8_t b) {
        return Color(static_cast<uint32_t>(r) |
                     (static_cast<uint32_t>(g) << 8) |
                     (static_cast<uint32_t>(b) << 16));
    }

    /**
     * 从十六进制创建 (如 0xFFFFFF)
     */
    static Color FromHex(uint32_t hex) {
        // 通常 Hex 是 RRGGBB，需要转为 BBGGRR
        uint8_t r = (hex >> 16) & 0xFF;
        uint8_t g = (hex >> 8) & 0xFF;
        uint8_t b = hex & 0xFF;
        return RGB(r, g, b);
    }

    // ======================== 属性获取 ========================

    uint8_t R() const { return static_cast<uint8_t>(m_value & 0xFF); }
    uint8_t G() const { return static_cast<uint8_t>((m_value >> 8) & 0xFF); }
    uint8_t B() const { return static_cast<uint8_t>((m_value >> 16) & 0xFF); }

    /**
     * 获取原始值 (兼容 COLORREF)
     */
    uint32_t Value() const { return m_value; }
};

#endif // BXSCRIPT_COLOR_H
