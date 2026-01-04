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
 * @brief    Kernel32
 */
#ifndef BXSCRIPT_KERNEL32_H
#define BXSCRIPT_KERNEL32_H

#include <windows.h>
#include <tlhelp32.h> // 必须包含：用于进程快照和模块遍历

class Kernel32 {
public:
    // =========================================================================================
    // Group 1: 模块与库加载 (Modules & Libraries)
    // =========================================================================================

    /**
     * 加载动态链接库 (DLL)。
     */
    static HMODULE W32_LoadLibrary(LPCWSTR lpLibFileName) {
        return LoadLibraryW(lpLibFileName);
    }

    /**
     * 获取模块句柄。
     * @param lpModuleName 模块名。如果传 NULL，返回当前进程自身的句柄。
     */
    static HMODULE W32_GetModuleHandle(LPCWSTR lpModuleName) {
        return GetModuleHandleW(lpModuleName);
    }

    /**
     * 释放加载的库。
     */
    static bool W32_FreeLibrary(HMODULE hLibModule) {
        return FreeLibrary(hLibModule) != 0;
    }


    // =========================================================================================
    // Group 2: 全局内存管理 (Global Memory)
    // @note 现在的 Windows 开发中，堆内存通常用 new/malloc，但在剪贴板操作和 COM 中仍需 GlobalAlloc。
    // =========================================================================================

    /**
     * 分配全局内存对象。
     * @param uFlags 通常用 GMEM_MOVEABLE | GMEM_ZEROINIT
     */
    static HGLOBAL W32_GlobalAlloc(UINT uFlags, SIZE_T dwBytes) {
        return GlobalAlloc(uFlags, dwBytes);
    }

    /**
     * 锁定全局内存对象并返回指针。
     * @usage 访问 GlobalAlloc 返回的句柄前必须先 Lock。
     */
    static LPVOID W32_GlobalLock(HGLOBAL hMem) {
        return GlobalLock(hMem);
    }

    /**
     * 解锁全局内存对象。
     * @usage 访问完后必须 Unlock，否则内存无法被系统移动或回收。
     */
    static bool W32_GlobalUnlock(HGLOBAL hMem) {
        return GlobalUnlock(hMem);
    }

    /**
     * 释放全局内存对象。
     */
    static HGLOBAL W32_GlobalFree(HGLOBAL hMem) {
        return GlobalFree(hMem);
    }

    /**
     * 内存块移动/复制 (RtlMoveMemory)。
     * @note 相当于 C 语言的 memmove，处理了内存重叠的情况，比 memcpy 安全。
     */
    static void W32_MoveMemory(PVOID Destination, const VOID *Source, SIZE_T Length) {
        RtlMoveMemory(Destination, Source, Length);
    }


    // =========================================================================================
    // Group 3: 资源操作 (Resources)
    // @usage 用于从 EXE/DLL 文件中读取内嵌的图片、文本、二进制数据。
    // =========================================================================================

    /**
     * 查找资源。
     * @param lpName 资源ID (用 MAKEINTRESOURCE 转换) 或资源名字符串。
     * @param lpType 资源类型 (如 RT_BITMAP, RT_STRING)。
     */
    static HRSRC W32_FindResource(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType) {
        return FindResourceW(hModule, lpName, lpType);
    }

    /**
     * 获取资源大小（字节）。
     */
    static DWORD W32_SizeofResource(HMODULE hModule, HRSRC hResInfo) {
        return SizeofResource(hModule, hResInfo);
    }

    /**
     * 加载资源到内存。
     */
    static HGLOBAL W32_LoadResource(HMODULE hModule, HRSRC hResInfo) {
        return LoadResource(hModule, hResInfo);
    }

    /**
     * 锁定资源以获取其原始数据指针。
     * @return 返回指向资源数据的第一个字节的指针。不需要 Unlock。
     */
    static LPVOID W32_LockResource(HGLOBAL hResData) {
        return LockResource(hResData);
    }


    // =========================================================================================
    // Group 4: 进程与线程 (Process & Thread)
    // =========================================================================================

    /**
     * 获取当前线程的伪句柄。
     */
    static HANDLE W32_GetCurrentThread() {
        return GetCurrentThread();
    }

    /**
     * 获取当前线程 ID。
     */
    static DWORD W32_GetCurrentThreadId() {
        return GetCurrentThreadId();
    }

    /**
     * 打开已存在的进程。
     * @param dwDesiredAccess 访问权限 (如 PROCESS_TERMINATE)。
     */
    static HANDLE W32_OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
        return OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    }

    /**
     * 强制结束进程。
     */
    static bool W32_TerminateProcess(HANDLE hProcess, UINT uExitCode) {
        return TerminateProcess(hProcess, uExitCode) != 0;
    }

    /**
     * 关闭句柄。
     * @warning 任何 Open/Create 出来的 HANDLE (除 GetCurrent...外的) 都必须 Close，否则泄露。
     */
    static bool W32_CloseHandle(HANDLE hObject) {
        return CloseHandle(hObject) != 0;
    }

    /**
     * 获取进程的时间统计信息。
     */
    static bool W32_GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime,
                                    LPFILETIME lpUserTime) {
        return GetProcessTimes(hProcess, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime) != 0;
    }


    // =========================================================================================
    // Group 5: 进程快照与遍历 (ToolHelp32)
    // @usage 用于列出系统中所有正在运行的进程或模块。
    // =========================================================================================

    /**
     * 创建进程/模块快照。
     * @param dwFlags 通常用 TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE
     */
    static HANDLE W32_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
        return CreateToolhelp32Snapshot(dwFlags, th32ProcessID);
    }

    /**
     * 获取快照中的第一个模块。
     * @param lpme 指向 MODULEENTRY32 的指针，调用前必须设置 lpme->dwSize = sizeof(MODULEENTRY32)。
     */
    static bool W32_Module32First(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
        return Module32FirstW(hSnapshot, lpme) != 0;
    }

    /**
     * 获取快照中的下一个模块。
     */
    static bool W32_Module32Next(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
        return Module32NextW(hSnapshot, lpme) != 0;
    }


    // =========================================================================================
    // Group 6: 磁盘与文件系统 (Disk & Files)
    // =========================================================================================

    /**
     * 获取逻辑驱动器位掩码 (Bitmask)。
     * @return 位0=A盘, 位2=C盘...
     */
    static DWORD W32_GetLogicalDrives() {
        return GetLogicalDrives();
    }

    /**
     * 获取逻辑驱动器字符串 (如 "C:\<null>D:\<null>").
     */
    static DWORD W32_GetLogicalDriveStrings(DWORD nBufferLength, LPWSTR lpBuffer) {
        return GetLogicalDriveStringsW(nBufferLength, lpBuffer);
    }

    /**
     * 获取磁盘剩余空间 (支持超大磁盘)。
     */
    static bool W32_GetDiskFreeSpaceEx(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes,
                                       PULARGE_INTEGER lpTotalNumberOfFreeBytes) {
        return GetDiskFreeSpaceExW(lpDirectoryName, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes) != 0;
    }


    // =========================================================================================
    // Group 7: 系统与时间 (System & Time)
    // =========================================================================================

    /**
     * 数学运算：(nNumber * nNumerator) / nDenominator。
     * @usage 结果四舍五入。常用于字体大小换算。
     */
    static int W32_MulDiv(int nNumber, int nNumerator, int nDenominator) {
        return MulDiv(nNumber, nNumerator, nDenominator);
    }

    /**
     * 获取用户默认区域 ID (Locale ID)。
     */
    static LCID W32_GetUserDefaultLCID() {
        return GetUserDefaultLCID();
    }

    /**
     * 获取最后的错误代码。
     * @usage 当其他 API 返回 false/NULL 时调用此函数查看原因。
     */
    static DWORD W32_GetLastError() {
        return GetLastError();
    }

    /**
     * 获取系统时间 (UTC)。
     */
    static void W32_GetSystemTime(LPSYSTEMTIME lpSystemTime) {
        GetSystemTime(lpSystemTime);
    }

    /**
     * 设置系统时间 (需要管理员权限)。
     */
    static bool W32_SetSystemTime(CONST SYSTEMTIME *lpSystemTime) {
        return SetSystemTime(lpSystemTime) != 0;
    }

    /**
     * 获取系统空闲、内核、用户时间。
     */
    static bool W32_GetSystemTimes(LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime) {
        return GetSystemTimes(lpIdleTime, lpKernelTime, lpUserTime) != 0;
    }


    // =========================================================================================
    // Group 8: 字符串操作 (Utilities)
    // =========================================================================================

    /**
     * 获取宽字符串长度。
     * @note 虽然 C++ 有 std::wstring::length，但在处理 raw pointer 时这个很快。
     */
    static int W32_lstrlen(LPCWSTR lpString) {
        return lstrlenW(lpString);
    }

    /**
     * 复制宽字符串。
     * @warning 不安全！必须保证目标缓冲区足够大。建议用 std::wstring 代替。
     */
    static LPWSTR W32_lstrcpy(LPWSTR lpString1, LPCWSTR lpString2) {
        return lstrcpyW(lpString1, lpString2);
    }


    // =========================================================================================
    // Group 9: 控制台 (Console)
    // =========================================================================================

    /**
     * 获取控制台窗口句柄。
     */
    static HWND W32_GetConsoleWindow() {
        return GetConsoleWindow();
    }

    /**
     * 获取控制台屏幕缓冲区信息 (光标位置、颜色等)。
     */
    static bool W32_GetConsoleScreenBufferInfo(HANDLE hConsoleOutput, PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo) {
        return GetConsoleScreenBufferInfo(hConsoleOutput, lpConsoleScreenBufferInfo) != 0;
    }

    /**
     * 设置控制台文本颜色/背景色。
     */
    static bool W32_SetConsoleTextAttribute(HANDLE hConsoleOutput, WORD wAttributes) {
        return SetConsoleTextAttribute(hConsoleOutput, wAttributes) != 0;
    }
};

#endif //BXSCRIPT_KERNEL32_H
