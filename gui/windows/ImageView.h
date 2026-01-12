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
 * @brief    ImageView
 */
#ifndef BXSCRIPT_IMAGEVIEW_H
#define BXSCRIPT_IMAGEVIEW_H

#include "ControlBase.h"
#include "Bitmap.h"
#include "Utils.h"

class ImageView : public ControlBase {

    Bitmap *m_bmp = nullptr; // bmp *Bitmap

public:
    ImageView() = default;

    ~ImageView() override = default;

    // Static Factory Method
    static ImageView *New(Controller *parent);

    // Image loading methods
    bool DrawImageFile(const std::wstring &filepath);

    bool DrawImageUrl(const std::wstring &url);

    void DrawImage(Bitmap *bmp);

    // Override WndProc from ControlBase
    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif // BXSCRIPT_IMAGEVIEW_H
