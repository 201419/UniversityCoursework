# LispToC
《编程语言》实验二【编程语言分析及应用】

------

## Lisp格式到C格式的转换
Lisp的函数调用格式为： (函数名 参数1 参数2 … 参数n)

C的函数调用格式为：函数名(参数1, 参数2, …, 参数n)

现有五个整数的二元函数add、sub、mul、div、mod和一些使用这些函数的Lisp格式表达式，请实现一个程序转换器，将这些Lisp格式表达式转成C格式的表达式。

------

## Lisp格式表达式的计算

Lisp的函数调用格式为： (函数名 参数1 参数2 … 参数n)

现有五个整数的二元函数add、sub、mul、div、mod，请实现一个计算器，计算这些Lisp格式表达式。

------

## Lisp格式表达式的类型检测

Lisp的函数调用格式为： (函数名 参数1 参数2 … 参数n)

现有两个类型：整数int和布尔类型bool，以及五个整数的二元函数add、sub、gt、lt、equ和三个布尔值的函数and、or、not，请实现一个类型检测器，并检测这些Lisp格式表达式。
