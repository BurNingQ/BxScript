/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025-12-15
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    网络标准库
 */
#ifndef BXSCRIPT_NETMODULE_H
#define BXSCRIPT_NETMODULE_H
#include <regex>

#include "evaluator/EventLoop.h"
#if defined(_WIN32)
#define PLATFORM_NAME "Windows"
#include <windows.h>
#include <wininet.h>
#elif defined(__APPLE__)
#define PLATFORM_NAME "macOS"
#include <unistd.h>
#elif defined(__linux__)
#define PLATFORM_NAME "Linux"
#include <unistd.h>
#else
#define PLATFORM_NAME "Unknown"
#endif

#include "evaluator/Interpreter.h"
#include "evaluator/Logger.h"
#include "evaluator/Value.h"

class NetModule {
    struct UrlParts {
        std::string scheme;
        std::string host;
        int port;
        std::string path;
    };

    static UrlParts ParseUrl(const std::string &url) {
        const std::regex re(R"((http|https)://([^/:]+)(:(\d+))?(/.*)?)");
        std::smatch match;
        if (std::regex_search(url, match, re)) {
            const std::string scheme = match[1];
            const std::string host = match[2];
            int port = (scheme == "https") ? 443 : 80;
            if (match[4].matched) port = std::stoi(match[4]);
            const std::string path = match[5].matched ? match[5].str() : "/";
            return {scheme, host, port, path};
        }
        return {"http", "", 80, ""};
    }

    static ValuePtr SendHttpRequest(const std::string &host, const std::string &path, const std::string &method,
                                    const std::string &postData, const bool isHttps,
                                    const std::vector<std::pair<std::string, std::string> > &headers) {
#if defined(_WIN32)
        auto result = std::make_shared<ObjectValue>();
        const HINTERNET hInt = InternetOpenA("BxScript/1.0",INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
        if (!hInt) {
            result->Set(
                "error", std::make_shared<StringValue>("网络打开错误: " + std::to_string(GetLastError())));
            result->Set("status", std::make_shared<NumberValue>(-1));
            return std::move(result);
        }
        const INTERNET_PORT port = isHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
        const HINTERNET hc = InternetConnectA(hInt, host.c_str(), port, nullptr, nullptr,INTERNET_SERVICE_HTTP, 0, 0);
        if (!hc) {
            InternetCloseHandle(hInt);
            result->Set("error", std::make_shared<StringValue>("网络连接错误: " + std::to_string(GetLastError())));
            result->Set("status", std::make_shared<NumberValue>(-1));
            return std::move(result);
        }
        DWORD dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
        if (isHttps) {
            dwFlags |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
                    INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        }
        HINTERNET hr = HttpOpenRequestA(hc, method.c_str(), path.c_str(), nullptr, nullptr, nullptr, dwFlags, 0);
        if (!hr) {
            InternetCloseHandle(hc);
            InternetCloseHandle(hInt);
            result->Set("error", std::make_shared<StringValue>("网络请求错误: " + std::to_string(GetLastError())));
            result->Set("status", std::make_shared<NumberValue>(-1));
        }
        std::string headerStr{};
        if (!headers.empty()) {
            // 考虑POST要不要自动加上Content-Type: application/x-www-form-urlencoded
            for (auto &[k,v]: headers) {
                headerStr.append(k).append(": ").append(v).append("\r\n");
            }
        }
        const BOOL bs = HttpSendRequestA(hr, headerStr.empty() ? nullptr : headerStr.c_str(),
                                         headers.empty() ? 0 : static_cast<DWORD>(headerStr.length()),
                                         (LPVOID) (postData.empty() ? nullptr : postData.c_str()),
                                         (DWORD) postData.length()
        );
        if (!bs) {
            result->Set("error", std::make_shared<StringValue>("网络请求错误: " + std::to_string(GetLastError())));
            result->Set("status", std::make_shared<NumberValue>(-1));
            return std::move(result);
        }
        DWORD statusCode = 0;
        DWORD length = sizeof(DWORD);
        HttpQueryInfoA(hr, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &length, NULL);
        result->Set("status", std::make_shared<NumberValue>(statusCode));
        // 读取响应 Body
        std::string response = "";
        constexpr int bufferSize = 4096;
        char buffer[bufferSize];
        DWORD bytesRead = 0;
        while (InternetReadFile(hr, buffer, bufferSize, &bytesRead) && bytesRead > 0) {
            response.append(buffer, bytesRead);
        }
        InternetCloseHandle(hr);
        InternetCloseHandle(hc);
        InternetCloseHandle(hInt);
        result->Set("body", std::make_shared<StringValue>(response));
        result->Set("error", std::make_shared<NullValue>());
        return std::move(result);
#else
        // 其他平台稍后更新
#endif
    }

    static void InitGet(const std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.size() < 2) {
                    Logger::Error("参数错误: Http.get(url, callback)");
                }
                const std::string url = args[0]->ToString();
                const auto &callback = args[1];
                auto parts = ParseUrl(url);
                if (parts.host.empty()) {
                    Logger::Error("参数错误: Http.get(url, callback)");
                }
                std::thread t([parts, callback]() {
                    const std::vector<std::pair<std::string, std::string> > headers;
                    auto result = SendHttpRequest(parts.host, parts.path, "GET", "", parts.scheme == "https", headers);
                    EventLoop::Enqueue(callback, {std::move(result)});
                    EventLoop::RemoveActiveTask();
                });
                EventLoop::AddActiveTask();
                t.detach();
                return std::make_shared<NullValue>();
            });
        o->Set("get", fn);
    }

public:
    static ValuePtr CreateNetModule() {
        auto module = std::make_shared<ObjectValue>();
        InitGet(module);
        return module;
    }
};


#endif //BXSCRIPT_NETMODULE_H
