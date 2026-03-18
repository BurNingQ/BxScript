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
 * @brief    App
 */
#ifndef BXSCRIPT_APP_H
#define BXSCRIPT_APP_H

#include <cstdint>

class App {
    inline static uintptr_t g_gdiplusToken = 0;

public:
    /**
     * 初始化应用程序环境。
     */
    static void Init();

    /**
     * 获取当前应用程序实例句柄。
     */
    static void *GetInstance();

    /**
     * 运行主消息循环 (阻塞)。
     */
    static int Run();

    /**
     * 退出应用程序。
     */
    static void Exit(int exitCode = 0);

    static void (*OnGlobalHotkey)(int id);

    static bool PollEvents();

    static void WaitEvents(int timeoutMs);

private:
    static void Shutdown();

    static bool PreTranslateMessage(void *msg);
};

#endif // BXSCRIPT_APP_H
