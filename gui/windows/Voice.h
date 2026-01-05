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
 * @brief    Voice
 */
#ifndef BXSCRIPT_VOICE_H
#define BXSCRIPT_VOICE_H

#include <string>

void OpenVoice(const std::wstring &voice);

void PlayVoice();

void StopVoice();

void CloseVoice();

#endif // BXSCRIPT_VOICE_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/WinMM.h"

namespace {
    void sendMCICommand(const std::wstring &command) {
        wchar_t buf[256];
        WinMM::W32_mciSendString(command.c_str(), buf, 256, nullptr);
    }
}

void OpenVoice(const std::wstring &voice) {
    std::wstring cmd = L"open \"" + voice + L"\" type mpegvideo alias BxScriptAudio";
    sendMCICommand(cmd);
}

void PlayVoice() {
    sendMCICommand(L"play BxScriptAudio");
}

void StopVoice() {
    sendMCICommand(L"stop BxScriptAudio");
}

void CloseVoice() {
    sendMCICommand(L"close BxScriptAudio");
}

#endif // BXSCRIPT_IMPLEMENTATION
