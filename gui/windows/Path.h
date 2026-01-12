/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/5
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Path
 */
#ifndef BXSCRIPT_PATH_H
#define BXSCRIPT_PATH_H

#include <string>
#include <vector>

class Path {
public:
    static std::wstring AppDataPath();

    static std::wstring CommonAppDataPath();

    static std::wstring LocalAppDataPath();

    static std::wstring EnsureAppDataPath(const std::wstring &company, const std::wstring &product);

    static std::vector<std::wstring> DriveNames();

private:
    static std::wstring knownFolderPath(int id);
};

#endif
