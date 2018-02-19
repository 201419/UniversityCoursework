#include <iostream>
#include <string>
using namespace std;

#define MAXRESERVED 5

// ���ʵ����ͱ��
typedef enum { ENDFILE, ERROR, ADD, SUB, MUL, DIV, MOD, NUM } TokenType;

// �����ֵĶ��壬��������
static struct
{
	string str;
	TokenType tok;
} reservedWords[MAXRESERVED] = { { "add",ADD },{ "sub",SUB },{ "mul",MUL },{ "div",DIV },{ "mod",MOD } };

// ����״̬����״̬����
typedef enum { START, INNUM, INID, DONE } StateType;

// �﷨���Ľڵ�����
typedef enum { OpK, NumK } NodeKind;

// �﷨���Ľڵ�ṹ
typedef struct treeNode
{
	struct treeNode * child[2];
	//struct treeNode * sibling; /* ��������ֻ��������ʱ�򣬿������ֵܽڵ�������������ڵ� */
	TokenType nodekind;
	int value;
	string str;
} TreeNode;

// ȫ�ֱ���
string str;
int pos;
bool isDigit;
string currentToken;
TokenType currentTokenType;

// �����ֲ�ѯ����������Ҳ���������ERROR
static TokenType reservedLookup(string s) {
	for (int i = 0; i < MAXRESERVED; i++)
		if (s == reservedWords[i].str)
			return reservedWords[i].tok;
	return ERROR;
}

// �ʷ������������е��ʵ��и�
void getToken() {
	NodeKind currentKind;
	StateType state = START;
	bool save;
	currentToken = "";

	// DFA
	while (state != DONE) {
		if (pos == str.length()) { currentTokenType = ENDFILE; break; }
		
		int c = str[pos++];
		save = true;
		
		switch (state) {
		case START:
			if (isdigit(c)) { state = INNUM; currentKind = NumK; }
			else if (isalpha(c)) { state = INID; currentKind = OpK; }
			else if (c == ' ' || c == '(' || c == ')') continue;
		case INNUM:
			if (c == ' ' || c == ')') { state = DONE; save = false; }
			break;
		case INID:
			if (c == ' ' || c == ')') { state = DONE; save = false; }
			break;
		case DONE:
		default:
			break;
		}
		if (save) { currentToken += (char)c; }
		if (state == DONE) {
			if (currentKind == OpK) currentTokenType = reservedLookup(currentToken);
			if (currentKind == NumK) currentTokenType = NUM;
		}
	}
}

// �﷨�����������ڹ����﷨��
TreeNode * parse() {
	TreeNode * t = NULL;
	
	if (currentTokenType != ENDFILE) {
		t = new TreeNode();
		t->nodekind = currentTokenType;
		t->str = currentToken;
	}

	getToken();
	if (currentTokenType != ENDFILE) {
		TreeNode * p = NULL;
		if (currentTokenType == NUM) {
			p = new TreeNode();
			p->nodekind = currentTokenType;
			p->value = atoi(currentToken.c_str());
		}
		else if (currentTokenType == ERROR) {
			p = new TreeNode();
			p->nodekind = currentTokenType;
			p->str = currentToken;
		}
		else { p = parse(); }
		t->child[0] = p;
	}

	getToken();
	if (currentTokenType != ENDFILE) {
		TreeNode * p = NULL;
		if (currentTokenType == NUM) {
			p = new TreeNode();
			p->nodekind = currentTokenType;
			p->value = atoi(currentToken.c_str());
		}
		else if (currentTokenType == ERROR) {
			p = new TreeNode();
			p->nodekind = currentTokenType;
			p->str = currentToken;
		}
		else { p = parse(); }
		t->child[1] = p;
	}

	return t;
}

// �﷨���ĸ�ʽ�����
void printTree(TreeNode * tree) {
	if (tree->nodekind == NUM) { cout << tree->value; }
	else if (tree->nodekind == ERROR) { cout << tree->str; }
	else { cout << tree->str << "("; }

	if (tree->child[0] != NULL) { printTree(tree->child[0]); cout << ", "; }
	if (tree->child[1] != NULL) { printTree(tree->child[1]); cout << ")"; }
}

// ���ʽ�Ľ������
int Calculate(TreeNode * tree) {
	if (tree->nodekind == NUM) { return tree->value; }
	else if (tree->nodekind == ERROR) { isDigit = false; return NULL; }
	else {
		int a, b, c;
		a = Calculate(tree->child[0]);
		b = Calculate(tree->child[1]);
		switch (tree->nodekind) {
		case ADD: c = a + b; break;
		case SUB: c = a - b; break;
		case MUL: c = a * b; break;
		case DIV: if (b == 0) { isDigit = false; return NULL; } c = a / b; break;
		case MOD: if (b == 0) { isDigit = false; return NULL; } c = a % b; break;
		}
		return c;
	}
}

int main() {
	while (true) {
		getline(cin, str);  // ��ȡ����

		// �����﷨��������������Խ��з���
		pos = 0;
		getToken();
		TreeNode * syntaxTree;
		syntaxTree = parse();

		// ���ʽ�ĸ�ʽת����������㼰��Ӧ�����
		printTree(syntaxTree);
		cout << endl;
		isDigit = true;
		int num = Calculate(syntaxTree);
		if (isDigit) { cout << num << endl; }
		else { cout << "e" << endl; }
		cout << endl;
	}
	system("pause");
	return 0;
}
