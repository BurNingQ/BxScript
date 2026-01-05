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
 * @brief    Keyboard
 */
#ifndef BXSCRIPT_KEYBOARD_H
#define BXSCRIPT_KEYBOARD_H

#include <string>
#include <map>
#include <cstdint>

typedef uint16_t Key;

enum : Key {
    KeyLButton = 0x01,
    KeyRButton = 0x02,
    KeyCancel = 0x03,
    KeyMButton = 0x04,
    KeyXButton1 = 0x05,
    KeyXButton2 = 0x06,
    KeyBack = 0x08,
    KeyTab = 0x09,
    KeyClear = 0x0C,
    KeyReturn = 0x0D,
    KeyShift = 0x10,
    KeyControl = 0x11,
    KeyAlt = 0x12,
    KeyMenu = 0x12,
    KeyPause = 0x13,
    KeyCapital = 0x14,
    KeyKana = 0x15,
    KeyHangul = 0x15,
    KeyJunja = 0x17,
    KeyFinal = 0x18,
    KeyHanja = 0x19,
    KeyKanji = 0x19,
    KeyEscape = 0x1B,
    KeyConvert = 0x1C,
    KeyNonconvert = 0x1D,
    KeyAccept = 0x1E,
    KeyModeChange = 0x1F,
    KeySpace = 0x20,
    KeyPrior = 0x21,
    KeyNext = 0x22,
    KeyEnd = 0x23,
    KeyHome = 0x24,
    KeyLeft = 0x25,
    KeyUp = 0x26,
    KeyRight = 0x27,
    KeyDown = 0x28,
    KeySelect = 0x29,
    KeyPrint = 0x2A,
    KeyExecute = 0x2B,
    KeySnapshot = 0x2C,
    KeyInsert = 0x2D,
    KeyDelete = 0x2E,
    KeyHelp = 0x2F,
    Key0 = 0x30,
    Key1 = 0x31,
    Key2 = 0x32,
    Key3 = 0x33,
    Key4 = 0x34,
    Key5 = 0x35,
    Key6 = 0x36,
    Key7 = 0x37,
    Key8 = 0x38,
    Key9 = 0x39,
    KeyA = 0x41,
    KeyB = 0x42,
    KeyC = 0x43,
    KeyD = 0x44,
    KeyE = 0x45,
    KeyF = 0x46,
    KeyG = 0x47,
    KeyH = 0x48,
    KeyI = 0x49,
    KeyJ = 0x4A,
    KeyK = 0x4B,
    KeyL = 0x4C,
    KeyM = 0x4D,
    KeyN = 0x4E,
    KeyO = 0x4F,
    KeyP = 0x50,
    KeyQ = 0x51,
    KeyR = 0x52,
    KeyS = 0x53,
    KeyT = 0x54,
    KeyU = 0x55,
    KeyV = 0x56,
    KeyW = 0x57,
    KeyX = 0x58,
    KeyY = 0x59,
    KeyZ = 0x5A,
    KeyLWIN = 0x5B,
    KeyRWIN = 0x5C,
    KeyApps = 0x5D,
    KeySleep = 0x5F,
    KeyNumpad0 = 0x60,
    KeyNumpad1 = 0x61,
    KeyNumpad2 = 0x62,
    KeyNumpad3 = 0x63,
    KeyNumpad4 = 0x64,
    KeyNumpad5 = 0x65,
    KeyNumpad6 = 0x66,
    KeyNumpad7 = 0x67,
    KeyNumpad8 = 0x68,
    KeyNumpad9 = 0x69,
    KeyMultiply = 0x6A,
    KeyAdd = 0x6B,
    KeySeparator = 0x6C,
    KeySubtract = 0x6D,
    KeyDecimal = 0x6E,
    KeyDivide = 0x6F,
    KeyF1 = 0x70,
    KeyF2 = 0x71,
    KeyF3 = 0x72,
    KeyF4 = 0x73,
    KeyF5 = 0x74,
    KeyF6 = 0x75,
    KeyF7 = 0x76,
    KeyF8 = 0x77,
    KeyF9 = 0x78,
    KeyF10 = 0x79,
    KeyF11 = 0x7A,
    KeyF12 = 0x7B,
    KeyF13 = 0x7C,
    KeyF14 = 0x7D,
    KeyF15 = 0x7E,
    KeyF16 = 0x7F,
    KeyF17 = 0x80,
    KeyF18 = 0x81,
    KeyF19 = 0x82,
    KeyF20 = 0x83,
    KeyF21 = 0x84,
    KeyF22 = 0x85,
    KeyF23 = 0x86,
    KeyF24 = 0x87,
    KeyNumlock = 0x90,
    KeyScroll = 0x91,
    KeyLShift = 0xA0,
    KeyRShift = 0xA1,
    KeyLControl = 0xA2,
    KeyRControl = 0xA3,
    KeyLAlt = 0xA4,
    KeyLMenu = 0xA4,
    KeyRAlt = 0xA5,
    KeyRMenu = 0xA5,
    KeyBrowserBack = 0xA6,
    KeyBrowserForward = 0xA7,
    KeyBrowserRefresh = 0xA8,
    KeyBrowserStop = 0xA9,
    KeyBrowserSearch = 0xAA,
    KeyBrowserFavorites = 0xAB,
    KeyBrowserHome = 0xAC,
    KeyVolumeMute = 0xAD,
    KeyVolumeDown = 0xAE,
    KeyVolumeUp = 0xAF,
    KeyMediaNextTrack = 0xB0,
    KeyMediaPrevTrack = 0xB1,
    KeyMediaStop = 0xB2,
    KeyMediaPlayPause = 0xB3,
    KeyLaunchMail = 0xB4,
    KeyLaunchMediaSelect = 0xB5,
    KeyLaunchApp1 = 0xB6,
    KeyLaunchApp2 = 0xB7,
    KeyOEM1 = 0xBA,
    KeyOEMPlus = 0xBB,
    KeyOEMComma = 0xBC,
    KeyOEMMinus = 0xBD,
    KeyOEMPeriod = 0xBE,
    KeyOEM2 = 0xBF,
    KeyOEM3 = 0xC0,
    KeyOEM4 = 0xDB,
    KeyOEM5 = 0xDC,
    KeyOEM6 = 0xDD,
    KeyOEM7 = 0xDE,
    KeyOEM8 = 0xDF,
    KeyOEM102 = 0xE2,
    KeyProcessKey = 0xE5,
    KeyPacket = 0xE7,
    KeyAttn = 0xF6,
    KeyCRSel = 0xF7,
    KeyEXSel = 0xF8,
    KeyErEOF = 0xF9,
    KeyPlay = 0xFA,
    KeyZoom = 0xFB,
    KeyNoName = 0xFC,
    KeyPA1 = 0xFD,
    KeyOEMClear = 0xFE
};

typedef uint8_t Modifiers;

const Modifiers ModShift = 1 << 0;
const Modifiers ModControl = 1 << 1;
const Modifiers ModAlt = 1 << 2;

struct Shortcut {
    Modifiers Modifiers;
    Key Key;

    bool operator<(const Shortcut &other) const {
        if (Modifiers != other.Modifiers) return Modifiers < other.Modifiers;
        return Key < other.Key;
    }
};

std::wstring KeyToString(Key k);

std::wstring ModifiersToString(Modifiers m);

std::wstring ShortcutToString(Shortcut s);

bool AltDown();

bool ControlDown();

bool ShiftDown();

Modifiers ModifiersDown();

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"

static std::map<Key, std::wstring> key2string = {
    {KeyLButton, L"LButton"}, {KeyRButton, L"RButton"}, {KeyCancel, L"Cancel"},
    {KeyMButton, L"MButton"}, {KeyXButton1, L"XButton1"}, {KeyXButton2, L"XButton2"},
    {KeyBack, L"Back"}, {KeyTab, L"Tab"}, {KeyClear, L"Clear"}, {KeyReturn, L"Return"},
    {KeyShift, L"Shift"}, {KeyControl, L"Control"}, {KeyAlt, L"Alt / Menu"},
    {KeyPause, L"Pause"}, {KeyCapital, L"Capital"}, {KeyKana, L"Kana / Hangul"},
    {KeyJunja, L"Junja"}, {KeyFinal, L"Final"}, {KeyHanja, L"Hanja / Kanji"},
    {KeyEscape, L"Escape"}, {KeyConvert, L"Convert"}, {KeyNonconvert, L"Nonconvert"},
    {KeyAccept, L"Accept"}, {KeyModeChange, L"ModeChange"}, {KeySpace, L"Space"},
    {KeyPrior, L"Prior"}, {KeyNext, L"Next"}, {KeyEnd, L"End"}, {KeyHome, L"Home"},
    {KeyLeft, L"Left"}, {KeyUp, L"Up"}, {KeyRight, L"Right"}, {KeyDown, L"Down"},
    {KeySelect, L"Select"}, {KeyPrint, L"Print"}, {KeyExecute, L"Execute"},
    {KeySnapshot, L"Snapshot"}, {KeyInsert, L"Insert"}, {KeyDelete, L"Delete"},
    {KeyHelp, L"Help"}, {Key0, L"0"}, {Key1, L"1"}, {Key2, L"2"}, {Key3, L"3"},
    {Key4, L"4"}, {Key5, L"5"}, {Key6, L"6"}, {Key7, L"7"}, {Key8, L"8"}, {Key9, L"9"},
    {KeyA, L"A"}, {KeyB, L"B"}, {KeyC, L"C"}, {KeyD, L"D"}, {KeyE, L"E"}, {KeyF, L"F"},
    {KeyG, L"G"}, {KeyH, L"H"}, {KeyI, L"I"}, {KeyJ, L"J"}, {KeyK, L"K"}, {KeyL, L"L"},
    {KeyM, L"M"}, {KeyN, L"N"}, {KeyO, L"O"}, {KeyP, L"P"}, {KeyQ, L"Q"}, {KeyR, L"R"},
    {KeyS, L"S"}, {KeyT, L"T"}, {KeyU, L"U"}, {KeyV, L"V"}, {KeyW, L"W"}, {KeyX, L"X"},
    {KeyY, L"Y"}, {KeyZ, L"Z"}, {KeyLWIN, L"LWIN"}, {KeyRWIN, L"RWIN"}, {KeyApps, L"Apps"},
    {KeySleep, L"Sleep"}, {KeyNumpad0, L"Numpad0"}, {KeyNumpad1, L"Numpad1"},
    {KeyNumpad2, L"Numpad2"}, {KeyNumpad3, L"Numpad3"}, {KeyNumpad4, L"Numpad4"},
    {KeyNumpad5, L"Numpad5"}, {KeyNumpad6, L"Numpad6"}, {KeyNumpad7, L"Numpad7"},
    {KeyNumpad8, L"Numpad8"}, {KeyNumpad9, L"Numpad9"}, {KeyMultiply, L"Multiply"},
    {KeyAdd, L"Add"}, {KeySeparator, L"Separator"}, {KeySubtract, L"Subtract"},
    {KeyDecimal, L"Decimal"}, {KeyDivide, L"Divide"}, {KeyF1, L"F1"}, {KeyF2, L"F2"},
    {KeyF3, L"F3"}, {KeyF4, L"F4"}, {KeyF5, L"F5"}, {KeyF6, L"F6"}, {KeyF7, L"F7"},
    {KeyF8, L"F8"}, {KeyF9, L"F9"}, {KeyF10, L"F10"}, {KeyF11, L"F11"}, {KeyF12, L"F12"},
    {KeyF13, L"F13"}, {KeyF14, L"F14"}, {KeyF15, L"F15"}, {KeyF16, L"F16"},
    {KeyF17, L"F17"}, {KeyF18, L"F18"}, {KeyF19, L"F19"}, {KeyF20, L"F20"},
    {KeyF21, L"F21"}, {KeyF22, L"F22"}, {KeyF23, L"F23"}, {KeyF24, L"F24"},
    {KeyNumlock, L"Numlock"}, {KeyScroll, L"Scroll"}, {KeyLShift, L"LShift"},
    {KeyRShift, L"RShift"}, {KeyLControl, L"LControl"}, {KeyRControl, L"RControl"},
    {KeyLMenu, L"LMenu"}, {KeyRMenu, L"RMenu"}, {KeyBrowserBack, L"BrowserBack"},
    {KeyBrowserForward, L"BrowserForward"}, {KeyBrowserRefresh, L"BrowserRefresh"},
    {KeyBrowserStop, L"BrowserStop"}, {KeyBrowserSearch, L"BrowserSearch"},
    {KeyBrowserFavorites, L"BrowserFavorites"}, {KeyBrowserHome, L"BrowserHome"},
    {KeyVolumeMute, L"VolumeMute"}, {KeyVolumeDown, L"VolumeDown"},
    {KeyVolumeUp, L"VolumeUp"}, {KeyMediaNextTrack, L"MediaNextTrack"},
    {KeyMediaPrevTrack, L"MediaPrevTrack"}, {KeyMediaStop, L"MediaStop"},
    {KeyMediaPlayPause, L"MediaPlayPause"}, {KeyLaunchMail, L"LaunchMail"},
    {KeyLaunchMediaSelect, L"LaunchMediaSelect"}, {KeyLaunchApp1, L"LaunchApp1"},
    {KeyLaunchApp2, L"LaunchApp2"}, {KeyOEM1, L"OEM1"}, {KeyOEMPlus, L"OEMPlus"},
    {KeyOEMComma, L"OEMComma"}, {KeyOEMMinus, L"OEMMinus"}, {KeyOEMPeriod, L"OEMPeriod"},
    {KeyOEM2, L"OEM2"}, {KeyOEM3, L"OEM3"}, {KeyOEM4, L"OEM4"}, {KeyOEM5, L"OEM5"},
    {KeyOEM6, L"OEM6"}, {KeyOEM7, L"OEM7"}, {KeyOEM8, L"OEM8"}, {KeyOEM102, L"OEM102"},
    {KeyProcessKey, L"ProcessKey"}, {KeyPacket, L"Packet"}, {KeyAttn, L"Attn"},
    {KeyCRSel, L"CRSel"}, {KeyEXSel, L"EXSel"}, {KeyErEOF, L"ErEOF"}, {KeyPlay, L"Play"},
    {KeyZoom, L"Zoom"}, {KeyNoName, L"NoName"}, {KeyPA1, L"PA1"}, {KeyOEMClear, L"OEMClear"}
};

static std::map<Modifiers, std::wstring> modifiers2string = {
    {ModShift, L"Shift"},
    {ModControl, L"Ctrl"},
    {ModControl | ModShift, L"Ctrl+Shift"},
    {ModAlt, L"Alt"},
    {ModAlt | ModShift, L"Alt+Shift"},
    {ModAlt | ModControl | ModShift, L"Alt+Ctrl+Shift"}
};

std::wstring KeyToString(Key k) { return key2string[k]; }

std::wstring ModifiersToString(Modifiers m) { return modifiers2string[m]; }

std::wstring ShortcutToString(Shortcut s) {
    std::wstring m = ModifiersToString(s.Modifiers);
    if (m.empty()) return KeyToString(s.Key);
    return m + L"+" + KeyToString(s.Key);
}

bool AltDown() {
    return (User32::W32_GetKeyState(KeyAlt) >> 15) != 0;
}

bool ControlDown() {
    return (User32::W32_GetKeyState(KeyControl) >> 15) != 0;
}

bool ShiftDown() {
    return (User32::W32_GetKeyState(KeyShift) >> 15) != 0;
}

Modifiers ModifiersDown() {
    Modifiers m = 0;
    if (ShiftDown()) m |= ModShift;
    if (ControlDown()) m |= ModControl;
    if (AltDown()) m |= ModAlt;
    return m;
}

#endif