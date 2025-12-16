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
 * @brief    加密模块标准库(stbrumme/hash-library)
 */

#ifndef BXSCRIPT_CRYPTMODULE_H
#define BXSCRIPT_CRYPTMODULE_H
#include "evaluator/Logger.h"
#include "evaluator/Value.h"
#include "libs/hmac.h"
#include "libs/md5/md5.h"
#include "libs/sha256/sha256.h"

const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class CryptModule {
    static bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    static uint32_t crc32_bitwise(const std::string &data) {
        uint32_t crc = 0xFFFFFFFF;
        for (unsigned char c: data) {
            crc ^= c;
            for (int i = 0; i < 8; i++) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
        }
        return ~crc;
    }

    static void InitBase64Encode(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<StringValue>("");
                }
                if (args[0]->type != ValueType::STRING) {
                    Logger::Error("参数错误: Crypt.encode([str])");
                }
                auto arg = std::static_pointer_cast<StringValue>(args[0]);
                std::string output;
                int i = 0, j = 0;
                unsigned char char_array_3[3];
                unsigned char char_array_4[4];
                for (auto c: arg->Value) {
                    char_array_3[i++] = c;
                    if (i == 3) {
                        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                        char_array_4[3] = char_array_3[2] & 0x3f;
                        for (i = 0; i < 4; i++)
                            output += base64_chars[char_array_4[i]];
                        i = 0;
                    }
                }
                if (i > 0) {
                    for (j = i; j < 3; j++) {
                        char_array_3[j] = '\0';
                    }
                    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                    char_array_4[3] = char_array_3[2] & 0x3f;
                    for (j = 0; j < i + 1; j++) {
                        output += base64_chars[char_array_4[j]];
                    }
                    while (i++ < 3) {
                        output += '=';
                    }
                }
                return std::make_shared<StringValue>(output);
            });
        o->Set("encode", fn);
    }

    static void InitBase64Decode(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<StringValue>("");
                }
                if (args[0]->type != ValueType::STRING) {
                    Logger::Error("参数错误: Crypt.decode([str])");
                }
                auto arg = std::static_pointer_cast<StringValue>(args[0]);
                std::string output;
                int i = 0, j = 0, in_ = 0;
                unsigned char char_array_4[4];
                unsigned char char_array_3[3];
                size_t length = arg->Value.length();
                while (length-- && arg->Value[in_] != '=' && is_base64(arg->Value[in_])) {
                    char_array_4[i++] = arg->Value[in_];
                    in_++;
                    if (i == 4) {
                        for (i = 0; i < 4; i++) {
                            char_array_4[i] = base64_chars.find(char_array_4[i]);
                        }
                        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
                        for (i = 0; i < 3; i++) {
                            output += char_array_3[i];
                        }
                        i = 0;
                    }
                }
                if (i > 0) {
                    for (j = i; j < 4; j++) {
                        char_array_4[j] = 0;
                    }
                    for (j = 0; j < 4; j++) {
                        char_array_4[j] = base64_chars.find(char_array_4[j]);
                    }
                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
                    for (j = 0; j < i - 1; j++) {
                        output += char_array_3[j];
                    }
                }
                return std::make_shared<StringValue>(output);
            });
        o->Set("decode", fn);
    }

    static void initMd5(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<StringValue>("");
                }
                if (args[0]->type != ValueType::STRING) {
                    Logger::Error("Crypt.md5([str])");
                }
                const auto arg = std::static_pointer_cast<StringValue>(args[0]);
                return std::make_shared<StringValue>(MD5()(arg->Value));
            });
        o->Set("md5", fn);
    }

    static void initSha256(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<StringValue>("");
                }
                if (args[0]->type != ValueType::STRING) {
                    Logger::Error("Crypt.sha256([str])");
                }
                const auto arg = std::static_pointer_cast<StringValue>(args[0]);
                return std::make_shared<StringValue>(SHA256()(arg->Value));
            });
        o->Set("sha256", fn);
    }

    static void initHmac(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.size() != 3) {
                    Logger::Error("参数错误: Crypt.hmac(algo, key, msg)");
                }
                const std::string algo = args[0]->ToString();
                const std::string key = args[1]->ToString();
                const std::string msg = args[2]->ToString();
                std::string result;
                if (algo == "sha256" || algo == "SHA256") {
                    result = hmac<SHA256>(msg, key);
                } else if (algo == "md5" || algo == "MD5") {
                    result = hmac<MD5>(msg, key);
                } else {
                    Logger::Error("不支持的 HMAC 算法: " + algo);
                }
                return std::make_shared<StringValue>(result);
            });
        o->Set("hmac", fn);
    }

    static void InitCRC32(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) return std::make_shared<NumberValue>(0);
                std::string str = args[0]->ToString();
                uint32_t res = crc32_bitwise(str);
                std::stringstream ss;
                ss << std::hex << res;
                return std::make_shared<StringValue>(ss.str());
            });
        o->Set("crc32", fn);
    }

public:
    static ValuePtr CreateCryptModule() {
        auto cryptObj = std::make_shared<ObjectValue>();
        InitBase64Encode(cryptObj);
        InitBase64Decode(cryptObj);
        initMd5(cryptObj);
        initSha256(cryptObj);
        initHmac(cryptObj);
        InitCRC32(cryptObj);
        return cryptObj;
    }
};


#endif //BXSCRIPT_CRYPTMODULE_H
