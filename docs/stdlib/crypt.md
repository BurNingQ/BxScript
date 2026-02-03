# Crypt 模块

提供常用的哈希、加密和编码算法。

使用前请导入：
```javascript
import std.Crypt as Crypt;
```

## 编码
*   `encode(str)`: Base64 编码
*   `decode(str)`: Base64 解码

## 哈希算法
*   `md5(str)`: 计算 MD5 摘要
*   `sha256(str)`: 计算 SHA256 摘要
*   `crc32(str)`: 计算 CRC32 值 (返回 Hex 字符串)

## 消息认证
*   `hmac(algo, key, msg)`: 计算 HMAC。
*   `algo`: "md5" 或 "sha256"

```javascript
let sign = Crypt.hmac("sha256", "my-secret-key", "hello data");
```