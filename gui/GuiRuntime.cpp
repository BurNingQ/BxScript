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
#include <iostream>
#include "../evaluator/EventLoop.h"
#include "../stdlib/GuiModule.h"
#include "GuiRenderer.h"
#include "common/FontKit.h"

struct {
    int is_dragging;
    double anchor_x, anchor_y;
} window_drag = {0, 0.0, 0.0};

ValuePtr GetMainForm() {
    return GuiModule::GlobalForms[0];
}

void GuiRuntime::Run() {
    glfwInitHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    if (!glfwInit()) {
        std::cerr << "[GUI] GLFW 初始化失败" << std::endl;
        return;
    }
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWwindow *win = glfwCreateWindow(1000, 600, "BxScript WinForm", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    nk_context *ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    // 字体配置
    nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font_config cfg = {0};
    cfg.oversample_h = 1;
    cfg.oversample_v = 1;
    cfg.pixel_snap = 1;
    const nk_rune *chinese_ranges = nk_font_chinese_glyph_ranges();
    cfg.range = chinese_ranges;
    auto [data, size] = FontKit::GetEmbeddedFont();
    const nk_font *font = nk_font_atlas_add_from_memory(
        atlas,
        const_cast<void *>(data),
        size,
        25.0f,
        &cfg
    );
    nk_glfw3_font_stash_end();
    if (font) nk_style_set_font(ctx, &font->handle);
    // 主渲染循环
    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        nk_glfw3_new_frame();
        EventLoop::Dispatch(5);
        if (GuiModule::GlobalForms.empty()) {
            break;
        }
        int w, h;
        glfwGetWindowSize(win, &w, &h);
        struct nk_rect bound = {0, 0, static_cast<float>(w), static_cast<float>(h)};
        if (nk_begin(ctx, "BxScriptBackGround", bound, NK_WINDOW_BACKGROUND | NK_WINDOW_NO_INPUT)) {
            constexpr float titleHeight = 40.0f;
            nk_layout_row_begin(ctx, NK_DYNAMIC, titleHeight, 3);
            const float btnRatio = 45.0f / (float) w;
            const float textRatio = 1.0f - btnRatio * 2;
            nk_layout_row_push(ctx, textRatio);
            nk_command_buffer *canvas = nk_window_get_canvas(ctx);
            struct nk_rect totalSpace = nk_window_get_content_region(ctx);
            totalSpace.h = titleHeight;
            nk_fill_rect(canvas, totalSpace, 0, nk_rgb(45, 45, 48));
            nk_label(ctx, GetMainForm()->Get("text")->ToString().c_str(), NK_TEXT_LEFT);
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
                int dx = (int) (mx - window_drag.anchor_x);
                int dy = (int) (my - window_drag.anchor_y);
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
                glfwSetWindowShouldClose(win, 1);
            }
            nk_layout_row_end(ctx);
        }
        nk_layout_row_dynamic(ctx, 200, 1);
        if (nk_group_begin(ctx, "BxScriptContent", NK_WINDOW_BORDER)) {
            GuiRenderer::RenderContent(ctx);
            nk_group_end(ctx);
        }
        nk_end(ctx);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(win);
    }
    nk_glfw3_shutdown();
    glfwTerminate();
}
