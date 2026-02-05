# 特殊说明

## IFEE模式

```javascript
import std.IO as io;
(function(){
    io.println("run");
})();
```

## defer (LIFO)
```javascript
import std.IO as io;
function deferTest() {
    let str = "1";
    str += "2";
    defer (function(){
        str += "3";
        io.println(str);
    })();
}
deferTest();
// 输出123
```

## 高阶函数
```javascript
function buildFunc(){
    return function(a, b){
        return a + b;
    }
}
buildFunc()();
```

## 原型扩展（Prototype Extension）
```javascript
String.prototype.sayHi = function() {
    return "Hi, " + this;
};
"fox".sayHi(); //输出Hi, fox
```

## 并发模型

### EventLoop 机制说明
线程、http请求回调均通过EventLoop分发到主线程执行,回调时机取决于消息模型,

### Thread 模块与消息传递

