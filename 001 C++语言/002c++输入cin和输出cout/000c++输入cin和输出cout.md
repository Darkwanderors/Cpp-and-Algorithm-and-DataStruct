# c++输入和输出

c++在c语言的输入、输出函数的基础上进行了封装。这里重点介绍`cin`和`cout`，分别对应c语言的`scanf`和`printf`。

使用c++输入输出有时更方便，不需要像`printf`/`scanf`输入输出时那样，需要手动控制格式。c++的输入输出可以自动识别变量类型。但在操作的数据量比较大时`cin`和`cout`会消耗更多的时间，所以使用再方便也要分情况使用。

## cin和iostream

c可以理解为控制台，in可以理解为输入。因为c++对结构体（在c++有另一个称呼：类）进行了修改，使得`cin`能完成对结构体变量的输入，但要设计特殊的函数。

使用`cin`标准输入对象（键盘）时，必须展开`iostream`头文件以及按命名空间使用方法指定域名`std`。

注意：早期标准库将所有功能在全局域中实现，声明在 .h 后缀的头文件中，使用时只需包含对应头文件即可，后来将其实现在`std`命名空间下，为了和C头文件区分，也为了正确使用命名空间，规定C++头文件不带`.h`；旧编译器(vc 6.0)中还支持`<iostream.h>`格式，后续编译器已不支持，因此推荐使用`<iostream>+std`的方式：

```cpp
#include<iostream>
```

使用参考：

```cpp
void f(){
    int a;float b;double c;char d;
    cin>>a>>b>>c>>d;//这里和scanf是一样的功能
    //scanf("%d%lf%lf%c",&a,&b,&c,&d);
}

void f2(){
    char st[20]={"\0"};//读取字符串
    cin>>st;//不读空格
    cin.getline(st,20);//读取一行的20个字符，包括空格，不包括换行符
    (cin>>st).get();//不读空格，读完后吸收多余换行符
}
```

## 用cin对结构体进行输入

想让结构体也能通过`cin`输入，可以加入函数`operator>>`。`>>`是流提取运算符，从控制台（输入流）提取数据，将数据的值给`>>`后面的变量。

```cpp
#include<iostream>
//using namespace std;//可以将展开命名空间std的语句解除注释

struct A {
	int a; int b;
};

//abc是cin的引用，x是结构体变量的引用
std::istream& operator>>(std::istream& cin, A& x) {
	cin >> x.a >> x.b;
	return cin;//传引用返回
}

int main() {
	A a;
	std::cin >> a;//cin可以理解为istream类的对象
	operator>>(std::cin, a);//显式调用
	printf("%d %d", a.a, a.b);
	return 0;
}
```

其中引用见[c++入门——引用和内联函数-CSDN博客](https://blog.csdn.net/m0_73693552/article/details/145215015)。

关于`operator`，是通过`operator`关键字将c语言的部分运算符也能发挥函数的功能，当然也有添加新的功能。详细见[类和对象——运算符重载_类外重载运算符-CSDN博客](https://blog.csdn.net/m0_73693552/article/details/145499735)。



## cout和endl

`cin`对应c语言的`scanf`，`cout`则对应c语言的`printf`（当然不止`scanf`和`printf`）。

使用`cout`标准输出对象（控制台）和时，也必须展开`iostream`头文件以及按命名空间使用方法指定域名`std`。

使用参考：

```cpp
void f(){
    int a=1;float b=2.2f;
    cout<<a<<b;//输出变量，类似printf
    //printf("%d,%f",a,b);
    
    char st[]="Hello, world.";
    cout<<"Hello, world.";//输出字符串
    cout<<st;//输出字符串
    cout<<endl;//输出换行符endl，endl会刷新流
}

#include<iomanip>
//格式输出字符串
void f2(){
    float b=2.2f;
    cout<<fixed<<setprecision(3)<<b;//按3位小数输出浮点数
    int a=4;
    cout<<step(5)<<a;
}
```

其中`endl`（end of line）。是特殊的C++符号，表示**换行输出**，但在c语言的换行的基础上新增了很多东西，例如刷新输出流等。

和`cin`、`cout`一样，`endl`也要展开`iostream`头文件以及指定命名空间`std`。

输出结构体，同样可以实现`operator<<`函数。`<<`是流插入运算符，将`<<`后的变量输出在控制台。

```cpp
#include<iostream>

struct A {
	int a; int b;
};

std::istream& operator>>(std::istream& abc, A& x) {
	abc >> x.a >> x.b;
	return abc;
}

std::ostream& operator<<(std::ostream& cout, A& x) {
	cout << x.a << x.b;
	return cout;//为支持连续输出，需要将cout返回
}

int main() {
	A a;
	//std::cin >> a;//cin可以理解为istream类的对象
	//operator>>(std::cin, a);//显式调用

	a.a = 3; a.b = 4;
	//printf("%d %d", a.a, a.b);
	std::cout << a<<std::endl;//cout可以理解为ostream类的对象
	operator<<(std::cout, a);
	return 0;
}
```

`operator<<`和`operator>>`也能当成结构体变量的成员进行显式调用，且因为返回值都是`cout`和`cin`的引用，所以支持连续调用。

```cpp
#include<iostream>
using namespace std;

int main() {
	int a, b;
	double c;
	cin.operator>>(a);//可以理解为cin的成员函数，返回值为cin
	cin.operator>>(b).operator>>(c);
	(cout.operator<<(a) << b).operator<<(c);//加()调整优先级
	return 0;
}
```

## 提升cin和cout的输出效率

在算法竞赛中，有时为了`cin`和`cout`的使用便利，和追求和`scanf`、`printf`一样的效率，会在代码中加入`std::ios::sync_with_stdio`提升`cin`和`cout`的运行效率。

```cpp
#include<iostream>

int main() {
	//std::ios::sync_with_stdio(true);//库里默认是true
	std::ios::sync_with_stdio(false);//提升cin和cout效率需要修改为false
	std::cin.tie(0);
	std::cout.tie(0);
	//...
	return 0;
}
```

因为`<<`和`>>`的每次使用都要调用`flush`（详细见[flush - C++ Reference](https://legacy.cplusplus.com/reference/ostream/flush-free/?kw=flush)，后期会单独介绍），所以可以通过`cin`和`cout`的`tie(0)`的调用来进一步加快执行效率。

但使用`sync_with_stdio`后不能再使用`scanf`和`printf`，所以加了`sync_with_stdio`后，使用`cin`和`cout`，还是`scanf`和`printf`需要做出选择。关于`sync_with_stdio`在c++学习后期会单独介绍。





