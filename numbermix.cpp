#include <iostream>
#include <algorithm>
using namespace std;

bool cmp(string a, string b) {
	return a + b > b + a;
}//cmp的目的是重构后文中sort函数中的比较标准，同时此中的+为string的加法即首尾相接

int main() {
	int n = 0;
	cin >> n;
	string a[n];
	for (int i = 0; i < n; i++)
		cin >> a[i];//这里可以学一下，直接就连续输入了捏
	sort(a, a + n, cmp);
	for (int i = 0; i < n; i++) {
		cout << a[i];
	}
	cout << endl;
	return 0;
}