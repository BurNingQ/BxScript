/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Keyboard
 */
#include "Keyboard.h"

#include <map>
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

std::wstring KeyToString(const Key k) { return key2string[k]; }

std::wstring ModifiersToString(const Modifiers m) { return modifiers2string[m]; }

std::wstring ShortcutToString(const Shortcut s) {
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