#### 1. 基础与控制流
```javascript
let sum = 0;
for (let i = 0; i < 10; i++) {
    if (i % 2 == 0) {
        continue;
    }
    sum = sum + i;
}
// 支持 += 等复合运算
sum += 100;
```
#### 2. 闭包与高阶函数
```javascript
function makeAdder(x) {
    return function(y) {
        return x + y;
    };
}

let add5 = makeAdder(5);
print(add5(10)); // 输出 15
```

#### 3. 数组与原生方法
```javascript
let arr = [];
arr.push("Hello");
arr.push("World");

print(arr.length); // 2
print(arr[0] + " " + arr[1]); // Hello World
```
#### 4. 中文与字符串处理
```javascript
let s = "你好世界";
print(s[0]); // 输出: "你" (底层自动处理 UTF-8)
print(s.indexOf("世")); // 输出: 2
```

#### 5. 模块导入
```javascript
// 自动搜索 ./lib/math.bx 或相对路径
import std.math as m; 

let r = m.add(10, 20);
```