#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>

using namespace std;

// �����������
enum SymbolType {
	NUMBER, ID, LPAREN, RPAREN, PLUS, MINUS, MULT, DIV, DOLLAR,
	E, EP, T, TP, F, EPSILON, ERROR
};

// Ԥ�������
unordered_map<SymbolType, unordered_map<SymbolType, vector<SymbolType>>> parsingTable;

// ���ŵ��ַ�����ӳ��
unordered_map<SymbolType, string> symbolToString = {
	{NUMBER, "number"}, {ID, "id"}, {LPAREN, "("}, {RPAREN, ")"},
	{PLUS, "+"}, {MINUS, "-"}, {MULT, "*"}, {DIV, "/"}, {DOLLAR, "$"},
	{E, "E"}, {EP, "E'"}, {T, "T"}, {TP, "T'"}, {F, "F"}, {EPSILON, "��"}
};

// ��ʼ��Ԥ�������
void initializeParsingTable() {
	// E ��
	parsingTable[E][NUMBER] = {T, EP};
	parsingTable[E][ID] = {T, EP};
	parsingTable[E][LPAREN] = {T, EP};

	// E' ��
	parsingTable[EP][RPAREN] = {EPSILON};
	parsingTable[EP][PLUS] = {PLUS, T, EP};
	parsingTable[EP][MINUS] = {MINUS, T, EP};
	parsingTable[EP][DOLLAR] = {EPSILON};

	// T ��
	parsingTable[T][NUMBER] = {F, TP};
	parsingTable[T][ID] = {F, TP};
	parsingTable[T][LPAREN] = {F, TP};

	// T' ��
	parsingTable[TP][RPAREN] = {EPSILON};
	parsingTable[TP][PLUS] = {EPSILON};
	parsingTable[TP][MINUS] = {EPSILON};
	parsingTable[TP][MULT] = {MULT, F, TP};
	parsingTable[TP][DIV] = {DIV, F, TP};
	parsingTable[TP][DOLLAR] = {EPSILON};

	// F ��
	parsingTable[F][NUMBER] = {NUMBER};
	parsingTable[F][ID] = {ID};
	parsingTable[F][LPAREN] = {LPAREN, E, RPAREN};
}

// ���ַ�ת��Ϊ��������
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

// �ʷ��������������ַ���ת��Ϊ��������
vector<SymbolType> lexer(const string &input) {
	vector<SymbolType> tokens;
	string temp = input + "$"; // ��ӽ�����

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
			// ��������
			while (i + 1 < temp.length() && isdigit(temp[i + 1])) {
				i++;
			}
			tokens.push_back(NUMBER);
		} else if (isalpha(c)) {
			// �����ʶ��
			while (i + 1 < temp.length() && isalnum(temp[i + 1])) {
				i++;
			}
			tokens.push_back(ID);
		}
		// ���Կո�������ַ�
	}

	return tokens;
}

// ��ȡ����ʽ�ַ���
string getProductionString(SymbolType nonTerminal, const vector<SymbolType> &production) {
	string result = symbolToString[nonTerminal] + " �� ";
	for (SymbolType sym : production) {
		if (sym == EPSILON) {
			result += "�� ";
		} else {
			result += symbolToString[sym] + " ";
		}
	}
	return result;
}

// ��ȡջ���ݵ��ַ�����ʾ
string getStackString(stack<SymbolType> st) {
	string result;
	while (!st.empty()) {
		result = symbolToString[st.top()] + result;
		st.pop();
	}
	return result;
}

// ��ȡ���봮���ַ�����ʾ
string getInputString(const vector<SymbolType> &tokens, size_t ip) {
	string result;
	for (size_t i = ip; i < tokens.size(); i++) {
		result += symbolToString[tokens[i]];
	}
	return result;
}

// LL(1) �﷨����
bool LL1Parser(const string &input) {
	vector<SymbolType> tokens = lexer(input);
	stack<SymbolType> st;

	// ��ʼ��ջ
	st.push(DOLLAR);
	st.push(E);

	size_t ip = 0; // ����ָ��
	string matched = ""; // ��ƥ�䲿��

	// �����ͷ
	cout << "+---------------------------------------------------------------+" << endl;
	cout << "| ���� |   ��ƥ��   |     ջ     |     ����     |      ����      |" << endl;
	cout << "|------|------------|------------|--------------|----------------|" << endl;

	int step = 1;

	while (!st.empty()) {
		SymbolType X = st.top();
		SymbolType a = tokens[ip];

		string stackStr = getStackString(st);
		string inputStr = getInputString(tokens, ip);
		string action = "";

		if (X == a) {
			// ƥ���ս��
			if (X == DOLLAR) {
				action = "����";
				cout << "| " << setw(4) << step << " | "
				     << setw(10) << matched << " | "
				     << setw(10) << stackStr << " | "
				     << setw(12) << inputStr << " | "
				     << setw(14) << action << " |" << endl;
				return true;
			}

			action = "ƥ�� " + symbolToString[X];
			matched += symbolToString[X];
			st.pop();
			ip++;

		} else if (X == NUMBER || X == ID || X == LPAREN || X == RPAREN ||
		           X == PLUS || X == MINUS || X == MULT || X == DIV) {
			// X ���ս������ƥ��
			action = "�������� " + symbolToString[X] + "�����ҵ� " + symbolToString[a];
			cout << "| " << setw(4) << step << " | "
			     << setw(10) << matched << " | "
			     << setw(10) << stackStr << " | "
			     << setw(12) << inputStr << " | "
			     << setw(14) << action << " |" << endl;
			return false;
		} else {
			// X �Ƿ��ս��
			if (parsingTable[X].find(a) != parsingTable[X].end()) {
				vector<SymbolType> production = parsingTable[X][a];
				action = "���: " + getProductionString(X, production);

				st.pop();

				// ������ʽ�Ҳ�����ѹ��ջ�У��� ��ѹ�룩
				if (production[0] != EPSILON) {
					for (int i = static_cast<int>(production.size()) - 1; i >= 0; i--) {
						st.push(production[i]);
					}
				}
			} else {
				action = "����M[" + symbolToString[X] + ", " + symbolToString[a] + "] Ϊ��";
				cout << "| " << setw(4) << step << " | "
				     << setw(10) << matched << " | "
				     << setw(10) << stackStr << " | "
				     << setw(12) << inputStr << " | "
				     << setw(14) << action << " |" << endl;
				return false;
			}
		}

		// �����ǰ����
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
	cout << "�������봮: " << input << endl << endl;

	bool result = LL1Parser(input);

	if (result) {
		cout << "+---------------------------------------------------------------+" << endl;
		cout << endl << "�����ɹ������봮�ǺϷ��ı��ʽ��" << endl;
	} else {
		cout << endl << "����ʧ�ܣ����봮���ǺϷ��ı��ʽ��" << endl;
	}

	return 0;
}