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
 * @brief    Gdi32
 */
#ifndef BXSCRIPT_GDI32_H
#define BXSCRIPT_GDI32_H

#include <windows.h>

/**
 * Gdi32 封装类
 * 负责所有的图形绘制、字体管理、颜色控制以及 OpenGL 上下文准备。
 */
class Gdi32 {
public:
    // =========================================================================================
    // Group 1: 设备上下文 (DC) 管理
    // DC 是 GDI 绘图的画布，所有的绘图操作都需要一个 DC 句柄。
    // =========================================================================================

    /**
     * 创建一个与指定设备（通常是屏幕）兼容的内存 DC。
     * @usage 常用于“双缓冲”绘图技术，先在内存 DC 画好，再一次性贴到屏幕上，防止闪烁。
     */
    static HDC W32_CreateCompatibleDC(HDC hdc) {
        return CreateCompatibleDC(hdc);
    }

    /**
     * 创建指定设备的 DC（如打印机、特定显示器）。
     * @param pwszDriver 驱动名（通常传 NULL 或 "DISPLAY"）
     * @param pwszDevice 设备名
     */
    static HDC W32_CreateDC(LPCWSTR pwszDriver, LPCWSTR pwszDevice, LPCWSTR pszPort, CONST DEVMODEW *pdm) {
        return CreateDCW(pwszDriver, pwszDevice, pszPort, pdm);
    }

    /**
     * 创建一个信息上下文 (IC)。
     * @usage IC 只能用来获取信息（如屏幕大小、支持的颜色数），不能用来画图，开销比 CreateDC 小。
     */
    static HDC W32_CreateIC(LPCWSTR pszDriver, LPCWSTR pszDevice, LPCWSTR pszPort, CONST DEVMODEW *pdm) {
        return CreateICW(pszDriver, pszDevice, pszPort, pdm);
    }

    /**
     * 销毁 DC。
     * @warning 必须与 CreateDC/CreateCompatibleDC 成对使用，否则内存泄露。
     * @note 不要用它删除 GetDC 获取的句柄（那个要用 ReleaseDC）。
     */
    static bool W32_DeleteDC(HDC hdc) {
        return DeleteDC(hdc);
    }

    /**
     * 重置 DC 配置（如改变打印机纸张方向）。
     */
    static HDC W32_ResetDC(HDC hdc, CONST DEVMODEW *lpdm) {
        return ResetDCW(hdc, lpdm);
    }

    /**
     * 获取设备能力信息。
     * @usage 获取屏幕分辨率、DPI、是否支持特定绘图功能等。
     * @param index 如 HORZRES (水平分辨率), LOGPIXELSX (水平DPI)。
     */
    static int W32_GetDeviceCaps(HDC hdc, const int index) {
        return GetDeviceCaps(hdc, index);
    }


    // =========================================================================================
    // Group 2: GDI 对象管理 (笔、刷、字体、位图)
    // =========================================================================================

    /**
     * 【核心函数】将对象（笔、刷、字体、位图）选中到 DC 中。
     * @return 返回被替换掉的旧对象句柄（通常需要保存起来，画完图后再选回去）。
     */
    static HGDIOBJ W32_SelectObject(HDC hdc, HGDIOBJ h) {
        return SelectObject(hdc, h);
    }

    /**
     * 【核心函数】删除 GDI 对象。
     * @warning 绝对不要删除正在被 DC 选中的对象！先 SelectObject 恢复旧对象，再 DeleteObject。
     */
    static bool W32_DeleteObject(HGDIOBJ hob) {
        return DeleteObject(hob);
    }

    /**
     * 获取系统预设对象（不需要手动删除）。
     * @param index 如 WHITE_BRUSH (白刷), DEFAULT_GUI_FONT (系统字体)。
     * @fix 修复了原代码调用 GetDeviceCaps 的错误。
     */
    static HGDIOBJ W32_GetStockObject(int index) {
        return GetStockObject(index);
    }

    /**
     * 获取 GDI 对象的信息。
     * @usage 比如传入 HBITMAP，可以获取图片的宽、高、颜色格式。
     */
    static int W32_GetObject(HANDLE h, int c, LPVOID pv) {
        return GetObjectW(h, c, pv);
    }


    // =========================================================================================
    // Group 3: 颜色与背景控制
    // =========================================================================================

    /**
     * 创建纯色画刷（用于填充矩形、背景）。
     */
    static HBRUSH W32_CreateSolidBrush(const COLORREF color) {
        return CreateSolidBrush(color);
    }

    /**
     * 根据逻辑结构创建画刷（支持阴影线 Hatch）。
     */
    static HBRUSH W32_CreateBrushIndirect(LOGBRUSH *lplb) {
        return CreateBrushIndirect(lplb);
    }

    /**
     * 创建扩展画笔（支持虚线、端点样式等）。
     */
    static HPEN W32_ExtCreatePen(DWORD iPenStyle, DWORD cWidth, CONST LOGBRUSH *plbrush, DWORD cStyle, CONST DWORD *pstyle) {
        return ExtCreatePen(iPenStyle, cWidth, plbrush, cStyle, pstyle);
    }

    /**
     * 设置文字颜色。
     */
    static COLORREF W32_SetTextColor(HDC hdc, COLORREF color) {
        return SetTextColor(hdc, color);
    }

    /**
     * 设置文字背景色（当 BkMode 为 OPAQUE 时有效）。
     */
    static COLORREF W32_SetBkColor(HDC hdc, COLORREF color) {
        return SetBkColor(hdc, color);
    }

    /**
     * 设置背景模式。
     * @param mode TRANSPARENT (文字背景透明，常用), OPAQUE (文字背景有颜色填充)。
     */
    static int W32_SetBkMode(HDC hdc, int mode) {
        return SetBkMode(hdc, mode);
    }

    /**
     * 设置画刷原点（用于对齐填充图案）。
     */
    static bool W32_SetBrushOrg(HDC hdc, int x, int y, LPPOINT lppt) {
        return SetBrushOrgEx(hdc, x, y, lppt);
    }


    // =========================================================================================
    // Group 4: 矢量绘图 (点、线、面)
    // =========================================================================================

    /**
     * 移动当前画笔位置（不画线）。
     * @usage 相当于“抬起笔移动到(x,y)”。
     */
    static bool W32_MoveTo(HDC hdc, int x, int y, LPPOINT lppt) {
        return MoveToEx(hdc, x, y, lppt);
    }

    /**
     * 从当前位置画线到 (x,y)。
     */
    static bool W32_LineTo(HDC hdc, int x, int y) {
        return LineTo(hdc, x, y);
    }

    /**
     * 绘制矩形（使用当前 Pen 画边框，使用当前 Brush 填充内部）。
     */
    static bool W32_Rectangle(HDC hdc, int left, int top, int right, int bottom) {
        return Rectangle(hdc, left, top, right, bottom);
    }

    /**
     * 绘制椭圆（或圆）。
     */
    static bool W32_Ellipse(HDC hdc, int left, int top, int right, int bottom) {
        return Ellipse(hdc, left, top, right, bottom);
    }


    // =========================================================================================
    // Group 5: 位图操作 (BitBlt, DIB)
    // =========================================================================================

    /**
     * 【核心函数】位块传输（复制像素）。
     * @usage 将源 DC 的一块区域复制到目标 DC。
     * @param dwRop 光栅操作码 (SRCCOPY: 直接复制, NOTSRCCOPY: 反色等)。
     * @fix 修复了返回值逻辑 (API非0即成功)。
     */
    static bool W32_BitBlt(const HDC hdcDest, const int nXDest, const int nYDest, const int nWidth, const int nHeight,
                           const HDC hdcSrc, const int nXSrc, const int nYSrc, const int dwRop) {
        return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop) != 0;
    }

    /**
     * 拉伸位块传输（支持缩放）。
     * @usage 比如把一张小图放大画到屏幕上。
     */
    static bool W32_StretchBlt(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
                               HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop) {
        return StretchBlt(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, rop);
    }

    /**
     * 图案块传输（用当前 Brush 填充区域）。
     */
    static bool W32_PatBlt(HDC hdc, int nXLeft, int nYLeft, int nWidth, int nHeight, int dwRop) {
        return PatBlt(hdc, nXLeft, nYLeft, nWidth, nHeight, dwRop);
    }

    /**
     * 设置拉伸模式（决定缩放图片的质量）。
     * @param mode COLORONCOLOR (快但粗糙), HALFTONE (慢但质量好)。
     */
    static int W32_SetStretchBltMode(HDC hdc, int mode) {
        return SetStretchBltMode(hdc, mode);
    }

    /**
     * 创建兼容位图（用于内存绘图）。
     * @usage 创建的位图刚开始是黑色的，需要选入内存 DC 才能画东西进去。
     */
    static HBITMAP W32_CreateCompatibleBitmap(HDC hdc, int cx, int cy) {
        return CreateCompatibleBitmap(hdc, cx, cy);
    }

    /**
     * 创建 DIB (设备无关位图) 区域。
     * @usage 高级功能，允许直接访问像素数组（ppvBits）。
     */
    static HBITMAP W32_CreateDIBSection(HDC hdc, CONST BITMAPINFO *lpbmi, UINT usage, VOID **ppvBits, HANDLE hSection, DWORD offset) {
        return CreateDIBSection(hdc, lpbmi, usage, ppvBits, hSection, offset);
    }

    /**
     * 直接将像素数组绘制到设备上。
     */
    static int W32_SetDIBitsToDevice(HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines,
                                     CONST VOID *lpvBits, CONST BITMAPINFO *lpbmi, UINT ColorUse) {
        return SetDIBitsToDevice(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);
    }

    /**
     * 获取位图的像素数据。
     * @fix 修复了返回值逻辑 (API返回复制的行数，0为失败)。
     */
    static int W32_GetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT usage) {
        return GetDIBits(hdc, hbm, start, cLines, lpvBits, lpbmi, usage);
    }


    // =========================================================================================
    // Group 6: 文本与字体
    // =========================================================================================

    /**
     * 创建字体（间接方式）。
     * @usage 脚本层通常构建一个 LOGFONT 结构体来描述字体。
     */
    static HFONT W32_CreateFontIndirect(const LOGFONTW &font) {
        return CreateFontIndirectW(&font);
    }

    /**
     * 获取当前字体的度量信息（如行高、基线位置）。
     */
    static bool W32_GetTextMetrics(HDC hdc, LPTEXTMETRICW lptm) {
        return GetTextMetricsW(hdc, lptm);
    }

    /**
     * 计算字符串在屏幕上占据的宽和高。
     * @usage 用于自动布局。
     */
    static bool W32_GetTextExtentPoint32(HDC hdc, LPCWSTR lpString, int c, LPSIZE psizl) {
        return GetTextExtentPoint32W(hdc, lpString, c, psizl);
    }

    /**
     * 高级文本测量（计算能塞下多少个字符）。
     * @usage 用于实现文本自动换行。
     */
    static bool W32_GetTextExtentExPoint(HDC hdc, LPCWSTR lpszString, int cchString, int nMaxExtent, LPINT lpnFit, LPINT lpnDx, LPSIZE lpSize) {
        return GetTextExtentExPointW(hdc, lpszString, cchString, nMaxExtent, lpnFit, lpnDx, lpSize);
    }


    // =========================================================================================
    // Group 7: 增强型元文件 (EMF)
    // @usage EMF 是一种矢量图格式，记录了一系列的 GDI 绘图指令。
    // =========================================================================================

    static HDC W32_CreateEnhMetaFile(HDC hdc, LPCWSTR lpFilename, CONST RECT *lprc, LPCWSTR lpDesc) {
        return CreateEnhMetaFileW(hdc, lpFilename, lprc, lpDesc);
    }

    static HENHMETAFILE W32_CloseEnhMetaFile(HDC hdc) {
        return CloseEnhMetaFile(hdc);
    }

    static bool W32_DeleteEnhMetaFile(HENHMETAFILE f) {
        return DeleteEnhMetaFile(f);
    }

    static HENHMETAFILE W32_GetEnhMetaFile(LPCWSTR lpName) {
        return GetEnhMetaFileW(lpName);
    }

    static bool W32_PlayEnhMetaFile(HDC hdc, HENHMETAFILE hmf, CONST RECT *lprect) {
        return PlayEnhMetaFile(hdc, hmf, lprect);
    }

    static HENHMETAFILE W32_CopyEnhMetaFile(HENHMETAFILE hemfSrc, LPCWSTR lpszFile) {
        return CopyEnhMetaFileW(hemfSrc, lpszFile);
    }

    static unsigned int W32_GetEnhMetaFileHeader(HENHMETAFILE hemf, UINT nSize, LPENHMETAHEADER lpEnhMetaHeader) {
        return GetEnhMetaFileHeader(hemf, nSize, lpEnhMetaHeader);
    }

    static unsigned int W32_GetEnhMetaFilePixelFormat(HENHMETAFILE hemf, UINT cbBuffer, PIXELFORMATDESCRIPTOR *ppfd) {
        return GetEnhMetaFilePixelFormat(hemf, cbBuffer, ppfd);
    }


    // =========================================================================================
    // Group 8: 打印相关
    // =========================================================================================

    static int W32_StartDoc(HDC hdc, CONST DOCINFOW *lpdi) {
        return StartDocW(hdc, lpdi);
    }

    static int W32_EndDoc(HDC hdc) {
        return EndDoc(hdc);
    }

    static int W32_AbortDoc(HDC hdc) {
        return AbortDoc(hdc);
    }

    static int W32_StartPage(HDC hdc) {
        return StartPage(hdc);
    }

    static int W32_EndPage(HDC hdc) {
        return EndPage(hdc);
    }


    // =========================================================================================
    // Group 9: OpenGL 像素格式配置
    // @usage 这些函数用于配置 HDC 以支持 OpenGL 硬件加速渲染。
    // =========================================================================================

    static int W32_ChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd) {
        return ChoosePixelFormat(hdc, ppfd);
    }

    static bool W32_SetPixelFormat(HDC hdc, int format, CONST PIXELFORMATDESCRIPTOR *ppfd) {
        return SetPixelFormat(hdc, format, ppfd);
    }

    static int W32_GetPixelFormat(HDC hdc) {
        return GetPixelFormat(hdc);
    }

    static int W32_DescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd) {
        return DescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd);
    }

    static bool W32_SwapBuffers(HDC hdc) {
        return SwapBuffers(hdc);
    }

    /**
     * 设置前景混合模式 (Raster Operation)。
     * @usage 设为 R2_NOTXORPEN 可用于绘制“橡皮筋”选择框（画两次自动消失，不破坏背景）。
     * @param mode R2_COPYPEN (默认覆盖), R2_NOTXORPEN (异或), R2_BLACK (全黑) 等。
     */
    static int W32_SetROP2(HDC hdc, int mode) {
        return SetROP2(hdc, mode);
    }

    /**
     * 绘制圆角矩形。
     * @param w 圆角椭圆的宽
     * @param h 圆角椭圆的高
     */
    static bool W32_RoundRect(HDC hdc, int left, int top, int right, int bottom, int w, int h) {
        return RoundRect(hdc, left, top, right, bottom, w, h);
    }

    /**
     * 快速填充矩形区域（不画边框）。
     * @note 这个函数比 Rectangle+CreateSolidBrush 更快，且不需要把 Brush 选入 DC。
     * @param lprc 目标矩形指针
     * @param hbr 用来填充的画刷句柄
     */
    static int W32_FillRect(HDC hdc, CONST RECT *lprc, HBRUSH hbr) {
        return FillRect(hdc, lprc, hbr);
    }

    /**
     * 快速绘制矩形边框（不填充内部）。
     * @note 边框宽度永远是 1 个像素。如果需要粗边框，请用 W32_Rectangle。
     */
    static int W32_FrameRect(HDC hdc, CONST RECT *lprc, HBRUSH hbr) {
        return FrameRect(hdc, lprc, hbr);
    }

    /**
     * 创建逻辑字体（全参数版）。
     * @param nHeight 字体高度（负数代表字符高度，正数代表字符元胞高度）
     * @param nWidth 字体宽度（0 表示根据高度自动匹配）
     * @param nEscapement 倾斜角度（0.1度为单位，用于旋转文字）
     * @param nOrientation 字符基线倾斜角度
     * @param fnWeight 粗细 (FW_NORMAL=400, FW_BOLD=700)
     * @param fdwItalic 是否斜体
     * @param fdwUnderline 是否下划线
     * @param fdwStrikeOut 是否删除线
     * @param fdwCharSet 字符集 (GB2312_CHARSET, ANSI_CHARSET, DEFAULT_CHARSET)
     * @param fdwOutputPrecision 输出精度
     * @param fdwClipPrecision 裁剪精度
     * @param fdwQuality 输出质量 (CLEARTYPE_QUALITY = 抗锯齿)
     * @param fdwPitchAndFamily 字体族
     * @param lpszFace 字体名称 (如 "微软雅黑")
     */
    static HFONT W32_CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight,
                                DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet,
                                DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality,
                                DWORD fdwPitchAndFamily, LPCWSTR lpszFace) {
        return CreateFontW(nHeight, nWidth, nEscapement, nOrientation, fnWeight,
                           fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet,
                           fdwOutputPrecision, fdwClipPrecision, fdwQuality,
                           fdwPitchAndFamily, lpszFace);
    }
};

#endif //BXSCRIPT_GDI32_H
