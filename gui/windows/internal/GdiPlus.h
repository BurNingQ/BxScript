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
 * @brief    Gdiplus
 */
#ifndef BXSCRIPT_GDIPLUS_H
#define BXSCRIPT_GDIPLUS_H

#include <string>
#include <windows.h>
#include <gdiplus.h>

class GdiPlus {
public:
    // =========================================================================================
    // Helper: 错误码转字符串
    // =========================================================================================

    /**
     * 将 GpStatus 枚举转为可读字符串。
     * @usage 仅在 API 返回非 Ok 时调用此函数抛出异常或打印日志。
     */
    static std::string GetStatusStr(int s) {
        switch (s) {
            case 0: return "Ok";
            case 1: return "GenericError";
            case 2: return "InvalidParameter";
            case 3: return "OutOfMemory";
            case 4: return "ObjectBusy";
            case 5: return "InsufficientBuffer";
            case 6: return "NotImplemented";
            case 7: return "Win32Error";
            case 8: return "WrongState";
            case 9: return "Aborted";
            case 10: return "FileNotFound";
            case 11: return "ValueOverflow";
            case 12: return "AccessDenied";
            case 13: return "UnknownImageFormat";
            case 14: return "FontFamilyNotFound";
            case 15: return "FontStyleNotFound";
            case 16: return "NotTrueTypeFont";
            case 17: return "UnsupportedGdiplusVersion";
            case 18: return "GdiplusNotInitialized";
            case 19: return "PropertyNotFound";
            case 20: return "PropertyNotSupported";
            case 21: return "ProfileNotFound";
            default: return "Unknown Status Value";
        }
    }

    // =========================================================================================
    // Group 1: 初始化与销毁 (必须调用)
    // =========================================================================================

    /**
     * GDI+ 启动。
     * @return 返回 token，必须保存起来传给 Shutdown。
     */
    static Gdiplus::GpStatus W32_GdiplusStartup(ULONG_PTR *token) {
        Gdiplus::GdiplusStartupInput input; // 使用默认构造函数即可
        return Gdiplus::GdiplusStartup(token, &input, nullptr);
    }

    /**
     * GDI+ 关闭。
     * @warning 必须在程序退出前调用，否则会内存泄漏。
     */
    static void W32_GdiplusShutdown(ULONG_PTR token) {
        Gdiplus::GdiplusShutdown(token);
    }


    // =========================================================================================
    // Group 2: Graphics (画布) 管理
    // @note GDI+ 所有的绘图都不在 HDC 上直接进行，而是通过 Graphics 对象。
    // =========================================================================================

    /**
     * 从 HDC 创建 Graphics 对象。
     * @usage 在 WM_PAINT 中，先 GetDC，然后用这个函数创建一个 GDI+ 画布。
     */
    static Gdiplus::GpStatus W32_GdipCreateFromHDC(HDC hdc, Gdiplus::GpGraphics **graphics) {
        return Gdiplus::DllExports::GdipCreateFromHDC(hdc, graphics);
    }

    /**
     * 销毁 Graphics 对象。
     * @warning 每次画完必须删除 Graphics 对象，否则会闪烁或泄露。
     */
    static Gdiplus::GpStatus W32_GdipDeleteGraphics(Gdiplus::GpGraphics *graphics) {
        return Gdiplus::DllExports::GdipDeleteGraphics(graphics);
    }

    /**
     * 清空画布并填充背景色。
     */
    static Gdiplus::GpStatus W32_GdipGraphicsClear(Gdiplus::GpGraphics *graphics, Gdiplus::ARGB color) {
        return Gdiplus::DllExports::GdipGraphicsClear(graphics, color);
    }


    // =========================================================================================
    // Group 3: Bitmap (图片) 加载
    // =========================================================================================

    /**
     * 从文件加载图片 (支持 PNG, JPG, BMP, GIF 等)。
     */
    static Gdiplus::GpStatus W32_GdipCreateBitmapFromFile(GDIPCONST WCHAR *filename, Gdiplus::GpBitmap **bitmap) {
        return Gdiplus::DllExports::GdipCreateBitmapFromFile(filename, bitmap);
    }

    /**
     * 从内存流加载图片 (IStream)。
     * @usage 用于加载内嵌资源或网络下载的图片数据。
     */
    static Gdiplus::GpStatus W32_GdipCreateBitmapFromStream(IStream *stream, Gdiplus::GpBitmap **bitmap) {
        return Gdiplus::DllExports::GdipCreateBitmapFromStream(stream, bitmap);
    }

    /**
     * 从资源 ID 加载图片。
     */
    static Gdiplus::GpStatus W32_GdipCreateBitmapFromResource(HINSTANCE hInstance, GDIPCONST WCHAR *resName, Gdiplus::GpBitmap **bitmap) {
        return Gdiplus::DllExports::GdipCreateBitmapFromResource(hInstance, resName, bitmap);
    }

    /**
     * 将 GDI+ Bitmap 转换为传统的 GDI HBITMAP。
     * @usage 当你需要把图片传给不支持 GDI+ 的老旧 API (如设置窗口背景) 时使用。
     * @param argb 背景色 (处理透明通道用)
     */
    static Gdiplus::GpStatus W32_GdipCreateHBITMAPFromBitmap(Gdiplus::GpBitmap *bitmap, HBITMAP *hbmReturn, Gdiplus::ARGB background) {
        return Gdiplus::DllExports::GdipCreateHBITMAPFromBitmap(bitmap, hbmReturn, background);
    }

    /**
     * 获取图片宽度。
     */
    static Gdiplus::GpStatus W32_GdipGetImageWidth(Gdiplus::GpImage *image, UINT *width) {
        return Gdiplus::DllExports::GdipGetImageWidth(image, width);
    }

    /**
     * 获取图片高度。
     */
    static Gdiplus::GpStatus W32_GdipGetImageHeight(Gdiplus::GpImage *image, UINT *height) {
        return Gdiplus::DllExports::GdipGetImageHeight(image, height);
    }

    /**
     * 销毁图片对象。
     */
    static Gdiplus::GpStatus W32_GdipDisposeImage(Gdiplus::GpImage *image) {
        return Gdiplus::DllExports::GdipDisposeImage(image);
    }


    // =========================================================================================
    // Group 4: 绘图操作 (绘制图片)
    // =========================================================================================

    /**
     * 绘制图片 (DrawImage)。
     * @param x, y 绘制位置
     * @param w, h 绘制大小 (会自动缩放)
     */
    static Gdiplus::GpStatus W32_GdipDrawImageRectI(Gdiplus::GpGraphics *graphics, Gdiplus::GpImage *image, int x, int y, int w, int h) {
        return Gdiplus::DllExports::GdipDrawImageRectI(graphics, image, x, y, w, h);
    }

    /**
     * 绘制图片 (原样绘制，不缩放)。
     */
    static Gdiplus::GpStatus W32_GdipDrawImageI(Gdiplus::GpGraphics *graphics, Gdiplus::GpImage *image, int x, int y) {
        return Gdiplus::DllExports::GdipDrawImageI(graphics, image, x, y);
    }
};

#endif //BXSCRIPT_GDIPLUS_H
