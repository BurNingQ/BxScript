# Object 对象

BxScript 对象为键值对集合，键为字符串，值为任意类型。支持属性读写、getter/setter 代理和原型链。

## 创建对象

```javascript
let obj = { name: "Alice", age: 30 };
let empty = {};  // 空对象
```

## 属性读写

```javascript
let obj = { x: 1, y: 2 };
obj.x;         // 1 （点号访问）
obj["y"];      // 2 （括号访问）
obj.z = 3;     // 新增属性
obj["w"] = 4;  // 新增属性
```

## 静态方法

### `Object.keys(obj)`
返回对象所有自有属性名的数组。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `obj` | Object | 目标对象 |

| 返回值 | 描述 |
|:-------|:-----|
| Array | 属性名字符串数组 |

```javascript
let obj = { a: 1, b: 2, c: 3 };
Object.keys(obj);  // ["a", "b", "c"]
```

### `Object.remove(obj, key1, key2, ...)`
删除对象的一个或多个属性。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `obj` | Object | 目标对象 |
| `key...` | String | 要删除的属性名 |

```javascript
let obj = { a: 1, b: 2, c: 3 };
Object.remove(obj, "a", "c");
Object.keys(obj);  // ["b"]
```

### `Object.defineGetter(obj, attr, fn)`
为对象属性定义 getter（访问器）。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `obj` | Object | 目标对象 |
| `attr` | String | 属性名 |
| `fn` | Function | getter 函数，无参数，返回属性值 |

```javascript
let obj = { firstName: "Alice", lastName: "Wang" };
Object.defineGetter(obj, "fullName", function() {
    return this.firstName + " " + this.lastName;
});
obj.fullName;  // "Alice Wang"
```

### `Object.defineSetter(obj, attr, fn)`
为对象属性定义 setter（修改器）。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `obj` | Object | 目标对象 |
| `attr` | String | 属性名 |
| `fn` | Function | setter 函数，接收一个参数（新值） |

```javascript
let obj = { _value: 0 };
Object.defineSetter(obj, "value", function(val) {
    this._value = val * 2;
});
obj.value = 5;
obj._value;  // 10
```
