#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>

using namespace std;

// 定义符号类型
enum SymbolType {
	NUMBER, ID, LPAREN, RPAREN, PLUS, MINUS, MULT, DIV, DOLLAR,
	E, EP, T, TP, F, EPSILON, ERROR
};

// 预测分析表
unordered_map<SymbolType, unordered_map<SymbolType, vector<SymbolType>>> parsingTable;

// 符号到字符串的映射
unordered_map<SymbolType, string> symbolToString = {
	{NUMBER, "number"}, {ID, "id"}, {LPAREN, "("}, {RPAREN, ")"},
	{PLUS, "+"}, {MINUS, "-"}, {MULT, "*"}, {DIV, "/"}, {DOLLAR, "$"},
	{E, "E"}, {EP, "E'"}, {T, "T"}, {TP, "T'"}, {F, "F"}, {EPSILON, "ε"}
};

// 初始化预测分析表
void initializeParsingTable() {
	// E 行
	parsingTable[E][NUMBER] = {T, EP};
	parsingTable[E][ID] = {T, EP};
	parsingTable[E][LPAREN] = {T, EP};

	// E' 行
	parsingTable[EP][RPAREN] = {EPSILON};
	parsingTable[EP][PLUS] = {PLUS, T, EP};
	parsingTable[EP][MINUS] = {MINUS, T, EP};
	parsingTable[EP][DOLLAR] = {EPSILON};

	// T 行
	parsingTable[T][NUMBER] = {F, TP};
	parsingTable[T][ID] = {F, TP};
	parsingTable[T][LPAREN] = {F, TP};

	// T' 行
	parsingTable[TP][RPAREN] = {EPSILON};
	parsingTable[TP][PLUS] = {EPSILON};
	parsingTable[TP][MINUS] = {EPSILON};
	parsingTable[TP][MULT] = {MULT, F, TP};
	parsingTable[TP][DIV] = {DIV, F, TP};
	parsingTable[TP][DOLLAR] = {EPSILON};

	// F 行
	parsingTable[F][NUMBER] = {NUMBER};
	parsingTable[F][ID] = {ID};
	parsingTable[F][LPAREN] = {LPAREN, E, RPAREN};
}

// 将字符转换为符号类型
SymbolType charToSymbol(char c) {
	switch (c) {
		case '(':
			return LPAREN;
		case ')':
			return RPAREN;
		case '+':
			return PLUS;
		case '-':
			return MINUS;
		case '*':
			return MULT;
		case '/':
			return DIV;
		case '$':
			return DOLLAR;
		default:
			return ERROR;
	}
}

// 词法分析：将输入字符串转换为符号序列
vector<SymbolType> lexer(const string &input) {
	vector<SymbolType> tokens;
	string temp = input + "$"; // 添加结束符

	for (size_t i = 0; i < temp.length(); i++) {
		char c = temp[i];

		if (c == '$') {
			tokens.push_back(DOLLAR);
			break;
		}

		SymbolType sym = charToSymbol(c);
		if (sym != ERROR) {
			tokens.push_back(sym);
		} else if (isdigit(c)) {
			// 处理数字
			while (i + 1 < temp.length() && isdigit(temp[i + 1])) {
				i++;
			}
			tokens.push_back(NUMBER);
		} else if (isalpha(c)) {
			// 处理标识符
			while (i + 1 < temp.length() && isalnum(temp[i + 1])) {
				i++;
			}
			tokens.push_back(ID);
		}
		// 忽略空格等其他字符
	}

	return tokens;
}

// 获取产生式字符串
string getProductionString(SymbolType nonTerminal, const vector<SymbolType> &production) {
	string result = symbolToString[nonTerminal] + " → ";
	for (SymbolType sym : production) {
		if (sym == EPSILON) {
			result += "ε ";
		} else {
			result += symbolToString[sym] + " ";
		}
	}
	return result;
}

// 获取栈内容的字符串表示
string getStackString(stack<SymbolType> st) {
	string result;
	while (!st.empty()) {
		result = symbolToString[st.top()] + result;
		st.pop();
	}
	return result;
}

// 获取输入串的字符串表示
string getInputString(const vector<SymbolType> &tokens, size_t ip) {
	string result;
	for (size_t i = ip; i < tokens.size(); i++) {
		result += symbolToString[tokens[i]];
	}
	return result;
}

// LL(1) 语法分析
bool LL1Parser(const string &input) {
	vector<SymbolType> tokens = lexer(input);
	stack<SymbolType> st;

	// 初始化栈
	st.push(DOLLAR);
	st.push(E);

	size_t ip = 0; // 输入指针
	string matched = ""; // 已匹配部分

	// 输出表头
	cout << "+---------------------------------------------------------------+" << endl;
	cout << "| 步骤 |   已匹配   |     栈     |     输入     |      动作      |" << endl;
	cout << "|------|------------|------------|--------------|----------------|" << endl;

	int step = 1;

	while (!st.empty()) {
		SymbolType X = st.top();
		SymbolType a = tokens[ip];

		string stackStr = getStackString(st);
		string inputStr = getInputString(tokens, ip);
		string action = "";

		if (X == a) {
			// 匹配终结符
			if (X == DOLLAR) {
				action = "接受";
				cout << "| " << setw(4) << step << " | "
				     << setw(10) << matched << " | "
				     << setw(10) << stackStr << " | "
				     << setw(12) << inputStr << " | "
				     << setw(14) << action << " |" << endl;
				return true;
			}

			action = "匹配 " + symbolToString[X];
			matched += symbolToString[X];
			st.pop();
			ip++;

		} else if (X == NUMBER || X == ID || X == LPAREN || X == RPAREN ||
		           X == PLUS || X == MINUS || X == MULT || X == DIV) {
			// X 是终结符但不匹配
			action = "错误：期望 " + symbolToString[X] + "，但找到 " + symbolToString[a];
			cout << "| " << setw(4) << step << " | "
			     << setw(10) << matched << " | "
			     << setw(10) << stackStr << " | "
			     << setw(12) << inputStr << " | "
			     << setw(14) << action << " |" << endl;
			return false;
		} else {
			// X 是非终结符
			if (parsingTable[X].find(a) != parsingTable[X].end()) {
				vector<SymbolType> production = parsingTable[X][a];
				action = "输出: " + getProductionString(X, production);

				st.pop();

				// 将产生式右部逆序压入栈中（ε 不压入）
				if (production[0] != EPSILON) {
					for (int i = static_cast<int>(production.size()) - 1; i >= 0; i--) {
						st.push(production[i]);
					}
				}
			} else {
				action = "错误：M[" + symbolToString[X] + ", " + symbolToString[a] + "] 为空";
				cout << "| " << setw(4) << step << " | "
				     << setw(10) << matched << " | "
				     << setw(10) << stackStr << " | "
				     << setw(12) << inputStr << " | "
				     << setw(14) << action << " |" << endl;
				return false;
			}
		}

		// 输出当前步骤
		cout << "| " << setw(4) << step << " | "
		     << setw(10) << matched << " | "
		     << setw(10) << stackStr << " | "
		     << setw(12) << inputStr << " | "
		     << setw(14) << action << " |" << endl;

		step++;
	}

	cout << "+---------------------------------------------------------------+" << endl;
	return false;
}

int main() {
	initializeParsingTable();

	string input = "a*(3+b)/4-7";
	cout << "分析输入串: " << input << endl << endl;

	bool result = LL1Parser(input);

	if (result) {
		cout << "+---------------------------------------------------------------+" << endl;
		cout << endl << "分析成功！输入串是合法的表达式。" << endl;
	} else {
		cout << endl << "分析失败！输入串不是合法的表达式。" << endl;
	}

	return 0;
}