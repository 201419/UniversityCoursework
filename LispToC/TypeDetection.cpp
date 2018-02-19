#include <iostream>
#include <string>
using namespace std;

#define MAXRESERVED 8

// 单词的类型标记
typedef enum { ENDFILE, ERROR, ADD, SUB, GT, LT, EQU, AND, OR, NOT, BOOL, NUM } TokenType;

// 保留字的定义，即操作符
static struct
{
	string str;
	TokenType tok;
} reservedWords[MAXRESERVED] = { { "add",ADD },{ "sub",SUB },{ "gt",GT },{ "lt",LT }, { "equ",EQU },{ "and",AND },{ "or",OR },{ "not",NOT } };

// 有限状态机的状态类型
typedef enum { START, INNUM, INID, INBOOL, DONE } StateType;

// 语法树的节点类型
typedef enum { OpK, NumK, BoolK } NodeKind;

// 语法树的节点结构
typedef struct treeNode
{
	struct treeNode * child[2];
	//struct treeNode * sibling; /* 当参数不只有两个的时候，可利用兄弟节点来创建任意个节点 */
	TokenType nodekind;
	int value;
	string str;
} TreeNode;

// 全局变量
string str;
int pos;
bool isDigit;
bool isBool;
bool isError;
string currentToken;
TokenType currentTokenType;

// 保留字查询函数，如果找不到即返回ERROR
static TokenType reservedLookup(string s) {
	for (int i = 0; i < MAXRESERVED; i++)
		if (s == reservedWords[i].str)
			return reservedWords[i].tok;
	return ERROR;
}

// 词法分析器，进行单词的切割
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

// 语法分析器，用于构建语法树
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

// 语法树的格式化输出
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

// 表达式的类型检测
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
		getline(cin, str); // 读取输入

		// 构建语法树，对输入的语言进行分析
		pos = 0;
		getToken();
		TreeNode * syntaxTree;
		syntaxTree = parse();

		// 表达式的类型检测及检测结果的输出
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
