#include <iostream>
#include <algorithm>
using namespace std;

bool cmp(string a, string b) {
	return a + b > b + a;
}//cmp��Ŀ�����ع�������sort�����еıȽϱ�׼��ͬʱ���е�+Ϊstring�ļӷ�����β���

int main() {
	int n = 0;
	cin >> n;
	string a[n];
	for (int i = 0; i < n; i++)
		cin >> a[i];//�������ѧһ�£�ֱ�Ӿ�������������
	sort(a, a + n, cmp);
	for (int i = 0; i < n; i++) {
		cout << a[i];
	}
	cout << endl;
	return 0;
}