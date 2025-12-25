/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/25
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    TextureKit
 */
#ifndef BXSCRIPT_TEXTUREKIT_H
#define BXSCRIPT_TEXTUREKIT_H
#include <map>
#include <string>
#include <GL/gl.h>
#include "libs/stb_image.h"


class TextureKit {
    inline static std::map<std::string, GLuint> TextureCache;

public:
    static GLuint Load(const std::string &path) {
        if (TextureCache.find(path) != TextureCache.end()) {
            return TextureCache[path];
        }
        int w, h, n;
        unsigned char *data = stbi_load(path.c_str(), &w, &h, &n, 0);
        if (!data) {
            return 0;
        }
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        const GLenum format = (n == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        TextureCache[path] = tex;
        return tex;
    }
};


#endif //BXSCRIPT_TEXTUREKIT_H
