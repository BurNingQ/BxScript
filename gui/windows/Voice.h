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
