# Buffer 缓冲区

Buffer 为固定大小的二进制数据容器，用于处理原始字节。

## 创建

```javascript
let buf = new Buffer(10);  // 创建 10 字节的空缓冲区（全 0）
```

Buffer 通常由 IO 模块的文件读取或网络请求返回。

```javascript
import std.IO as IO;
let buf = IO.read("data.bin");  // 返回 Buffer
```

## 属性

| 属性 | 类型 | 描述 |
|:-----|:-----|:-----|
| `length` | Number | 缓冲区字节数 |
| `size` | Number | 同 `length` |

## 索引访问

通过数字索引读写每个字节（0-255）。

```javascript
let buf = new Buffer(4);
buf[0] = 65;   // 写入字节 'A'
buf[1] = 66;   // 写入字节 'B'
buf[2];        // 读取：0（未赋值默认为 0）
buf.length;    // 4
```

## 示例

```javascript
import std.IO as IO;
import std.Crypt as Crypt;

// 读取二进制文件
let data = IO.read("image.png");
IO.print("文件大小: " + data.length + " 字节");

// 读取文本文件（指定 UTF-8 编码）
let text = IO.read("config.txt", "utf8");
IO.print(text);

// 计算文件 MD5
let hash = Crypt.md5(text);
IO.print("MD5: " + hash);
```
