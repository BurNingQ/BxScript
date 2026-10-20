# Type 类型检查

运行时类型检测模块。

使用前请导入：
```javascript
import std.Type as Type;
```

## 函数

### `of(val)`
返回值的类型名称字符串。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `val` | any | 任意值 |

| 返回值 | 描述 |
|:-------|:-----|
| String | 类型名称 |

```javascript
Type.of(42);            // "number"
Type.of("hello");       // "string"
Type.of(true);          // "boolean"
Type.of([]);            // "array"
Type.of({});            // "object"
Type.of(function(){});  // "function"
Type.of(alert);         // "native function"
Type.of(null);          // "null"
```

### `isNumber(val)`
检测是否为数值。

```javascript
Type.isNumber(3.14);   // true
Type.isNumber("3");    // false
```

### `isString(val)`
检测是否为字符串。

```javascript
Type.isString("abc");  // true
```

### `isBool(val)`
检测是否为布尔值。

```javascript
Type.isBool(true);     // true
```

### `isNull(val)`
检测是否为 `null`。

```javascript
Type.isNull(null);     // true
```

### `isArray(val)`
检测是否为数组。

```javascript
Type.isArray([1, 2]);  // true
```

### `isObject(val)`
检测是否为对象。

```javascript
Type.isObject({});     // true
Type.isObject(null);   // false
```

### `isFunction(val)`
检测是否为函数（包括原生函数）。

```javascript
Type.isFunction(function(){});  // true
Type.isFunction(alert);         // true
```

### `isNativeFunction(val)`
检测是否为原生函数（C++ 实现）。

```javascript
Type.isNativeFunction(alert);          // true
Type.isNativeFunction(function(){});   // false
```

### `isBuffer(val)`
检测是否为 Buffer。

```javascript
Type.isBuffer(Buffer.alloc(8));  // true
```
