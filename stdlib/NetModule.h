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
#include <random>
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

    static bool IsTextContentType(const std::string &type) {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t.find("text/") != std::string::npos) return true;
        if (t.find("json") != std::string::npos) return true;
        if (t.find("xml") != std::string::npos) return true;
        if (t.find("javascript") != std::string::npos) return true;
        if (t.find("html") != std::string::npos) return true;
        return false;
    }

    static bool IsBinaryContent(const std::vector<unsigned char> &data) {
        size_t checkLen = std::min(data.size(), (size_t) 512);
        for (size_t i = 0; i < checkLen; ++i) {
            unsigned char c = data[i];
            if (c == 0x00) return true;
        }
        return false;
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
        HttpQueryInfoA(hr, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &length, nullptr);
        result->Set("status", std::make_shared<NumberValue>(statusCode));
        // 返回类型
        char contentTypeBuf[256];
        DWORD ctLen = sizeof(contentTypeBuf);
        std::string contentType{};
        if (HttpQueryInfoA(hr, HTTP_QUERY_CONTENT_TYPE, contentTypeBuf, &ctLen, nullptr)) {
            contentType = std::string(contentTypeBuf, ctLen);
        }
        std::vector<unsigned char> bodyBytes;
        bool isText = false;
        if (!contentType.empty()) {
            if (IsTextContentType(contentType)) {
                isText = true;
            }
        } else {
            if (!IsBinaryContent(bodyBytes)) {
                isText = true;
            }
        }
        constexpr int bufferSize = 4096;
        char buffer[bufferSize];
        DWORD bytesRead = 0;
        while (InternetReadFile(hr, buffer, bufferSize, &bytesRead) && bytesRead > 0) {
            bodyBytes.insert(bodyBytes.end(), buffer, buffer + bytesRead);
        }
        if (isText) {
            result->Set("body", std::make_shared<BufferValue>(std::move(bodyBytes)));
        } else {
            std::string strBody(bodyBytes.begin(), bodyBytes.end());
            result->Set("body", std::make_shared<StringValue>(std::move(strBody)));
        }
        InternetCloseHandle(hr);
        InternetCloseHandle(hc);
        InternetCloseHandle(hInt);
        result->Set("type", std::make_shared<StringValue>(contentType));
        result->Set("error", std::make_shared<NullValue>());
        return std::move(result);
#else
        // 其他平台稍后更新
#endif
    }

    static std::string UrlEncode(const std::string &value) {
        std::ostringstream escaped;
        escaped.fill('0');
        for (const char c: value) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << std::hex << std::uppercase;
                escaped << '%' << std::setw(2) << int((unsigned char) c);
                escaped << std::nouppercase;
            }
        }
        return escaped.str();
    }

    static std::string GenerateBoundary() {
        const auto now = std::chrono::high_resolution_clock::now();
        const auto nanos = now.time_since_epoch().count();
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<unsigned long long> dis;
        std::stringstream ss;
        ss << "---------------------------BxScriptBoundary";
        ss << std::hex << nanos;
        ss << std::hex << dis(gen);
        ss << std::hex << dis(gen);

        return ss.str();
    }

    static std::pair<std::string, std::string> BuildMultipartBody(const std::shared_ptr<ObjectValue> &obj) {
        std::string boundary = GenerateBoundary();
        std::string body;
        for (const auto &[key, val]: obj->Properties) {
            body += "--" + boundary + "\r\n";
            if (val->type == ValueType::BUFFER) {
                body += "Content-Disposition: form-data; name=\"" + key + "\"; filename=\"" + key + "\"\r\n";
                body += "Content-Type: application/octet-stream\r\n\r\n";
                auto buf = std::static_pointer_cast<BufferValue>(val);
                body.append((char *) buf->Buffer.data(), buf->Buffer.size());
            } else {
                body += "Content-Disposition: form-data; name=\"" + key + "\"\r\n\r\n";
                body += val->ToString();
            }
            body += "\r\n";
        }
        body += "--" + boundary + "--\r\n";
        return {body, boundary};
    }

    static void RegisterRequest(const std::shared_ptr<ObjectValue> &o,
                                const std::string &jsName,
                                const std::string &method,
                                bool hasBody) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [method, hasBody](const std::vector<ValuePtr> &args) -> ValuePtr {
                size_t minArgs = hasBody ? 2 : 1;
                if (args.size() < minArgs) {
                    Logger::Error("参数错误: Net." + method + "(url, " + (hasBody ? "data, " : "") + "[header], [callback])");
                }
                const std::string url = args[0]->ToString();
                auto parts = ParseUrl(url);
                if (parts.host.empty()) Logger::Error("URL 解析失败");
                std::string postData;
                std::vector<std::pair<std::string, std::string> > headers;
                ValuePtr callback = nullptr;
                int dataIdx = 1;
                int headerIdx = hasBody ? 2 : 1;
                int callbackIdx = headerIdx;
                std::string contentType = "application/json";
                bool userProvidedContentType = false;
                if (args.size() > headerIdx && args[headerIdx]->type == ValueType::OBJECT) {
                    auto const oHeaders = std::static_pointer_cast<ObjectValue>(args[headerIdx]);
                    for (const auto &[key, val]: oHeaders->Properties) {
                        std::string v = val->ToString();
                        headers.emplace_back(key, v);
                        auto tk = StringKit::ToUpperCase(key);
                        if (tk == "CONTENT-TYPE") {
                            contentType = v;
                            userProvidedContentType = true;
                        }
                    }
                    callbackIdx++;
                }
                if (hasBody && args.size() > dataIdx) {
                    auto &bodyVal = args[dataIdx];
                    if (bodyVal->type == ValueType::STRING) {
                        postData = bodyVal->ToString();
                    } else if (bodyVal->type == ValueType::BUFFER) {
                        const auto buf = std::static_pointer_cast<BufferValue>(bodyVal);
                        postData.assign((char *) buf->Buffer.data(), buf->Buffer.size());
                    } else if (bodyVal->type == ValueType::OBJECT) {
                        const auto obj = std::static_pointer_cast<ObjectValue>(bodyVal);
                        if (contentType.find("application/json") != std::string::npos) {
                            postData = JsonKit::ValueToJson(bodyVal).dump();
                        } else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
                            bool first = true;
                            for (auto &[k, v]: obj->Properties) {
                                if (!first) postData += "&";
                                postData += UrlEncode(k) + "=" + UrlEncode(v->ToString());
                                first = false;
                            }
                        } else if (contentType.find("multipart/form-data") != std::string::npos) {
                            auto [body, boundary] = BuildMultipartBody(obj);
                            postData = body;
                            std::string newCT = "multipart/form-data; boundary=" + boundary;
                            if (userProvidedContentType) {
                                for (auto &h: headers) {
                                    if (h.first == "Content-Type") h.second = newCT;
                                }
                            } else {
                                headers.emplace_back("Content-Type", newCT);
                            }
                        }
                    }
                }
                if (args.size() > callbackIdx && args[callbackIdx]->type == ValueType::FUNCTION) {
                    callback = args[callbackIdx];
                }
                EventLoop::AddActiveTask();
                std::thread t([parts, headers, postData, callback, method]() {
                    auto result = SendHttpRequest(parts.host, parts.path, method, postData, parts.scheme == "https", headers);
                    if (callback != nullptr) {
                        EventLoop::Enqueue(callback, {std::move(result)});
                    }
                    EventLoop::RemoveActiveTask();
                });
                t.detach();
                return std::make_shared<NullValue>();
            }
        );
        o->Set(jsName, fn);
    }

public:
    static ValuePtr CreateNetModule() {
        auto module = std::make_shared<ObjectValue>();
        RegisterRequest(module, "get", "GET", false);
        RegisterRequest(module, "delete", "DELETE", false);
        RegisterRequest(module, "post", "POST", true);
        RegisterRequest(module, "put", "PUT", true);
        RegisterRequest(module, "patch", "PATCH", true);
        return module;
    }
};


#endif //BXSCRIPT_NETMODULE_H
