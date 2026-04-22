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
#ifndef BXSCRIPT_GUIRUNTIME_H
#define BXSCRIPT_GUIRUNTIME_H

#include "../evaluator/Value.h"
#include <vector>

class GuiRuntime {
public:
    static void RunWithoutMessageLoop(const std::vector<ValuePtr> &rootValues);
    /**
     * 启动 GUI 应用程序
     * @param rootValues 脚本中定义的窗口数据对象列表 (通常由 win.form 创建)
     */
    static void Run(const std::vector<ValuePtr> &rootValues);

    static void RunWebView(const std::shared_ptr<ObjectValue> &cfg);
private:
    /**
     * 混合消息循环 (Hybrid Message Loop)
     * 同时处理 Windows UI 消息和 BxScript 异步任务
     */
    static void MainLoop();
};

#endif //BXSCRIPT_GUIRUNTIME_H
