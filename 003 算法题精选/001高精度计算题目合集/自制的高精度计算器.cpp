#include<bits/stdc++.h>
using namespace std; 

class Bigint {
public:
	//构造函数
	Bigint(const string& st) {
		num.clear();
		if (st.size() == 0)//空串
			num = "0";
		else {
			for (size_t i = 0; i < st.size(); i++) {
				if (i == 0) {
					if (((st[i] < '0' || st[i]>'9') && (st[i] != '-'))//直接就是其他符号。
						|| (st[0] == '-' && st.size() == 1)//单个负号
						|| (st[0] == '-' && st.size() > 1 && (st[1] < '0' || st[1]>'9'))) {//负号加其他符号。
						num = "0";
						break;
					}
					else
						num += st[0];
				}
				else if (!(st[i] < '0' || st[i]>'9'))
					num += st[i];
				else//000000g、123t
					break;
			}
			if (num[0] == '-') {//-0003或-000000
				while (num.size() > 2 && num[1] == '0')
					num.erase(1, 1);
				if (num[1] == '0')
					num = "0";
			}
			else {
				while (num.size() > 1 && num[0] == '0')
					num.erase(0, 1);
			}

		}

	}
	//委托构造函数，c++11之后的标准才能用
	Bigint(const char* st = "0") :Bigint(string(st)) {}
	Bigint(const Bigint& st) :Bigint(st.num) {}
	template<class T>
	Bigint(T st) : Bigint(to_string(st)) {}

	//赋值重载
	Bigint& operator=(const Bigint& aim) {
		if (&aim != this)
			num = aim.num;
		return *this;
	}
	template<class T>
	Bigint& operator=(T aim) {
		return *this = Bigint(aim);
	}

	//为了Bigint支持类似bool值的逻辑判断
	operator bool() {
		if (num == "0")
			return false;
		return true;
	}

	//等于
	bool operator==(const Bigint& aim) const {
		return aim.num == num;
	}
	template<class T>
	bool operator==(T aim) const {
		return Bigint(aim) == *this;
	}

	//大于
	bool operator>(const Bigint& aim) const {
		if (num[0] != '-' && aim.num[0] == '-')
			return 1;
		else if (num[0] == '-' && aim.num[0] != '-')
			return 0;
		if (num.size() > aim.num.size()) {
			if (num[0] == '-')
				return 0;
			return 1;
		}
		if (num.size() == aim.num.size())
			return num > aim.num;
		if (num.size() < aim.num.size()) {
			if (num[0] == '-')
				return 1;
		}
		return 0;
	}
	template<class T>
	bool operator>(T aim) const {
		return *this > Bigint(aim);
	}

	//小于
	bool operator<(const Bigint& aim) const {
		return !(*this > aim || *this == aim);
	}
	template<class T>
	bool operator<(T aim) const {
		return *this < Bigint(aim);
	}

	//小于等于
	bool operator<=(const Bigint& aim) const{
		return !(*this > aim);
	}
	template<class T>
	bool operator<=(T aim) const{
		return *this <= Bigint(aim);
	}

	//大于等于
	bool operator>=(const Bigint& aim) const{
		return !(*this < aim);
	}
	template<class T>
	bool operator>=(T aim) const{
		return *this >= Bigint(aim);
	}

	//不等于
	bool operator!=(const Bigint& aim) const{
		return !(*this == aim);
	}
	template<class T>
	bool operator!=(T aim) const{
		return *this != Bigint(aim);
	}

	//内置交换函数
	void swap(Bigint& b) {
		std::swap(this->num, b.num);
	}

	//加法
	Bigint& operator+=(const Bigint& aim) {
		int up = 0;
		bool flag = 0;
		string& a = num;
		string b = aim.num;

		//符号不同时跳转减法
		if (a[0] == '-' && b[0] != '-') {
			a.erase(0, 1);
			return *this = Bigint(b) -= *this;
		}
		if (a[0] != '-' && b[0] == '-') {
			return *this -= Bigint(aim.num.substr(1));
		}

		//负数加负数
		if (a[0] == '-' && b[0] == '-') {
			a.erase(0, 1); b.erase(0, 1);
			flag = 1;
		}
		string c(max(a.size(), b.size()), '0');

		//补齐长度
		if (a.size() < b.size())
			a = string(b.size() - a.size(), '0') + a;
		if (b.size() < a.size())
			b = string(a.size() - b.size(), '0') + b;

		//加法核心
		for (size_t i = a.size() - 1; i != -1; i--) {
			c[i] = char((a[i] - '0' + b[i] - '0' + up) % 10 + '0');
			up = (a[i] - '0' + b[i] - '0' + up) / 10;
		}
		while (up) {//补进位
			c = (char)(up % 10 + '0') + c;
			up /= 10;
		}

		//去除前缀0
		while (*c.begin() == '0' && c.size() > 1)
			c.erase(0, 1);
		while (*a.begin() == '0' && a.size() > 1)
			a.erase(0, 1);
		while (*b.begin() == '0' && b.size() > 1)
			b.erase(0, 1);

		//两个负数相加
		if (flag) {
			c = '-' + c;
			a = '-' + a;
			b = '-' + b;
		}
		a.swap(c);
		return *this;
	}
	template<class T>
	Bigint& operator+=(const T& aim) {
		return *this += Bigint(aim);
	}

	template<class T>
	Bigint operator+(const T& aim) {
		Bigint tmp = *this;
		tmp += Bigint(aim);
		return tmp;
	}

	//减法
	Bigint& operator-=(const Bigint& aim) {
		string& a = num;
		string b = aim.num;
		//符号不同时跳转加法
		if (a[0] == '-' && b[0] != '-') {
			a.erase(0, 1);
			*this += aim;
			a = '-' + a;
			return *this;
		}
		if (a[0] != '-' && b[0] == '-') {
			b.erase(0, 1);
			return *this += Bigint(b);
		}

		//负数减负数
		bool flag1 = 0;
		if (a[0] == '-' && b[0] == '-') {
			a.erase(0, 1); b.erase(0, 1);
			a.swap(b);
			flag1 = 1;
		}

		//排除a<b的情况
		bool flag2 = 0;
		if (Bigint(b)>*this) {
			a.swap(b);
			flag2 = 1;
		}

		if (b.size() < a.size())
			b = string(a.size() - b.size(), '0') + b;

		//字符串减法
		string c(a.size(), '0');
		bool ten = 0;
		for (size_t i = a.size() - 1; i != -1; i--) {
			ten = 0;
			if (a[i] < b[i]) {
				a[i - 1] -= 1;
				a[i] += 10;
				ten = 1;
			}
			c[i] = a[i] - b[i] + '0';
			if (ten)
				a[i] -= 10;
			if (i + 1 < a.size())
				if (c[i + 1] - '0' + b[i + 1] - '0' > 9)
					a[i] += 1;
		}

		while (c.size() > 1 && c[0] == '0')
			c.erase(0, 1);
		while (b.size() > 1 && b[0] == '0')
			b.erase(0, 1);

		if (flag2) {
			a.swap(b);
			c = '-' + c;
		}
		if (flag1) {
			a = '-' + a;
			b = '-' + b;
			a.swap(b);
		}
		a.swap(c);
		return *this;
	}
	template<class T>
	Bigint& operator-=(const T& aim) {
		return *this -= Bigint(aim);
	}

	template<class T>
	Bigint operator-(const T& aim) {
		Bigint tmp = *this;
		tmp -= Bigint(aim);
		return tmp;
	}

	//乘法
	Bigint& operator*=(const Bigint& aim) {
		string& a = num;
		string b = aim.num;
		//符号相同时结果为正
		bool va = 0, vb = 0;
		if (a[0] == '-' && b[0] == '-') {
			a.erase(0, 1);
			b.erase(0, 1);
			va = 1; vb = 1;
		}

		//符号不同时结果为负数
		bool flag = 0;
		if ((a[0] == '-' && b[0] != '-') || (a[0] != '-' && b[0] == '-')) {
			if (a[0] == '-') {
				va = 1;
				a.erase(0, 1);
			}
			if (b[0] == '-') {
				vb = 1;
				b.erase(0, 1);
			}
			flag = 1;
		}

		//1位0的情况
		if ((a.size() == 1 && a[0] == '0') || (b.size() && b[0] == '0')) {
			if (va)
				a = '-' + a;
			if (vb)
				b = '-' + b;
			this->num = "0";
			return *this;
		}

		//高精度乘法原理（string对象）：
		//c[i+j+1]=(c[i+j+1]-'0')+(a[i]-'0')*(b[j]-'0')+w;
		string c(a.size() + b.size(), '0');
		for (size_t j = b.size() - 1, w; j != -1; j--) {
			w = 0;
			for (size_t i = a.size() - 1; i != -1; i--) {
				c[i + j + 1] = (c[i + j + 1] - '0') + (a[i] - '0') * (b[j] - '0') + w;
				w = c[i + j + 1] / 10;
				c[i + j + 1] %= 10;
				c[i + j + 1] += '0';
			}
			c[j] = c[j] + w;
		}
		//清除占位用的0
		while (c[0] == '0' && c.size() > 1)
			c.erase(0, 1);

		if (flag)
			c = '-' + c;
		if (va)
			a = '-' + a;
		if (vb)
			b = '-' + b;
		a.swap(c);
		return *this;
	}
	template<class T>
	Bigint& operator*=(const T& aim) {
		return *this *= Bigint(aim);
	}
	template<class T>
	Bigint operator*(const T& aim) {
		Bigint tmp = *this;
		tmp *= Bigint(aim);
		return tmp;
	}

	//除法
	Bigint& operator/=(const Bigint& aim) {
		string& a = num;
		string b = aim.num;

		//b为0的情况
		bool vb = 0;
		if (b[0] == '-') {
			b.erase(0, 1);
			vb = 1;
		}
		while (b.size() > 1 && b[0] == '0')
			b.erase(0, 1);
		if (b.size() == 1 && b == "0") {
			this->num = "nan";
			return *this;
		}
		//a为0的情况
		bool va = 0;
		if (a[0] == '-') {
			a.erase(0, 1);
			va = 1;
		}
		while (a.size() > 1 && a[0] == '0')
			a.erase(0, 1);
		if (a.size() == 1 && a == "0") {
			this->num = "0";
			return *this;
		}

		//正数除以正数
		string c, r;//商，余数
		for (auto& x : a) {
			r += x;
			while (r.size() > 1 && r[0] == '0')
				r.erase(0, 1);
			size_t cnt = 0;
			Bigint tr = r, tb = b;
			while (tr>tb || tr == tb) {
				tr-=tb;
				++cnt;
			}
			c.push_back(char(cnt + '0'));
			while (c.size() > 1 && c[0] == '0')
				c.erase(0, 1);
			r.swap(tr.num);
		}
		//正数除以负数
		if (!va && vb) {
			b = '-' + b;
			if (c.size() == 1 && c[0] == '0') {
				a.swap(c);
				return *this;
			}
			c = '-' + c;
			a.swap(c);
			return *this;
		}
		//负数除以正数
		if (va && !vb) {
			a = '-' + a;
			if (c.size() == 1 && c[0] == '0') {
				a.swap(c);
				return *this;
			}
			c = '-' + c;
			if (!(r.size() == 1 && c[0] == '0'))
				r = '-' + r;
			a.swap(c);
			return *this;
		}
		//负数除以负数
		if (va && vb) {
			a = '-' + a;
			b = '-' + b;
			if (!(r.size() == 1 && c[0] == '0'))
				r = '-' + r;
			a.swap(c);
			return *this;
		}
		a.swap(c);
		return *this;
	}
	template<class T>
	Bigint& operator/=(const T& aim) {
		return *this /= Bigint(aim);
	}
	template<class T>
	Bigint operator/(const T& aim) {
		Bigint tmp = *this;
		Bigint tmp2 = Bigint(aim);
		if (tmp2.num.size() == 1 && tmp2.num[0] == '0')
			tmp.num = "nan";
		else
			tmp /= Bigint(aim);
		return tmp;
	}


	//求模
	Bigint& operator%=(const Bigint& aim) {
		string& a = num;
		string b = aim.num;

		//b为0的情况
		bool vb = 0;
		if (b[0] == '-') {
			b.erase(0, 1);
			vb = 1;
		}
		while (b.size() > 1 && b[0] == '0')
			b.erase(0, 1);
		if (b.size() == 1 && b == "0") {
			this->num = "nan";
			return *this;
		}
		//a为0的情况
		bool va = 0;
		if (a[0] == '-') {
			a.erase(0, 1);
			va = 1;
		}
		while (a.size() > 1 && a[0] == '0')
			a.erase(0, 1);
		if (a.size() == 1 && a == "0") {
			this->num = "0";
			return *this;
		}

		//正数除以正数
		string c, r;//商，余数
		for (auto& x : a) {
			r += x;
			while (r.size() > 1 && r[0] == '0')
				r.erase(0, 1);
			size_t cnt = 0;
			Bigint tr = r, tb = b;
			while (tr > tb || tr == tb) {
				tr -= tb;
				++cnt;
			}
			c.push_back(char(cnt + '0'));
			while (c.size() > 1 && c[0] == '0')
				c.erase(0, 1);
			r.swap(tr.num);
		}
		//正数除以负数
		if (!va && vb) {
			a.swap(r);
			return *this;
		}
		//负数除以正数
		if (va && !vb) {
			a = '-' + a;
			if (c.size() == 1 && c[0] == '0') {
				a.swap(c);
				return *this;
			}
			c = '-' + c;
			if (!(r.size() == 1 && c[0] == '0'))
				r = '-' + r;
			a.swap(r);
			return *this;
		}
		//负数除以负数
		if (va && vb) {
			a = '-' + a;
			b = '-' + b;
			if (!(r.size() == 1 && c[0] == '0'))
				r = '-' + r;
			a.swap(r);
			return *this;
		}
		a.swap(r);
		return *this;
	}
	template<class T>
	Bigint& operator%=(const T& aim) {
		return *this %= Bigint(aim);
	}
	template<class T>
	Bigint operator%(const T& aim) {
		Bigint tmp = *this;
		return tmp %= Bigint(aim);
	}

	friend ostream& operator<<(ostream&, const Bigint&);
	friend istream& operator>>(istream&, Bigint&);
private:
	string num = "0";
};

ostream& operator<<(ostream& out, const Bigint& aim) {
	out << aim.num;
	return out;
}

istream& operator>>(istream& in, Bigint& aim) {
	in >> aim.num;
	return in;
}

int main(){
	Bigint a,b;
	char c;
	cin>>a>>c>>b;
	if(c=='+')
		cout<<(a+b)<<endl;
	else if(c=='-')
		cout<<(a-b)<<endl;
	else if(c=='*')
		cout<<(a*b)<<endl;
	else if(c=='/')
		cout<<(a/b)<<endl;
	else if(c=='%')
		cout<<(a%b)<<endl;
	return 0;
} 
/*
        654465749865465
               65479864
42854328293848666496760
*/

