# String 字符串

BxScript 字符串为 UTF-8 编码，内部基于 `u32string` 处理 Unicode 字符。通过数字索引可访问单个字符（UTF-32 code point）。

## 属性

| 属性 | 类型 | 描述 |
|:-----|:-----|:-----|
| `length` | Number | 字符串的 Unicode 字符数 |

```javascript
"Hello 世界".length; // 7
```

## 方法

### `indexOf(search)`
查找子串首次出现的位置。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `search` | String | 要查找的子串 |

| 返回值 | 描述 |
|:-------|:-----|
| Number | 首次出现的索引（从 0 开始），未找到返回 `-1` |

```javascript
"HELLO".indexOf("L");  // 2
"HELLO".indexOf("X");  // -1
```

### `lastIndexOf(search, [fromIndex])`
查找子串最后一次出现的位置，从后往前搜索。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `search` | String | 要查找的子串 |
| `fromIndex` | Number | 可选，搜索起始位置（从该位置往前搜索），默认末尾 |

| 返回值 | 描述 |
|:-------|:-----|
| Number | 最后一次出现的索引，未找到返回 `-1` |

```javascript
"HELLO".lastIndexOf("L");     // 3
"HELLO".lastIndexOf("L", 2);  // 2
```

### `split(separator)`
按分隔符拆分字符串为数组。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `separator` | String | 分隔符。为空字符串时按字符拆分；省略时返回 `[self]` |

| 返回值 | 描述 |
|:-------|:-----|
| Array | 拆分后的字符串数组 |

```javascript
"a,b,c".split(",");   // ["a", "b", "c"]
"hello".split("");    // ["h", "e", "l", "l", "o"]
```

### `startsWith(search)`
检测字符串是否以指定子串开头。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `search` | String | 要检测的前缀 |

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否以 `search` 开头 |

```javascript
"hello".startsWith("he");  // true
"hello".startsWith("lo");  // false
```

### `endsWith(search)`
检测字符串是否以指定子串结尾。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `search` | String | 要检测的后缀 |

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否以 `search` 结尾 |

```javascript
"hello".endsWith("lo");  // true
"hello".endsWith("he");  // false
```

### `replace(search, replacement)`
替换字符串中**所有**匹配的子串。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `search` | String | 被替换的子串 |
| `replacement` | String | 替换后的新子串 |

| 返回值 | 描述 |
|:-------|:-----|
| String | 替换后的新字符串，原字符串不变 |

```javascript
"HELLO".replace("L", "X");   // "HEXXO"
"hello hello".replace("he", "HA"); // "HAllo HAllo"
```

### `toLower()`
将所有 ASCII 大写字母转为小写。

| 返回值 | 描述 |
|:-------|:-----|
| String | 转换后的新字符串 |

```javascript
"HELLO World".toLower();  // "hello world"
```

### `toUpper()`
将所有 ASCII 小写字母转为大写。

| 返回值 | 描述 |
|:-------|:-----|
| String | 转换后的新字符串 |

```javascript
"hello world".toUpper();  // "HELLO WORLD"
```

### `trim()`
去除字符串首尾的所有空白字符（包括空格、制表符、换行符、不间断空格等全部 Unicode 空白）。

| 返回值 | 描述 |
|:-------|:-----|
| String | 去除首尾空白后的新字符串 |

```javascript
"  hello  ".trim();        // "hello"
"\t\n hello \r\n".trim();  // "hello"
```

### `charCodeAt(index)`
获取指定位置字符的 Unicode 码点值。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `index` | Number | 字符索引（从 0 开始） |

| 返回值 | 描述 |
|:-------|:-----|
| Number | 该位置字符的 Unicode 码点，越界返回 `NaN` |

```javascript
"ABC".charCodeAt(0);  // 65
"ABC".charCodeAt(9);  // NaN
```

### `substr(start, end)`
截取子串。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `start` | Number | 起始索引（含） |
| `end` | Number | 结束索引（含） |

| 返回值 | 描述 |
|:-------|:-----|
| String | 截取的子串 |

```javascript
"Hello World".substr(0, 4);  // "Hello"
"Hello World".substr(6, 10); // "World"
```

## 静态方法

### `String.fromCharCode(code1, code2, ...)`
将一个或多个 Unicode 码点转为字符串。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `code...` | Number | 一个或多个 Unicode 码点值 |

| 返回值 | 描述 |
|:-------|:-----|
| String | 由指定码点组成的字符串 |

```javascript
String.fromCharCode(72, 69, 76, 76, 79); // "HELLO"
String.fromCharCode(20320, 22909);       // "你好"
```
