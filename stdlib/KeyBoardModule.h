/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/30
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    KeyBoardModule
 */
#ifndef BXSCRIPT_KEYBOARDMODULE_H
#define BXSCRIPT_KEYBOARDMODULE_H
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "../evaluator/Value.h"
#include "../common/StringKit.h"
#include "../error/RuntimeError.h"

class KeyBoardModule {
#if defined(_WIN32)
    // 虚拟键码映射表
    inline static std::map<std::string, int> VK_MAP = {
        {"BACK", VK_BACK}, {"TAB", VK_TAB}, {"CLEAR", VK_CLEAR}, {"ENTER", VK_RETURN},
        {"SHIFT", VK_SHIFT}, {"CTRL", VK_CONTROL}, {"ALT", VK_MENU}, {"PAUSE", VK_PAUSE},
        {"CAPSLK", VK_CAPITAL}, {"ESC", VK_ESCAPE}, {"SPACE", VK_SPACE}, {"PGUP", VK_PRIOR},
        {"PGDN", VK_NEXT}, {"END", VK_END}, {"HOME", VK_HOME}, {"LEFT", VK_LEFT},
        {"UP", VK_UP}, {"RIGHT", VK_RIGHT}, {"DOWN", VK_DOWN}, {"SELECT", VK_SELECT},
        {"PRINT", VK_PRINT}, {"EXEC", VK_EXECUTE}, {"PRTSCR", VK_SNAPSHOT}, {"INS", VK_INSERT},
        {"DEL", VK_DELETE}, {"HELP", VK_HELP},
        {"LWIN", VK_LWIN}, {"RWIN", VK_RWIN}, {"APPS", VK_APPS},
        {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34},
        {"5", 0x35}, {"6", 0x36}, {"7", 0x37}, {"8", 0x38}, {"9", 0x39},
        {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45},
        {"F", 0x46}, {"G", 0x47}, {"H", 0x48}, {"I", 0x49}, {"J", 0x4A},
        {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E}, {"O", 0x4F},
        {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54},
        {"U", 0x55}, {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},
        {"NUM0", VK_NUMPAD0}, {"NUM1", VK_NUMPAD1}, {"NUM2", VK_NUMPAD2}, {"NUM3", VK_NUMPAD3},
        {"NUM4", VK_NUMPAD4}, {"NUM5", VK_NUMPAD5}, {"NUM6", VK_NUMPAD6}, {"NUM7", VK_NUMPAD7},
        {"NUM8", VK_NUMPAD8}, {"NUM9", VK_NUMPAD9},
        {"MULTIPLY", VK_MULTIPLY}, {"ADD", VK_ADD}, {"SEPARATOR", VK_SEPARATOR},
        {"SUBTRACT", VK_SUBTRACT}, {"DECIMAL", VK_DECIMAL}, {"DIVIDE", VK_DIVIDE},
        {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5},
        {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8}, {"F9", VK_F9}, {"F10", VK_F10},
        {"F11", VK_F11}, {"F12", VK_F12},
        {"NUMLK", VK_NUMLOCK}, {"SCRLK", VK_SCROLL},
        {"LSHIFT", VK_LSHIFT}, {"RSHIFT", VK_RSHIFT},
        {"LCTRL", VK_LCONTROL}, {"RCTRL", VK_RCONTROL},
        {"LALT", VK_LMENU}, {"RALT", VK_RMENU},
        {";", VK_OEM_1}, {"+", VK_OEM_PLUS}, {",", VK_OEM_COMMA},
        {"-", VK_OEM_MINUS}, {".", VK_OEM_PERIOD}, {"/", VK_OEM_2},
        {"~", VK_OEM_3}, {"[", VK_OEM_4}, {"\\", VK_OEM_5},
        {"]", VK_OEM_6}, {"'", VK_OEM_7}
    };

    // 默认延迟 (ms)
    static constexpr int DELAY_PRESS = 5;
    static constexpr int DELAY_DOWN = 5;
    static constexpr int DELAY_UP = 1;

    // 辅助：获取 VK Code
    static int GetKeyCode(const ValuePtr &v) {
        if (v->type == ValueType::NUMBER) {
            return static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
        }
        if (v->type == ValueType::STRING) {
            std::string keyName = std::static_pointer_cast<StringValue>(v)->Value;
            std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);
            if (VK_MAP.find(keyName) != VK_MAP.end()) {
                return VK_MAP[keyName];
            }
        }
        return 0;
    }

    // 辅助：执行按键动作 (Down/Up)
    static void DoKey(int vk, bool isUp) {
        if (vk == 0) return;
        DWORD flags = 0;
        if (isUp) flags |= KEYEVENTF_KEYUP;
        // 扩展键处理 (方向键等需要 Extended Key 标志)
        if ((vk >= 33 && vk <= 46) || (vk >= 91 && vk <= 93)) {
            flags |= KEYEVENTF_EXTENDEDKEY;
        }
        keybd_event(static_cast<BYTE>(vk), MapVirtualKey(vk, 0), flags, 0);
    }
#endif

    // 方法：按下
    static void InitDown(std::shared_ptr<ObjectValue> &o) {
        o->Set("down", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       for (const auto &arg: args) {
                           int vk = GetKeyCode(arg);
                           DoKey(vk, false); // down
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                       }
                       std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_DOWN));
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

    // 方法：弹起
    static void InitUp(std::shared_ptr<ObjectValue> &o) {
        o->Set("up", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       for (const auto &arg: args) {
                           int vk = GetKeyCode(arg);
                           DoKey(vk, true); // up
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                       }
                       std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_UP));
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

    // 方法：点击 (按下+弹起)
    static void InitPress(std::shared_ptr<ObjectValue> &o) {
        o->Set("press", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       for (const auto &arg: args) {
                           int vk = GetKeyCode(arg);
                           DoKey(vk, false);
                           std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_DOWN));
                           DoKey(vk, true);
                           std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_PRESS));
                       }
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

    // 方法：组合键 (combine)
    // 逻辑：依次按下所有键 -> 延时 -> 倒序弹起所有键
    static void InitCombine(std::shared_ptr<ObjectValue> &o) {
        o->Set("combine", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       std::vector<int> vks;
                       // 1. 依次按下
                       for (const auto &arg: args) {
                           int vk = GetKeyCode(arg);
                           if (vk != 0) {
                               vks.push_back(vk);
                               DoKey(vk, false);
                               std::this_thread::sleep_for(std::chrono::milliseconds(1));
                           }
                       }

                       std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_DOWN));

                       // 2. 倒序弹起
                       for (auto it = vks.rbegin(); it != vks.rend(); ++it) {
                           DoKey(*it, true);
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                       }
                       std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_PRESS));
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

    // 方法：发送字符串 (支持 Unicode)
    static void InitSendString(std::shared_ptr<ObjectValue> &o) {
        o->Set("sendString", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       if (args.empty()) return std::make_shared<NullValue>();

                       std::string text = args[0]->ToString();
                       int delayMs = 0;
                       if (args.size() > 1 && args[1]->type == ValueType::NUMBER) {
                           delayMs = static_cast<int>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                       }

                       std::wstring wstr = StringKit::U8ToU16(text);

                       for (wchar_t c: wstr) {
                           INPUT inputs[2] = {};
                           ZeroMemory(inputs, sizeof(inputs));

                           // Press
                           inputs[0].type = INPUT_KEYBOARD;
                           inputs[0].ki.wScan = c;
                           inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

                           // Release
                           inputs[1].type = INPUT_KEYBOARD;
                           inputs[1].ki.wScan = c;
                           inputs[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

                           SendInput(2, inputs, sizeof(INPUT));

                           if (delayMs > 0) {
                               std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
                           }
                       }
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

    // 方法：获取按键状态
    // getState(vk): 返回 toggle 状态 (如 CapsLock 灯是否亮)
    // getStateX(vk): 返回 物理按键是否被按下
    static void InitState(std::shared_ptr<ObjectValue> &o) {
        o->Set("getState", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       if (args.empty()) return std::make_shared<BoolValue>(false);
                       int vk = GetKeyCode(args[0]);
                       // 获取 Toggle 状态 (低位为 1 表示 Toggle On)
                       return std::make_shared<BoolValue>((GetKeyState(vk) & 0x0001) != 0);
#else
                       return std::make_shared<BoolValue>(false);
#endif
                   }));

        o->Set("isPressed", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       if (args.empty()) return std::make_shared<BoolValue>(false);
                       int vk = GetKeyCode(args[0]);
                       // 获取按下状态 (高位为 1 表示按下)
                       return std::make_shared<BoolValue>((GetAsyncKeyState(vk) & 0x8000) != 0);
#else
                       return std::make_shared<BoolValue>(false);
#endif
                   }));
    }

    // 方法：等待按键
    static void InitWait(std::shared_ptr<ObjectValue> &o) {
        o->Set("wait", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       if (args.empty()) return std::make_shared<NullValue>();
                       int vk = GetKeyCode(args[0]);
                       if (vk == 0) return std::make_shared<NullValue>();

                       // 1. 等待释放 (如果当前是按下的)
                       while (GetAsyncKeyState(vk) & 0x8000) {
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                       }

                       // 2. 等待按下
                       while (!(GetAsyncKeyState(vk) & 0x8000)) {
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                       }

                       // 3. 等待释放 (模拟完整点击过程的等待)
                       while (GetAsyncKeyState(vk) & 0x8000) {
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                       }
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

    // 方法：锁定输入
    static void InitBlock(std::shared_ptr<ObjectValue> &o) {
        o->Set("block", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
                       bool block = true;
                       if (!args.empty() && args[0]->type == ValueType::BOOL) {
                           block = std::static_pointer_cast<BoolValue>(args[0])->Value;
                       }
                       BlockInput(block ? TRUE : FALSE);
#endif
                       return std::make_shared<NullValue>();
                   }));
    }

public:
    static ValuePtr CreateKeyBoardModule() {
        auto m = std::make_shared<ObjectValue>();
        InitPress(m);
        InitDown(m);
        InitUp(m);
        InitCombine(m);
        InitSendString(m);
        InitState(m);
        InitWait(m);
        InitBlock(m);
        return m;
    }
};


#endif //BXSCRIPT_KEYBOARDMODULE_H
