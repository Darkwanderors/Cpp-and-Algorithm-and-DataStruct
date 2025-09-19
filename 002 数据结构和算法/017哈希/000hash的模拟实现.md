# 哈希表的模拟实现

> 学习c++，最大的乐趣个人认为是搓天搓地搓空气，包括底层的操作系统也是由和c++很接近的c语言和汇编语言等搓出来的（不排除后期可能有别的）。

## 闭散列除留取余法

这里选用**开放定址法**和**闭散列**的线性探测。但需要用枚举或其他变量来表示状态标识，因为线性探测在原来的位置满了的时候会向后探测找空位，需要根据状态来进行判断空位是否真的是否为空。

根据[红黑树的模拟实现](https://blog.csdn.net/m0_73693552/article/details/149614728?spm=1001.2014.3001.5501)的经验，这里将通过模板参数来推演 Key 模型和 Key-Value 模型。

```cpp
template<class Key,class Type,
class KeyOfType = mapKeyOfType<Key, Type>,
class Hash = HashFunc<Key> >
class HashTable {
private:
    vector<HashData<Type> >tables;
    size_t size;
    KeyOfType kot;
    Hash hashfunc;
    typedef HashData<Type> HashData;
};
```

其中的模板参数：

`Key`：查找哈希地址的键的类型。

`Type`：存储的数据的类型。

`KeyOfType`：从`Type`类型数据中获得`Key`型键值的仿函数。

`Hash`：通过`Key`型键值查找哈希地址的哈希函数。

### 查找、插入和删除

1. 查找

查找时根据`Key`型键值来查找。获取哈希地址后，要查找的数据可能通过开放寻址法放到其他空位，所以需要从获取的哈希地址向后查找。

2. 插入

首先查找要插入的数据是否存在，不存在就不插入。

然后检测负载因子。若负载因子大于 0.7 （推荐设置为0.7），则扩容。扩容时推荐创建新表，将旧表的数据重新插入新表，再删除旧表。

最后就是通过哈希函数找到哈希地址，从哈希地址开始通过线性探测寻找空位，找到后进行插入。

3. 删除

首先查找要删除的数据是否存在，不存在直接返回。

找到要删除的数据后，将状态设置为`EMPTY`即可。

### 闭散列参考程序

因为闭散列造成数据堆积的情况相对严重，因此只适合数据量小的情况，闭散列也不是重点，这里给 Key 模型实现参考用于复习即可。

```cpp
#pragma once
#include<string>
#include<vector>
#include<algorithm>
#include<iostream>
using std::string;
using std::vector;
using std::lower_bound;
using std::cout;
using std::pair;
using std::make_pair;

//开放寻址法 + 除留取余法 的哈希表
namespace open_adress {
	enum State {
		EMPTY,
		EXIST,
		DELETE
	};

	template<class Key, class Type>
	struct mapKeyOfType {
		const Key& operator()(const Type& aim) {
			return aim.first;
		}
	};

	template<class Key>
	struct HashFunc {
		size_t operator()(const Key& aim) {
			return size_t(aim);
		}
	};

	template<>
	struct HashFunc<string> {
		size_t operator()(const string& aim) {
			size_t val = 0;
			for (auto& x : aim)
				val = val * 131 + x;
			return val;
		}
	};

	template<class Type>
	struct HashData {
		Type data;
		State state;
		HashData(const Type& data = Type(), const State& state = EMPTY)
			:data(data)
			, state(state) {}
		HashData(const HashData& aim)
			:data(aim.data)
			, state(aim.state) {}
	};

	inline unsigned long __stl_next_prime(unsigned long n) {
		// Note: assumes long is at least 32 bits.
		static const int __stl_num_primes = 28;
		static const unsigned long __stl_prime_list[__stl_num_primes] = {
			53, 97, 193, 389, 769,
			1543, 3079, 6151, 12289, 24593,
			49157, 98317, 196613, 393241, 786433,
			1572869, 3145739, 6291469, 12582917, 25165843,
			50331653, 100663319, 201326611, 402653189, 805306457,
			1610612741, 3221225473, 4294967291
		};
		const unsigned long* first = __stl_prime_list;
		const unsigned long* last = __stl_prime_list + __stl_num_primes;
		const unsigned long* pos = lower_bound(first, last, n);
		return pos == last ? *(last - 1) : *pos;
	}

	template<class Key,
		class Type,
		class KeyOfType = mapKeyOfType<Key, Type>,
		class Hash = HashFunc<Key> >
		class HashTable {
		private:
			vector<HashData<Type> >tables;
			size_t size;
			KeyOfType kot;
			Hash hashfunc;
			typedef HashData<Type> HashData;
		public:
			HashTable(const size_t& capacity = __stl_next_prime(0)) {
				tables.resize(capacity);
				size = 0;
			}

			bool insert(const Type& data) {
				if (find(kot(data)))
					return false;
				// 负载因子0.7就扩容
				if (size * 10 / tables.size() >= 7) {
					HashTable<Key, Type, KeyOfType>newht
					(__stl_next_prime(tables.size() + 1));
					for (auto& x : tables)
						if (x.state == EXIST)
							newht.insert(x.data);
					tables.swap(newht.tables);
				}
				size_t hashi = hashfunc(kot(data)) % tables.size();
				while (tables[hashi].state == EXIST) {
					hashi++;
					hashi %= tables.size();
				}
				tables[hashi].data = data;
				tables[hashi].state = EXIST;
				++size;
				return true;
			}

			HashData* find(const Key& aim) {
				size_t hashi = hashfunc(aim) % tables.size();
				while (tables[hashi].state != EMPTY) {
					if (kot(tables[hashi].data) == aim)
						return &tables[hashi];
					hashi = (hashi + 1) % tables.size();
				}
				return nullptr;
			}

			bool erase(const Key& aim) {
				HashData* ret = find(aim);
				if (ret) {
					ret->state = DELETE;
					return true;
				}
				return false;
			}

		public:
			void print() {
				for (auto& x : tables)
					if (x.state == EXIST)
						cout << x.data << ',';
				cout << "\n";
			}

			void mapprint() {
				for (auto& x : tables)
					if (x.state == EXIST)
						cout << "[" << x.data.first << ':' << x.data.second << "] ";
				cout << "\n";
			}
	};

    //测试用的函数，一起打包在了命名空间中
	template<class Key, class Type>
	struct setKeyOfData {
		const Key& operator()(const Type& aim) {
			return aim;
		}
	};

	void fset() {
		srand(size_t(time(0)));
		HashTable<int, int, setKeyOfData<int, int> >ht;
		vector<int>a = {
			94,98,2,2,97,5,15,18,19,20,
			22,27,27,30,33,34,38,38,42,
			46,48,60,61,66,70,71,73,73,
			77,85,85,87,88,90,91,91,93,
			94,95,95 };
		for (auto& x : a)
			ht.insert(x);
		ht.print();
		cout << "\n";
		ht.erase(61);
		ht.print();
		cout << "\n";
		ht.insert(61);
		ht.print();
	}

	void fmap() {
		HashTable<string, pair<string, string> >ht;
		ht.insert(make_pair("insert", "插入"));
		ht.insert(make_pair("erase", "删除"));
		ht.insert(make_pair("hash", "哈希"));
		ht.mapprint();
	}
}
```



## 开散列除留取余法（数组+链表）

开散列的哈希表实际上是一个支持下标索引的数组，每个元素存储一个或一组容器。当发生哈希冲突时，将同一哈希函数得到的哈希地址相同的数据放在同一容器中。这个容器称哈希桶。

哈系统可以是链表，也可以是红黑树，还可以是链表和红黑树的组合，反正就是哪个数据的查询快就用哪个。

这里的开散列哈希表只用链表实现，这样的哈希表相对容易实现。但当冲突的数据越来越多时，链表会变得很长，遍历的效率会降低。因此不作为实际应用时的参考。

参考原型：

```cpp
inline unsigned long __stl_next_prime(unsigned long n) {
	// Note: assumes long is at least 32 bits.
	static const int __stl_num_primes = 28;
	static const unsigned long __stl_prime_list[__stl_num_primes] = {
		53, 97, 193, 389, 769,
		1543, 3079, 6151, 12289, 24593,
		49157, 98317, 196613, 393241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843,
		50331653, 100663319, 201326611, 402653189, 805306457,
		1610612741, 3221225473, 4294967291
	};
	const unsigned long* first = __stl_prime_list;
	const unsigned long* last = __stl_prime_list + __stl_num_primes;
	const unsigned long* pos = lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}

template<class Key, class Type,
	class KeyOfType, class Hash = HashFunc<Key> >
	class HashTable {
private:
	vector<list<Type>>tables;
	size_t size;
    KeyOfType kot;
    Hash hashfunc;
public:
	HashTable(size_t capacity = __stl_next_prime(0)) {
		tables.resize(capacity);
		size = 0;
	}

HashTable(const HashTable<Key, Type, KeyOfType, Hash>& aim)
	:tables(aim.tables)
	, size(aim.size)
	, kot(aim.kot)
	, hashfunc(aim.hashfunc){}

};
```

`__stl_next_prime`函数是从c++98的`stl_hashtable`中获得。因为库中是使用素数作为哈希表的容量实现，所以这里选择借鉴。

`tables`：哈希表本体，`vector`数组代替。

`size`：统计元素个数。

`kot`：用于从`Type`类型中获取键值。

`hashfunc`：用于将`Key`型键值转化为哈希地址。



### 迭代器

因为决定了使用`list`作为哈希桶的容器，所以迭代器最好使用`list`的迭代器。然后迭代器需要访问哈希表以及指定位置的哈希桶。因此迭代器需要包含3个成员：`list`迭代器、哈希表和哈希桶。

因为哈希表直接拷贝的的话开销可能很大，所以迭代器内包含哈希表的指针，通过指针访问哈希表即可。

在实现过程中我遇到或一个问题：参考[c++STL-list的模拟实现-CSDN博客](https://blog.csdn.net/m0_73693552/article/details/147914422)，我将迭代器也设置成常量迭代器和非常量迭代器用同一个迭代器模板类推导得到，但遇到很多常属性迭代器（特别是`list`的迭代器）和非常属性的迭代器之间互相赋值等越权的情况。

针对这种情况，需要将迭代器类型和哈希表指针类型也作为模板参数，也可以使用条件编译选择类型（需要编译器支持c++11，且操作繁琐，不如给模板参数方便）。

因此迭代器原型参考：
```cpp
template<class Key, class Type, class KeyOfType,
	class Ref, class Ptr, class Hash,
	class listiterator
	,class vectorptr>
	struct HashIterator {

	listiterator it;//list的迭代器
	vectorptr pt;//哈希表的指针
	size_t pb;//哈希桶
        
	typedef HashIterator<Key, Type, KeyOfType,
        Ref, Ptr, Hash,
		listiterator, vectorptr> self;

	HashIterator(listiterator it, vectorptr pt,
		size_t pb)
		:it(it)
		, pt(pt)
		, pb(pb) {}

	HashIterator(const self& aim)
		:it(aim.it)
		, pt(aim.pt)
		, pb(aim.pb) {}
};
```

由此也就衍生出了一个经验（个人认为）：**用其他容器的迭代器来实现本容器的迭代器**，**可以上传迭代器和容器的数据类型作为模板参数**。

之后若迭代器不带常属性，则在容器中这样定义：
```cpp
typedef HashIterator<Key, Type, KeyOfType, 
Type&, Type*, Hash,
typename list<Type>::iterator,
vector<list<Type>>*>
    iterator;
```

`typename`是让编译器将迭代器识别为类型。

若迭代器带常属性：

```cpp
typedef HashIterator<Key, Type, KeyOfType,
const Type&, const Type*, Hash,
typename list<Type>::const_iterator, 
const vector<list<Type>>*>
    const_iterator;
```

回忆迭代器要求的几个功能，根据哈希表来实现：

1. 支持对迭代器解引用操作。在这里的实现中直接返回`*it`即可。
2. 支持对迭代器的`->`操作。在这里的实现中直接返回`&(*it)`即可，即对迭代器解引用获得数据后，再取地址返回。
3. 支持`++`。先`++it`，若`it`为当前哈希桶的尾迭代器，则查找下一个非空哈希桶，找不到再重置为`end()`迭代器。
4. 支持`==`。判断哈希桶是否为同一个，以及迭代器`it`是否相同即可。

其他的根据需要自行补充。

最后：若迭代器无法访问哈希表，则哈希表类可能需要将迭代器设置为友元以及前置声明。声明参考c语言的声明，可以不实现，而是放一个模板参数和不带类体的类声明语句。

### 查找和删除

通过哈希函数的到要查找的目标（`aim`）的哈希地址，然后遍历哪个位置的哈希桶即可。找不到返回尾迭代器。

删除需要先找到目标是否存在，不存在的话可以报错，也可以返回尾迭代器；存在的话通过链表的`erase`方法删除迭代器（可能要使用标准库的`std::find`工具，需展开`algorithm`头文件）。

成功删除迭代器后，还需检查当前哈希桶内是否还有元素，没有元素的话需要向后查找第1个非空哈希桶，将非空哈希桶的首元素迭代器返回。

否则返回尾迭代器表示删除失败。

### 插入

首先查找要插入的元素是否在哈希表中。在的话就返回迭代器和`false`组成的键值对。这样做是因为容器`unordered_map`需要借助`insert`方法来实现`operator[]`的操作。

然后检查负载因子，为1时扩容。开散列不需要和闭散列一样做的太过严格。

最后就是老流程：通过哈希函数获取哈希地址，将数据插入指定地址的链表中。

### 开散列参考程序

这里只实现了迭代器、插入、查找和删除的基本功能。其他的函数是为了哈希表能做`unordered_map`和`unordered_set`顺手实现的。

```cpp
#pragma once
#include<string>
#include<vector>
#include<algorithm>
#include<iostream>
#include<list>
#include<set>
#include<cassert>
using std::string;
using std::vector;
using std::lower_bound;
using std::cout;
using std::pair;
using std::make_pair;
using std::list;
using std::set;

template<class Key>
struct HashFunc {
	size_t operator()(const Key& aim) {
		return size_t(aim);
	}
};

template<>
struct HashFunc<string> {
	size_t operator()(const string& aim) {
		size_t val = 0;
		for (auto& x : aim)
			val = val * 131 + x;
		return val;
	}
};

inline unsigned long __stl_next_prime(unsigned long n) {
	// Note: assumes long is at least 32 bits.
	static const int __stl_num_primes = 28;
	static const unsigned long __stl_prime_list[__stl_num_primes] = {
		53, 97, 193, 389, 769,
		1543, 3079, 6151, 12289, 24593,
		49157, 98317, 196613, 393241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843,
		50331653, 100663319, 201326611, 402653189, 805306457,
		1610612741, 3221225473, 4294967291
	};
	const unsigned long* first = __stl_prime_list;
	const unsigned long* last = __stl_prime_list + __stl_num_primes;
	const unsigned long* pos = lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}

//迭代器实现需要将其他容器的迭代器对象作为成员
template<class Key, class Type, class KeyOfType,
	class Ref, class Ptr, class Hash,
	class listiterator
	, class vectorptr>
	struct HashIterator {

	listiterator it;//list的迭代器
	vectorptr pt;//哈希表的指针
	size_t pb;//哈希桶
	typedef HashIterator<Key, Type, KeyOfType, Ref, Ptr, Hash,
		listiterator, vectorptr> self;

	HashIterator(listiterator it, vectorptr pt,
		size_t pb)
		:it(it)
		, pt(pt)
		, pb(pb) {}

	HashIterator(const self& aim)
		:it(aim.it)
		, pt(aim.pt)
		, pb(aim.pb) {}

	Ref operator*() {
		return *it;
	}

	Ptr operator->() {
		return &(*it);
	}

	self& operator++() {
		++it;
		if (it == (*pt)[pb].end()) {
			do
				++pb;
			while (pb < (*pt).size() && (*pt)[pb].empty());
			if (pb < (*pt).size()) {
				it = (*pt)[pb].begin();
			}
			else {
				--pb;
				it = (*pt)[pb].end();
			}
		}
		return *this;
	}

	self operator++(int) {
		self tmp(*this);
		++(*this);
		return tmp;
	}

	self& operator--() {
		//若最后一个哈希桶为空链表，则
		//链表的begin和end迭代器相等
		if (it == (*pt)[pb].begin()) {
			auto tmp = pb;
			//找到第1个非空的迭代器
			do
				--pb;
			while (pb > 0 && (*pt)[pb].empty());
			if ((*pt)[pb].empty()) {
				pb = tmp;
				return *this;
			}
			it = --(*pt)[pb].end();
		}
		else
			--it;
		return *this;
	}

	self operator--(int) {
		self tmp(*this);
		--(*this);
		return tmp;
	}

	bool operator==(const self& aim)const {
		return pb == aim.pb && (it == aim.it);
	}

	bool operator!=(const self& aim)const {
		return !(*this == aim);
	}

};

template<class Key, class Type,
	class KeyOfType, class Hash = HashFunc<Key> >
	class HashTable {
	private:
		vector<list<Type>>tables;
		size_t _size;
		KeyOfType kot;
		Hash hashfunc;
	public:
		HashTable(size_t capacity = __stl_next_prime(0)) {
			tables.resize(capacity);
			_size = 0;
		}

		HashTable(const HashTable<Key, Type, KeyOfType, Hash>& aim)
			:tables(aim.tables)
			, _size(aim._size)
			, kot(aim.kot)
			, hashfunc(aim.hashfunc) {}


		//迭代器模板类的友元声明
		template<class Key, class Type, class KeyOfType,
			class Ref, class Ptr, class Hash,
			class listiterator,
			class vectorptr>
			friend struct HashIterator;

		//迭代器
		typedef HashIterator<Key, Type, KeyOfType,
			Type&, Type*, Hash,
			typename list<Type>::iterator,
			vector<list<Type>>*>
			iterator;
		typedef HashIterator<Key, Type, KeyOfType,
			const Type&, const Type*, Hash,
			typename list<Type>::const_iterator,
			const vector<list<Type>>*> const_iterator;


		iterator begin() {
			for (size_t i = 0; i < tables.size(); i++)
				if (!tables[i].empty())
					return iterator(tables[i].begin(),
						(&tables), i);
			return end();
		}

		iterator end() {
			return iterator(tables[tables.size() - 1].end(),
				(&tables),
				tables.size() - 1);
		}

		const_iterator begin() const {
			for (size_t i = 0; i < tables.size(); i++)
				if (!tables[i].empty())
					return const_iterator(
						tables[i].cbegin(),
						const_cast<const vector<list<Type>>*>(&tables),
						i);
			return end();
		}

		const_iterator end() const {
			return const_iterator(
				tables[tables.size() - 1].cend(),
				const_cast<const vector<list<Type>>*>(&tables),
				tables.size() - 1);
		}

		//插入
		pair<iterator, bool> insert(const Type& aim) {
			auto it = find(kot(aim));
			if (it != end())
				return make_pair(it, false);
			//负载因子等于1时扩容
			if (_size == tables.size()) {
				//生成新表
				vector<list<Type>>nt(__stl_next_prime(_size + 1));
				for (auto& tb : tables) {
					for (auto& lt : tb) {
						size_t hashi = hashfunc(kot(lt)) % nt.size();
						nt[hashi].push_front(lt);
					}
				}
				tables.swap(nt);
			}

			size_t hashi = hashfunc(kot(aim)) % tables.size();
			tables[hashi].push_front(aim);
			++_size;
			return make_pair(iterator(tables[hashi].begin(), &tables, hashi), true);
		}

		//查找
		iterator find(const Key& aim) {
			size_t hashi = hashfunc(aim) % tables.size();
			for (auto it = tables[hashi].begin(), ed = tables[hashi].end();
				it != ed; it++)
				if (kot(*it) == aim)
					return iterator(it, &tables, hashi);
			return end();
		}

		//删除
		iterator erase(const Key& aim) {
			auto it = find(aim);
			if (it == end())
				return it;
			size_t hashi = hashfunc(aim) % tables.size();

			auto nit = tables[hashi].erase(
				std::find(tables[hashi].begin(), tables[hashi].end(), *it)
			);
			--_size;
			if (nit != tables[hashi].end()) {
				return iterator(nit, &tables, hashi);
			}
			for (size_t i = hashi + 1; i < tables.size(); i++)
				if (!tables[i].empty())
					return iterator(tables[i].begin(), &tables, i);
			return end();
		}

		size_t size()const {
			return this->_size;
		}

		bool empty()const {
			return _size == 0;
		}

		void clear() {
			for (auto& x : tables)
				x.clear();
			_size = 0;
		}

		size_t count(const Key& val) {
			size_t cnt = 0, hashi = hashfunc(val) % tables.size();
			for (auto& x : tables[hashi])
				if (kot(x) == val)
					++cnt;
			return cnt;
		}
};

//以下为测试用的函数
template<class Key, class Type>
struct setKeyOfType {
	const Key& operator()(const Type& aim) {
		return aim;
	}
};

template<class Key, class Type>
struct mapKeyOfType {
	const Key& operator()(const Type& aim) {
		return aim.first;
	}
};

//void f() {
//	std::vector<int>arr = { 1,3,4,5,6,7 };
//	auto it = arr.begin();
//	for (const auto ed = arr.end(); it != ed; ++it)
//		cout << *it << " ";
//}

void testhash() {
	srand(size_t(time(0)));
	HashTable<int, int, setKeyOfType<int, int> >ht;
	vector<int>arr = { 1,2,55,92,2,3,5,6,7,8,34 };
	for (auto x : arr)
		ht.insert(x);
	//测试find和erase
	auto it = ht.find(2);
	cout << *it << "\n";
	it = ht.erase(92);
	//cout << *it << endl;
	if (it == ht.end())
		cout << 92 << "\n";
	cout << "\n";

	//测试迭代器
	auto nit = (ht).begin();
	for (auto ned = ht.end(); nit != ned; nit++)
		cout << *nit << ' ';
	cout << "\n";
	for (const auto& x : ht)
		cout << x << ' ';
	cout << "\n";
	cout << "\n";

	//带常属性的迭代器实验
	const HashTable<int, int, setKeyOfType<int, int> >ht2(ht);
	for (auto& x : ht2)
		cout << x << ' ';
	cout << "\n";
	auto ht2it = ht2.begin();
	for (auto ht2ed = ht2.end(); ht2it != ht2ed; ++ht2it)
		cout << *ht2it << ' ';
	cout << "\n";
	cout << "\n";

	//测试Key-Value模型的匹配度
	HashTable<int, pair<int, int>, mapKeyOfType<int, pair<int, int>> >ht3;
	ht3.insert(make_pair(1, 2));
	ht3.insert(make_pair(2, 2));
	ht3.insert(make_pair(3, 2));
	ht3.insert(make_pair(4, 2));
	for (auto& x : ht3)
		cout << "[" << x.first << ":" << x.second << "]" << " ";
	cout << "\n";
	cout << (*ht3.find(3)).second << "\n";
	ht3.erase(3);
	for (auto& x : ht3)
		cout << "[" << x.first << ":" << x.second << "]" << " ";
	cout << "\n";

	ht3.insert(make_pair(3, 2));
	ht3.insert(make_pair(56, 2));
	ht3.insert(make_pair(109, 2));
	auto mapit = --ht3.end();
	for (auto maped = ht3.begin(); (mapit) != maped; --mapit)
		cout << "[" << (*mapit).first << ":" << (*mapit).second << "]" << " ";
	cout << "\n";
	cout << "[" << (*mapit).first << ":" << (*mapit).second << "]" << "\n";
	--mapit;
	//这里设置了--begin()不起作用
	cout << "[" << mapit->first << ":" << mapit->second << "]" << "\n";
}


```

用红黑树和链表实现的哈希表，以后有机会会实现。

# unordered_map和unordered_set的模拟实现

`unordered_map`和`unordered_set`完全可以将红黑树的[map和set的模拟实现](https://blog.csdn.net/m0_73693552/article/details/149635563)的原型搬过来，将底层容器更改为开散列哈希表，再修改一些细节即可。

## unordered_map

```cpp
#pragma once
#include"hash_table.h"
#include<functional>
using std::less;

namespace mystd {
	template<class Key, class Type, class Compare = less<Type> >
	class unordered_map {
	public:
		typedef pair<const Key, Type> val_type;
	public:
		//获取键值的仿函数
		struct mapKeyOfType {
			const Key& operator()(const val_type& key) const {
				return key.first;
			}
		};
	private:
		HashTable<Key, pair<const Key, Type>, mapKeyOfType> ht;
	public:
		//构造函数
		unordered_map() {}
		unordered_map(const unordered_map& st)
			:ht(st.ht) {}
		template<class InputIterator>
		unordered_map(InputIterator first, InputIterator last) {
			for (auto it = first; it != last; ++it)
				insert(*it);
		}

		//迭代器
		typedef typename
			HashTable<Key, pair<const Key, Type>,
			mapKeyOfType>::iterator
			iterator;

		typedef typename
			HashTable<Key, pair<const Key, Type>,
			mapKeyOfType>::const_iterator
			const_iterator;

		iterator begin() {
			return ht.begin();
		}
		iterator end() {
			return ht.end();
		}
		const_iterator begin() const {
			return ht.begin();
		}
		const_iterator end() const {
			return ht.end();
		}

		//插入
		pair<iterator, bool> insert(const val_type& val) {
			return pair<iterator, bool>(ht.insert(val));
		}

		//[]访问
		Type& operator[](const Key& key) {
			//照搬库里的写法
			return (*(
				insert(make_pair(key, Type()))
				.first)).second;
		}

		//删除
		bool erase(const Key& val) {
			size_t tmp = ht.size();
			auto it = ht.erase(val);
			if (tmp == ht.size() + 1)
				return true;
			return false;
		}

		//返回存储的元素个数
		size_t size()const {
			return ht.size();
		}

		//判空
		bool empty() const {
			return ht.empty();
		}

		//清空
		void clear() {
			ht.clear();
		}

		//查找
		iterator find(const Key& val) {
			return ht.find(val);
		}

		//计数
		size_t count(const Key& val) {
			return ht.count(val);
		}

	};
	void test_map() {
		mystd::unordered_map<string, int>mp;
		if (mp.empty())
			cout << "mp.empty()" << "\n";
		mp.insert(make_pair("insert", 1));
		if (!mp.empty())
			cout << mp.size() << "\n";
		mp["insert"] = 2;
		mp["erase"] = 3;
		mp["hash"] = 4;
		cout << mp.find("erase")->second << "\n";
		for (auto& x : mp)
			cout << "[" << x.first << ":" << x.second << "] ";
		cout << "\n";
		mp.erase("erase");
		for(auto&x:mp)
			cout << "[" << x.first << ":" << x.second << "] ";
		cout << "\n";

	}
}
```

## unordered_set

```cpp
#pragma once
#include"hash_table.h"
#include<functional>
using std::less;

namespace mystd {
	template<class Type, class Compare = less<Type> >
	class unordered_set {
	public:
		typedef Type Key;
		//获取键值的仿函数
		struct setKeyOfType {
			const Type& operator()(const Type& key) const {
				return key;
			}
		};

		//构造函数
		unordered_set() {}
		unordered_set(const unordered_set<Type, Compare>& st)
			:ht(st.ht) {}
		template<class InputIterator>
		unordered_set(InputIterator first, InputIterator last) {
			for (auto it = first; it != last; ++it)
				insert(*it);
		}

		//迭代器
		typedef typename
			HashTable<const Key, Key,
			setKeyOfType>::iterator
			iterator;

		typedef typename
			HashTable<const Key, Key,
			setKeyOfType>::const_iterator
			const_iterator;

		iterator begin() {
			return ht.begin();
		}
		iterator end() {
			return ht.end();
		}
		const_iterator begin() const {
			return ht.begin();
		}
		const_iterator end() const {
			return ht.end();
		}

		//插入
		pair<iterator, bool> insert(const Type& val) {
			return pair<iterator, bool>(ht.insert(val));
		}

		//删除
		bool erase(const Type& val) {
			size_t tmp = ht.size();
			auto it = ht.erase(val);
			if (tmp == ht.size() + 1)
				return true;
			return false;
		}

		//返回存储的元素个数
		size_t size()const {
			return ht.size();
		}

		//判空
		bool empty() const {
			return ht.empty();
		}

		//清空
		void clear() {
			ht.clear();
		}

		//查找
		iterator find(const Type& val) {
			return ht.find(val);
		}

		//计数
		size_t count(const Key& val) {
			return ht.count(val);
		}

	private:
		//底层哈希表
		//STL容器禁止上传带const的数据类型作为容器存储的数据
		HashTable<const Key, Key, setKeyOfType> ht;
	};
	void fset() {
		mystd::unordered_set<int>st;
		if (st.empty())
			cout << "st.empty()" << "\n";
		st.insert(1);
		if (!st.empty())
			cout << st.size() << "\n";
		st.insert(2);
		st.insert(3);
		st.insert(4);
		cout << *st.find(3) << "\n";
		for (auto& x : st)
			cout << x << " ";
		cout << "\n";
		st.erase(3);
		for (auto& x : st)
			cout << x << " ";
		cout << "\n";

	}
}
```







