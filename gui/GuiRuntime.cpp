/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/22
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    GuiRuntime
 */
#include "GuiRuntime.h"
#include "GuiRenderer.h"
#include "WebViewRenderer.h"
#include "windows/App.h"

void GuiRuntime::RunWithoutMessageLoop(const std::vector<ValuePtr> &rootValues) {
    if (rootValues.empty()) return;
    static bool isAppInitialized = false;
    if (!isAppInitialized) {
        App::Init();
        isAppInitialized = true;
    }
    for (const auto &val: rootValues) {
        GuiRenderer::Render(val);
    }
}

void GuiRuntime::Run(const std::vector<ValuePtr> &rootValues) {
    RunWithoutMessageLoop(rootValues);
    MainLoop();
}

void GuiRuntime::RunWebView(const std::shared_ptr<ObjectValue> &cfg) {
    App::Init();
    WebViewRenderer::Run(cfg);
    MainLoop();
}

void GuiRuntime::MainLoop() {
    while (true) {
        if (!App::PollEvents()) {
            break;
        }
        bool hasScriptWork = EventLoop::Dispatch(10);
        if (!hasScriptWork) {
            App::WaitEvents(15);
        }
    }
}
