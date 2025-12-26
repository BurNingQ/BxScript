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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL2_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_glfw_gl2.h"
#include <GLFW/glfw3.h>

#include "GuiRuntime.h"
#include <map>

#include "../evaluator/EventLoop.h"
#include "../stdlib/GuiModule.h"
#include "GuiRenderer.h"
#include "common/FontKit.h"
// 标题栏高度
constexpr float TITLE_BAR_HEIGHT = 40.0f;

// 拖动参数容器
struct {
    int is_dragging;
    double anchor_x, anchor_y;
} window_drag = {0, 0.0, 0.0};

// 关闭状态机
bool isClosing = false;
double lastTime = glfwGetTime();
float MAIN_FORM_OPACITY = 1.0f;
constexpr float ANIMATION_DURATION = 0.2f;
constexpr float FADE_SPEED = 1.0f / ANIMATION_DURATION;
// 字体缓存
std::map<int, nk_font *> FontCache;
// 是否显示alert
bool showAlert = false;
// 是否显示confirm
bool showConfirm = false;
// 主窗体
std::shared_ptr<ObjectValue> mainForm = nullptr;

void BeginFrame() {
    glfwPollEvents();
    nk_glfw3_new_frame();
}

/**
 *  初始化窗体位置
 * @param isCenter 是否居中
 * @param width 宽度
 * @param height 高度
 * @param x 坐标x
 * @param y 坐标y
 * @param win nk窗体
 */
void initFormPos(const bool &isCenter, const int &width, const int &height, const int &x, const int &y, GLFWwindow *win) {
    if (isCenter) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            int area_x, area_y, area_w, area_h;
            glfwGetMonitorWorkarea(monitor, &area_x, &area_y, &area_w, &area_h);
            const auto cx = area_x + (area_w - width) / 2;
            const auto cy = area_y + (area_h - height) / 2;
            glfwSetWindowPos(win, cx, cy);
        }
    } else {
        if (x > 0 || y > 0) {
            glfwSetWindowPos(win, x, y);
        }
    }
}

/**
 * 初始化字体
 */
void initFonts() {
    nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    int sizes[] = {14, 24, 34};
    auto [data, size] = FontKit::GetEmbeddedFont();
    struct nk_font_config cfg = {0};
    cfg.oversample_h = 1;
    cfg.oversample_v = 1;
    cfg.pixel_snap = 1;
    const nk_rune *chinese_ranges = nk_font_chinese_glyph_ranges();
    cfg.range = chinese_ranges;
    for (int s: sizes) {
        nk_font *f = nk_font_atlas_add_from_memory(atlas, const_cast<void *>(data), size, s, &cfg);
        FontCache[s] = f;
    }
    nk_glfw3_font_stash_end();
}

// 初始化退出状态机
void initExitStatus(GLFWwindow *win) {
    const double currentTime = glfwGetTime();
    const auto dt = currentTime - lastTime;
    lastTime = currentTime;
    if (isClosing) {
        MAIN_FORM_OPACITY -= FADE_SPEED * dt;
        if (MAIN_FORM_OPACITY <= 0.0f) {
            MAIN_FORM_OPACITY = 0.0f;
            glfwSetWindowShouldClose(win, 1);
        }
        glfwSetWindowOpacity(win, MAIN_FORM_OPACITY);
    }
}

/**
 * 初始化标题栏
 * @param win nk窗口
 * @param ctx 上下文
 * @param width 宽度
 * @param height 高度
 */
void initTitleBar(GLFWwindow *win, nk_context *ctx, const int &width) {
    nk_layout_row_begin(ctx, NK_DYNAMIC, TITLE_BAR_HEIGHT, 3);
    const float btnRatio = 45.0f / static_cast<float>(width);
    const float textRatio = 1.0f - btnRatio * 2;
    nk_layout_row_push(ctx, textRatio);
    nk_command_buffer *canvas = nk_window_get_canvas(ctx);
    struct nk_rect totalSpace = nk_window_get_content_region(ctx);
    totalSpace.h = TITLE_BAR_HEIGHT;
    nk_fill_rect(canvas, totalSpace, 0, nk_rgb(45, 45, 48));
    nk_label(ctx, GuiRenderer::GetString(mainForm, "text").c_str(), NK_TEXT_LEFT);
    struct nk_rect titleRect = totalSpace;
    titleRect.w -= 90;
    if (nk_input_is_mouse_hovering_rect(&ctx->input, titleRect)) {
        if (nk_input_is_mouse_down(&ctx->input, NK_BUTTON_LEFT)) {
            if (!window_drag.is_dragging) {
                window_drag.is_dragging = 1;
                glfwGetCursorPos(win, &window_drag.anchor_x, &window_drag.anchor_y);
            }
        }
    }
    if (!nk_input_is_mouse_down(&ctx->input, NK_BUTTON_LEFT)) {
        window_drag.is_dragging = 0;
    }
    if (window_drag.is_dragging) {
        double mx, my;
        glfwGetCursorPos(win, &mx, &my);
        int wx, wy;
        glfwGetWindowPos(win, &wx, &wy);
        const int dx = static_cast<int>(mx - window_drag.anchor_x);
        const int dy = static_cast<int>(my - window_drag.anchor_y);
        if (dx != 0 || dy != 0) {
            glfwSetWindowPos(win, wx + dx, wy + dy);
        }
    }
    nk_layout_row_push(ctx, btnRatio);
    if (nk_button_label(ctx, "_")) {
        glfwIconifyWindow(win);
    }
    nk_layout_row_push(ctx, btnRatio);
    if (nk_button_label(ctx, "X")) {
        isClosing = true;
    }
    nk_layout_row_end(ctx);
}

/**
 * 初始化内容
 * @param ctx 上下文
 * @param height 高度
 */
void initSection(nk_context *ctx, const int &height) {
    const float contentHeight = static_cast<float>(height) - TITLE_BAR_HEIGHT;
    nk_layout_row_dynamic(ctx, contentHeight, 1);
    if (nk_group_begin(ctx, "BxScriptContent", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
        GuiRenderer::RenderContent(ctx, contentHeight, FontCache);
        nk_group_end(ctx);
    }
}

/**
 * 初始化双向绑定（其实是单向）
 * @param win nk窗口
 */
void initDataBind(GLFWwindow *win) {
    static int lastW = 0, lastH = 0, lastX = 0, lastY = 0;
    int w, h, x, y;
    glfwGetWindowSize(win, &w, &h);
    glfwGetWindowPos(win, &x, &y);
    if (w != lastW) {
        mainForm->Set("width", std::make_shared<NumberValue>(w));
        lastW = w;
    }
    if (h != lastH) {
        mainForm->Set("height", std::make_shared<NumberValue>(h));
        lastH = h;
    }
    if (x != lastX) {
        mainForm->Set("x", std::make_shared<NumberValue>(x));
        lastX = x;
    }
    if (y != lastY) {
        mainForm->Set("y", std::make_shared<NumberValue>(y));
        lastY = y;
    }
}

/**
 * 结束绘制并推送缓冲区
 * @param win
 * @param width
 * @param height
 */
void EndFrame(GLFWwindow *win, int const &width, int const &height) {
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    nk_glfw3_render(NK_ANTI_ALIASING_ON);
    glfwSwapBuffers(win);
}

void initAlertBox(std::string const &title, std::string const &content, nk_context *ctx) {
    struct nk_rect s = {20, 100, 200, 150};
    if (nk_popup_begin(ctx, NK_POPUP_STATIC, title.empty() ? "BxScript" : title.c_str(), NK_WINDOW_NO_SCROLLBAR, s)) {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, content.c_str(), NK_TEXT_CENTERED);
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "确 定")) {
            showAlert = false;
            nk_popup_close(ctx);
        }
        nk_popup_end(ctx);
    } else {
        showAlert = false;
    }
}

void initConfirmBox(std::string const &title, std::string const &content, nk_context *ctx) {
    struct nk_rect s = {50, 50, 250, 150};
    if (nk_popup_begin(ctx, NK_POPUP_STATIC, title.empty() ? "BxScript" : title.c_str(), 0, s)) {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, content.c_str(), NK_TEXT_CENTERED);
        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_button_label(ctx, "确定")) {
            showConfirm = false;
            nk_popup_close(ctx);
        }
        if (nk_button_label(ctx, "取消")) {
            showConfirm = false;
            nk_popup_close(ctx);
        }
        nk_popup_end(ctx);
    } else {
        showConfirm = false;
    }
}

void GuiRuntime::Run() {
    glfwInitHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    if (!glfwInit()) {
        return;
    }
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    if (GuiModule::GlobalForms.empty()) return;

    mainForm = std::static_pointer_cast<ObjectValue>(GuiModule::GlobalForms[0]);
    const int initW = static_cast<int>(GuiRenderer::GetFloat(mainForm, "width", 1000));
    const int initH = static_cast<int>(GuiRenderer::GetFloat(mainForm, "height", 600));
    const std::string title = GuiRenderer::GetString(mainForm, "text", "BxScriptApplication");
    const bool isCenter = GuiRenderer::GetBool(mainForm, "center", false);
    const int initX = static_cast<int>(GuiRenderer::GetFloat(mainForm, "x", 100));
    const int initY = static_cast<int>(GuiRenderer::GetFloat(mainForm, "y", 100));

    GLFWwindow *win = glfwCreateWindow(initW, initH, title.c_str(), nullptr, nullptr);
    if (!win) {
        glfwTerminate();
        return;
    }
    // 初始化窗体位置
    initFormPos(isCenter, initW, initH, initX, initY, win);
    glfwMakeContextCurrent(win);
    nk_context *ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    // 字体配置
    initFonts();
    nk_style_set_font(ctx, &FontCache[24]->handle);
    // 主渲染循环
    while (!glfwWindowShouldClose(win)) {
        BeginFrame(); // 开始绘制
        EventLoop::Dispatch(5); // 处理事件循环
        if (GuiModule::GlobalForms.empty()) break;
        initExitStatus(win); // 初始化退出状态机
        int w, h;
        glfwGetWindowSize(win, &w, &h);
        struct nk_rect bound = {0, 0, static_cast<float>(w), static_cast<float>(h)};
        if (nk_begin(ctx, "BxScriptMainForm", bound, NK_WINDOW_BACKGROUND | NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {
            initTitleBar(win, ctx, w);
            initSection(ctx, h);
        }

        // 额外的处理
        if (showAlert) {
            const auto alertTitle = GuiRenderer::GetString(mainForm, "alert-title", "BxScript");
            const auto alertContext = GuiRenderer::GetString(mainForm, "alert-context", "");
            initAlertBox(alertTitle, alertContext, ctx);
        }
        // if (showConfirm) {
        //     const auto confirmTitle = GuiRenderer::GetString(mainForm, "confirm-title", "BxScript");
        //     const auto confirmContext = GuiRenderer::GetString(mainForm, "confirm-context", "");
        //     const auto confirmCallBack = GuiRenderer::GetFunction(mainForm, "confirm-success-fn");
        //     const auto cancelCallBack = GuiRenderer::GetFunction(mainForm, "confirm-cancel-fn");
        //     initConfirmBox(confirmTitle, confirmContext, ctx, confirmCallBack, cancelCallBack);
        // }

        nk_end(ctx);
        EndFrame(win, w, h);
        initDataBind(win);
    }
    nk_glfw3_shutdown();
    glfwTerminate();
}
