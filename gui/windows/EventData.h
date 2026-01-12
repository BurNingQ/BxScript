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
 * @brief    EventData
 */
#ifndef BXSCRIPT_EVENT_DATA_H
#define BXSCRIPT_EVENT_DATA_H

#include <string>
#include <vector>
#include "Canvas.h"
#include "ListInterfaces.h"

// 原始消息数据
struct RawMsgData {
    void *Hwnd;
    unsigned int Msg;
    uintptr_t WParam;
    uintptr_t LParam;
};

// 鼠标事件数据
struct MouseEventData {
    // 0:左, 1:中, 2:右
    int X, Y;
    int Button;
    // 滚轮增量
    int Wheel;
};

// 拖放文件数据
struct DropFilesEventData {
    int X, Y;
    std::vector<std::wstring> Files;
};

// 绘图事件数据
struct PaintEventData {
    Canvas *pCanvas;
};

// 按键事件数据 (KeyUp/KeyDown)
struct KeyEventData {
    int VKey;
    int ScanCode;
};

struct KeyUpEventData {
    int VKey;
    int Code;
};

// 尺寸改变数据
struct SizeEventData {
    unsigned int Type; // SIZE_RESTORED, SIZE_MAXIMIZED 等
    int Width;
    int Height;
};

struct LabelEditEventData {
    ListItem *Item;
    std::wstring Text;
};

#endif //BXSCRIPT_EVENT_DATA_H
