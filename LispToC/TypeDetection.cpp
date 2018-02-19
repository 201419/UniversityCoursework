#include <iostream>
#include <string>
using namespace std;

#define MAXRESERVED 8

// ���ʵ����ͱ��
typedef enum { ENDFILE, ERROR, ADD, SUB, GT, LT, EQU, AND, OR, NOT, BOOL, NUM } TokenType;

// �����ֵĶ��壬��������
static struct
{
	string str;
	TokenType tok;
} reservedWords[MAXRESERVED] = { { "add",ADD },{ "sub",SUB },{ "gt",GT },{ "lt",LT }, { "equ",EQU },{ "and",AND },{ "or",OR },{ "not",NOT } };

// ����״̬����״̬����
typedef enum { START, INNUM, INID, INBOOL, DONE } StateType;

// �﷨���Ľڵ�����
typedef enum { OpK, NumK, BoolK } NodeKind;

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
bool isBool;
bool isError;
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
			else if (c == 'T' || c == 'F') { state = INBOOL; currentKind = BoolK; }
			else if (isalpha(c)) { state = INID; currentKind = OpK; }
			else if (c == ' ' || c == '(' || c == ')') continue;
		case INNUM:
			if (c == ' ' || c == ')') { state = DONE; save = false; }
			break;
		case INID:
			if (c == ' ' || c == ')') { state = DONE; save = false; }
			break;
		case INBOOL:
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
			if (currentKind == BoolK) currentTokenType = BOOL;
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
		else if (currentTokenType == ERROR || currentTokenType==BOOL) {
			p = new TreeNode();
			p->nodekind = currentTokenType;
			p->str = currentToken;
		}
		else { p = parse(); }
		t->child[0] = p;
	}

	if (str[pos] != ')') {
		getToken();
		if (currentTokenType != ENDFILE) {
			TreeNode * p = NULL;
			if (currentTokenType == NUM) {
				p = new TreeNode();
				p->nodekind = currentTokenType;
				p->value = atoi(currentToken.c_str());
			}
			else if (currentTokenType == ERROR || currentTokenType == BOOL) {
				p = new TreeNode();
				p->nodekind = currentTokenType;
				p->str = currentToken;
			}
			else { p = parse(); }
			t->child[1] = p;
		}
	}

	return t;
}

// �﷨���ĸ�ʽ�����
void printTree(TreeNode * tree) {
	if (tree->nodekind == NUM) { cout << tree->value; }
	else if (tree->nodekind == ERROR || tree->nodekind == BOOL) { cout << tree->str; }
	else { cout << tree->str << "("; }

	if (tree->child[0] != NULL) { 
		printTree(tree->child[0]); 
		if(tree->child[1] == NULL){ cout << ")"; }
	}
	if (tree->child[1] != NULL) { cout << ", "; printTree(tree->child[1]); cout << ")";} 
}

// ���ʽ�����ͼ��
void typeDetection(TreeNode * tree) {
	if (tree->nodekind == NUM) { isDigit = true; }
	else if (tree->nodekind == BOOL) { isBool = true;}
	else {
		if (tree->child[0] != NULL) { typeDetection(tree->child[0]); }
		if (tree->child[1] != NULL) { typeDetection(tree->child[1]); }
		switch (tree->nodekind) {
		case ADD: if (isBool || isError) { isError = true; } break;
		case SUB: if (isBool || isError) { isError = true; } break;
		case GT: if (isBool || isError) { isError = true; } else { isBool=true; } break;
		case LT: if (isBool || isError) { isError = true; } else { isBool=true; } break;
		case EQU: if (isBool || isError) { isError = true; } else { isBool=true; } break;
		case AND: if (isDigit || isError) { isError = true; } break;
		case OR: if (isDigit || isError) { isError = true; } break;
		case NOT: if (isDigit || isError) { isError = true; } break;
		}
	}
}

int main() {
	while (true) {
		getline(cin, str); // ��ȡ����

		// �����﷨��������������Խ��з���
		pos = 0;
		getToken();
		TreeNode * syntaxTree;
		syntaxTree = parse();

		// ���ʽ�����ͼ�⼰����������
		printTree(syntaxTree);
		/*cout << endl;*/ cout << " : ";
		isDigit = false;
		isBool = false;
		isError = false;
		typeDetection(syntaxTree);
		if (isError) { cout << "error" << endl; }
		else if (isBool) { cout << "bool" << endl; }
		else if (isDigit) { cout << "int" << endl; }
	}
	system("pause");
	return 0;
}
