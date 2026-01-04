/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/4
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    WinMM
 */
#ifndef BXSCRIPT_WINMM_H
#define BXSCRIPT_WINMM_H

#include <windows.h>
#include <mmsystem.h> // 必须包含
#include <string>

class WinMM {
public:
    // =========================================================================================
    // Group 1: 简单音效 (PlaySound)
    // @usage 适合播放短促的提示音 (WAV格式)。
    // =========================================================================================

    /**
     * 播放波形声音。
     * @param pszSound 文件路径、资源名或内存指针。
     * @param fdwSound 标志位：
     *        - SND_FILENAME: pszSound 是文件名 (最常用)
     *        - SND_ASYNC: 异步播放 (不阻塞主线程)
     *        - SND_LOOP: 循环播放 (必须配合 SND_ASYNC)
     *        - SND_PURGE: 停止当前播放
     */
    static BOOL W32_PlaySound(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound) {
        return PlaySoundW(pszSound, hmod, fdwSound);
    }


    // =========================================================================================
    // Group 2: MCI 命令字符串 (Media Control Interface)
    // @usage 万能接口。可以用字符串命令播放 MP3, 调整音量, 控制进度等。
    //        命令示例: "open \"C:\\music.mp3\" type mpegvideo alias MyMusic"
    //                 "play MyMusic"
    //                 "close MyMusic"
    // =========================================================================================

    /**
     * 发送 MCI 字符串命令。
     * @param lpstrCommand 命令字符串。
     * @param lpstrReturnString [输出] 接收返回信息的缓冲区 (可选，传 NULL)。
     * @param uReturnLength 缓冲区大小。
     * @return 0 表示成功，非 0 表示错误码。
     */
    static DWORD W32_mciSendString(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback) {
        return mciSendStringW(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
    }

    /**
     * 获取 MCI 错误描述。
     * @usage 当 mciSendString 返回非 0 时，用这个函数获取具体的错误文字。
     */
    static BOOL W32_mciGetErrorString(DWORD fdwError, LPWSTR lpszErrorText, UINT cchErrorText) {
        return mciGetErrorStringW(fdwError, lpszErrorText, cchErrorText);
    }
};

#endif //BXSCRIPT_WINMM_H