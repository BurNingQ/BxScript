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

void GuiRuntime::RunModal(const ValuePtr &formValue) {
    const auto idVal = formValue->Get("id");
    if (!idVal || idVal->type != ValueType::STRING) return;

    auto *ctrl = GuiRenderer::FindControl(idVal->ToString());
    if (ctrl == nullptr) return;

    void *hwnd = ctrl->Handle();
    void *owner = ctrl->Parent() ? ctrl->Parent()->Handle() : nullptr;

    if (owner) App::SetEnabled(owner, false); // 这就是"模态"

    // 与 MainLoop 同构的嵌套消息循环，只是退出条件换成了"这个窗口还在吗"。
    // 注意：PollEvents() 会把 WM_QUIT 消费掉，所以在这里重新投递一次，
    // 让外层 MainLoop 也能收到退出信号，否则整个应用会卡住。
    while (App::IsAlive(hwnd)) {
        if (!App::PollEvents()) {
            App::Exit(0);
            break;
        }
        if (!EventLoop::Dispatch(10)) {
            App::WaitEvents(15);
        }
    }

    if (owner && App::IsAlive(owner)) {
        App::SetEnabled(owner, true);
        App::Activate(owner);
    }
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
