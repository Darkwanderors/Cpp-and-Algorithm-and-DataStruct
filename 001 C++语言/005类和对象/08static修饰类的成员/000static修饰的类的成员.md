# static成员

有时会有这样的需求：计算程序中创建出了多少个类的对象，以及多少个正在使用的对象。

因为构造函数和析构函数都只会调用一次，所以可以通过设置生命周期和`main`函数一致的计数变量进行统计。

计数变量用全局变量还会有别的问题：c++讲究封装，用全局变量可能会被不明因素修改。

```cpp
#include<iostream>
#include<cstdlib>
int n, m;

class A {
public:
	A() {
		++n;
		++m;
	}
	A(A& a) {
		++n;
		++m;
	}
	~A() {
		--m;
	}
private:
};

A f(A a) {
	return a;
}

A& f1(A a) {//传引用返回
	return a;
}

//A& f1(A& a) {//传引用返回，形参也是引用
//	return a;
//}

int main() { 
	A a1;
	A a2;
	using std::cout;

	cout << n << " " << m << "\n";

	//++n;++m;//外部修改

	A();

	cout << n << " " << m << "\n";

	f(a1);
	f1(a1);

	cout << n << " " << m << "\n";

	return 0;
}
```

此时我们希望计数变量和类绑定，但又不想这些计数变量和成员函数一样属于公有。

## static成员的概念

声明为`static`的类成员称为**类的静态成员**，用`static`修饰的**成员变量**，称之为**静态成员变量**；用**static修饰的成员函数**，称之为**静态成员函数**。

**静态成员变量一定要在类外进行初始化**。因为静态成员变量属于类，所以并不通过某一对象的初始化列表（构造函数）进行初始化，需要声明和定义分离。

通过`static`成员变量对创建的对象个数进行统计：

```cpp
#include<iostream>
using namespace std;

class A {
public:
    A() { ++_scount; }
    A(const A& t) { ++_scount; }
    ~A() { --_scount; }
    static int GetACount() { return _scount; }
private:
    static int _scount;//声明
};
int A::_scount = 0;//定义

int main() {
    cout << A::GetACount() << endl;
    A a1, a2;
    A a3(a1);
    A a4 = a1;
    cout << A::GetACount() << endl;
    return 0;
}
```

## 特性

1. **静态成员**为**所有类对象所共享**，不属于某个具体的对象，存放在静态区。这就意味着空指针也能访问静态成员变量，和空指针能访问成员函数是一样的道理：看访问的数据在哪。

2. **静态成员变量**必须在**类外定义**，定义时不添加`static`关键字，类中只是声明。

3. 类静态成员即可用`类名::静态成员`或者`对象.静态成员`、`类的指针->静态成员`来访问。
   例如这个案例通过`NULL`来访问静态成员。

```cpp
#include<iostream>
using namespace std;

class A {
public:
    A() { ++_scount; }
    A(const A& t) { ++_scount; }
    ~A() { --_scount; }
    static int GetACount() { return _scount; }
    static int _scount;//声明
private:
};
int A::_scount = 0;//定义

int main() {
    cout << A::GetACount() << endl;
    A a1, a2;
    A a3(a1);
    A a4 = a1;
    cout << ((A*)(NULL))->_scount << endl;
    //将NULL强制转换为A的指针用于访问公有成员
    return 0;
}
```

4. 静态成员函数**没有**隐藏的`this`指针，不能访问任何非静态成员。
   这就意味着静态成员函数不可调用非静态成员函数，
   但非静态成员函数可以调用类的静态成员函数。
   而且因为没有`this`指针，也就不需要访问构造函数，对其他的静态成员变量不构成影响。

5. 静态成员也是类的成员，受`public`、`protected`、`private` 访问限定符的限制。

6. 静态成员可以是数组，指针，但都要初始化。指针若存在使用`alloc`系列函数或关键字`new`申请空间，则需要在`main`结束前显式释放，或通过全局对象的析构函数进行空间释放。

```cpp
#include<iostream>
#include<cstdlib>
using namespace std;

class A {
public:
	~A() {
		if (c)//需要加语句判断，防止被局部对象造成段错误
			free(c);
		c = NULL;
	}
	static int a[];
	static int b[10];
	static int* c;
};

int A::a[] = { 6 };
//int A::b[6] = { 1,3,5,2 };//元素数量不匹配不给编译通过
int A::b[10] = { 0 };
int* A::c = (int*)malloc(4 * 10);

A a;//全局对象调用析构函数

int main() {
	A a;
	if (A::c) {
		free(A::c);//手动释放
		A::c = NULL;
	}
	return 0;
}
```

补充：和普通变量一样，用`static`修饰的实例化的对象和`static`修饰的内置类型变量拥有同样的属性：

* `static` 局部对象：只要调用这个函数，则这个对象一直是同一个。
* `static` 全局对象：限制可见性，只有同属一个 cpp 的函数才能访问。

`static`成员的应用可以通过这个OJ体现：[求1+2+3+...+n，要求不能使用乘除法、for、while、if、else、switch、case等关键字及条件判断语句（A?B:C)](https://www.nowcoder.com/practice/7a0da8fc483247ff8800059e12d7caf1?tpId=13&tqId=11200&tPage=3&rp=3&ru=/ta/coding-interviews&qru=/ta/coding-interviews/question-ranking) ，这个OJ将公式、位运算和递归都限制了。

但可以通过设置类的静态成员变量，并生成具有连续空间的数组来进行求和。

参考程序1：

```cpp
class A{
public:
    A(){
        ++n;
        sum+=n;
    }
    static int n;
    static int sum;
};
int A::n=0;
int A::sum=0;

class Solution {
public:
    int Sum_Solution(int n) {
        A a[n];
        return A::sum;
    }
};
```

Nowcoder平台支持变长数组，所以可以这样声明数组：`A a[n]`。若平台不支持，还可以通过`new`关键字生成对象数组：

```cpp
class A{
public:
    A(){
        ++n;
        sum+=n;
    }
    static int n;
    static int sum;
};
int A::n=0;
int A::sum=0;

class Solution {
public:
    int Sum_Solution(int n) {
        A* a=new A[n];
        int ans=A::sum;
        free(a);
        return ans;
    }
};
```

这里用`new`不用`malloc`函数或`calloc`函数，是因为后者申请内存时并不调用类的构造函数。