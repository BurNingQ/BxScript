# Buffer 缓冲区

Buffer 为固定大小的二进制数据容器，用于处理原始字节。

## 创建

用 `Buffer` 上的两个静态方法创建（本语言**没有 `new` 关键字**）：

| 方法 | 参数 | 返回 |
|:-----|:-----|:-----|
| `Buffer.alloc(size)` | Number | `size` 字节的空缓冲区，全部为 0 |
| `Buffer.from(str)` | String | 按 **UTF-8 字节**展开，如 `"中文"` 得到 6 字节 |
| `Buffer.from(arr)` | Number 数组 | 每个元素作为一个字节（0-255） |

```javascript
let a = Buffer.alloc(10);         // 10 字节，全 0
let b = Buffer.from("abc");       // 3 字节：97, 98, 99
let c = Buffer.from("中文");       // 6 字节（UTF-8，每字 3 字节）
let d = Buffer.from([1, 2, 255]); // 3 字节

a.length;   // 10
b[0];       // 97  ('a')
```

Buffer 也常由 IO 模块的文件读取或网络请求返回：

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
let buf = Buffer.alloc(4);
buf[0] = 65;   // 写入字节 'A'
buf[1] = 66;   // 写入字节 'B'
buf[2];        // 读取：0（未赋值默认为 0）
buf.length;    // 4
```

## 示例

```javascript
import std.IO as IO;
import std.Crypt as Crypt;
import std.Console as Console;

// 读取二进制文件
let data = IO.read("image.png");
Console.print("文件大小: " + data.length + " 字节");

// 读取文本文件（指定 UTF-8 编码）
let text = IO.read("config.txt", "utf8");
Console.print(text);

// 计算文件 MD5
let hash = Crypt.md5(text);
Console.print("MD5: " + hash);
```
