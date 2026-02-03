# IO 模块

提供文件读写、控制台输入输出及文件系统操作功能。

使用前请导入：
```javascript
import std.IO as IO;
```

## 控制台操作

| 方法                | 描述            | 示例                              |
|:------------------|:--------------|:--------------------------------|
| `print(arg...)`   | 打印内容到控制台（不换行） | `IO.print("Hello")`             |
| `println(arg...)` | 打印内容到控制台并换行   | `IO.println("Line 1")`          |
| `input(prompt)`   | 获取用户控制台输入     | `let name = IO.input("Name: ")` |

## 文件系统操作

| 方法                       | 描述                                 | 示例                                      |
|:-------------------------|:-----------------------------------|:----------------------------------------|
| `read(path, [encoding])` | 读取文件内容。默认返回 Buffer，指定 "utf8" 返回字符串 | `let txt = IO.read("test.txt", "utf8")` |
| `write(path, content)`   | 将字符串或 Buffer 写入文件                  | `IO.write("log.txt", "Data")`           |
| `exist(path)`            | 判断路径是否存在                           | `if (IO.exist("file.txt")) ...`         |
| `isFile(path)`           | 判断是否为文件                            | `IO.isFile("test.txt")`                 |
| `isDir(path)`            | 判断是否为目录                            | `IO.isDir("libs")`                      |
| `mkdir(path)`            | 创建目录（支持递归）                         | `IO.mkdir("a/b/c")`                     |
| `remove(path)`           | 删除文件或目录（递归）                        | `IO.remove("temp_dir")`                 |
| `copy(src, dst)`         | 复制文件                               | `IO.copy("a.txt", "b.txt")`             |
| `rename(old, new)`       | 重命名/移动文件                           | `IO.rename("old.txt", "new.txt")`       |
| `list(path)`             | 列出目录下所有文件（返回对象数组）                  | `let files = IO.list(".")`              |
| `abs(path)`              | 获取绝对路径                             | `IO.abs("./")`                          |
| `attr(path)`             | 获取文件属性（大小等）                        | `let info = IO.attr("test.txt")`        |