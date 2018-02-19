/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * declarations(void);
static TreeNode * decl(void);
static TreeNode * type_specifier(void);
static TreeNode * varlist(void);

static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * dowhile_stmt(void); // add code
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);

/* 输出错误所在行和相关消息 */
static void syntaxError(char * message) {
	fprintf(listing, "\n>>> ");
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);
	Error = TRUE;
}

static void match(TokenType expected) {
	if (token == expected) token = getToken();
	else {
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		fprintf(listing, "      ");
	}
}

TreeNode * declarations(void) {
	//对文法：decl;declarations
	//		| ε
	//的处理
	if ((token != INT) && (token != BOOL) && (token != STRING)) {
		return NULL;
	}
	TreeNode * t = decl();
	TreeNode * p = t;
	while ((token == INT) || (token == BOOL) || (token == STRING)) {
		TreeNode * q;
		//match(SEMI);
		q = decl();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else /* now p cannot be NULL either */
			{
				p->sibling = q; // 下一个 declaration 是右兄弟结点,形成同一层级
				p = q;
			}
		}
	}
	return t;
}

TreeNode * decl(void) {
	//文法： type_specifier varlist
	TreeNode * t = NULL;
	t = type_specifier();
	if (t != NULL) {
		t->child[0] = varlist();
	}
	return t;
}

TreeNode * type_specifier(void) {
	// 数据类型有三种： int | bool | string
	TreeNode * t = NULL;
	switch (token) {
	case INT: 
		match(INT);
		t = newExpNode(TypeK);
		if ((t != NULL))
			t->attr.name = copyString("int");
		break;
	case BOOL: 
		match(BOOL); 
		t = newExpNode(TypeK);
		if ((t != NULL))
			t->attr.name = copyString("bool");
		break;
	case STRING: 
		match(STRING); 
		t = newExpNode(TypeK);
		if ((t != NULL))
			t->attr.name = copyString("string");
		break;
	default: 
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	} /* end case */
	return t;
}

TreeNode * varlist(void) {
	// 文法： identifier{, identifier}
	TreeNode * t = NULL;
	t = newExpNode(IdK);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	TreeNode * p = t;
	while (token == COMMA) {
		match(COMMA);
		TreeNode * q = NULL;
		q = newExpNode(IdK);
		if ((q != NULL) && (token == ID)) {
			q->attr.name = copyString(tokenString);
			p->child[0] = q;
			p = q;
			match(ID);
		}
	}
	if (token == SEMI) {
		match(SEMI);
	}
	return t;
}

TreeNode * stmt_sequence(void) {
	// 形成一棵以第一条语句开始的参数语法树
	TreeNode * t = statement();
	TreeNode * p = t;
	while ((token != ENDFILE) && (token != END) &&
		(token != ELSE) && (token != UNTIL) && (token != WHILE)) {
		TreeNode * q;
		match(SEMI);
		q = statement();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else /* now p cannot be NULL either */
			{
				p->sibling = q; // 下一个语句是右兄弟结点,形成同一层级
				p = q;
			}
		}
	}
	return t;
}

TreeNode * statement(void) {
	TreeNode * t = NULL;
	switch (token) {
	case IF: t = if_stmt(); break;
	case REPEAT: t = repeat_stmt(); break;
	case ID: t = assign_stmt(); break;
	case READ: t = read_stmt(); break;
	case WRITE: t = write_stmt(); break;

	// 添加do-while控制结构
	case DO: t = dowhile_stmt(); break;
	
	default: 
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	} /* end case */
	return t;
}

// 扩充的do-while语句文法
TreeNode* dowhile_stmt(void) {	
	//对文法 ：	DO stmt_seq WHILE exp
	//的处理
	TreeNode * t = newStmtNode(DoWhileK);
	match(DO);
	if (t != NULL) t->child[0] = stmt_sequence();
	match(WHILE);
	if (t != NULL) t->child[1] = exp();
	return t;
}

TreeNode * if_stmt(void) {
	//对文法  :	IF exp THEN stmt_seq END
	//			IF exp THEN stmt_seq ELSE stmt_seq END
	//的处理
	TreeNode * t = newStmtNode(IfK);
	match(IF);
	if (t != NULL) t->child[0] = exp();
	match(THEN);
	if (t != NULL) t->child[1] = stmt_sequence();
	if (token == ELSE) {
		match(ELSE);
		if (t != NULL) t->child[2] = stmt_sequence();
	}
	match(END);
	return t;
}

TreeNode * repeat_stmt(void) {
	//对文法:REPEAT stmt_seq UNTIL exp
	//的处理
	TreeNode * t = newStmtNode(RepeatK);
	match(REPEAT);
	if (t != NULL) t->child[0] = stmt_sequence();
	match(UNTIL);
	if (t != NULL) t->child[1] = exp();
	return t;
}

TreeNode * assign_stmt(void) {
	//对文法: ID ASSIGN exp
	//的处理
	TreeNode * t = newStmtNode(AssignK);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = exp();
	return t;
}

TreeNode * read_stmt(void) {
	//对文法: READ ID
	//的处理
	TreeNode * t = newStmtNode(ReadK);
	match(READ);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	return t;
}

TreeNode * write_stmt(void) {
	//对文法: WRITE exp
	//的处理
	TreeNode * t = newStmtNode(WriteK);
	match(WRITE);
	if (t != NULL) t->child[0] = exp();
	return t;
}

TreeNode * exp(void) {
	//对文法:exp: simple_exp LT simple_exp
	//          | simple_exp EQ simple_exp
	//          | simple_exp
	//的处理
	// 先生成了左边的子表达式
	TreeNode * t = simple_exp();
	if ((token == LT) || (token == EQ) 
		|| (token == LTE) || (token == GTE)) {
		TreeNode * p = newExpNode(OpK); // 操作符表达式结点
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token; //操作符类型
			t = p; //t是需要返回的
		}
		match(token);
		if (t != NULL)
			t->child[1] = simple_exp();	// 再生成右边的子表达式
	}
	return t;
}

TreeNode * simple_exp(void) {
	//对文法:simple_exp  : simple_exp PLUS term
	//                   | simple_exp MINUS term
	//                   | term
	//的处理
	//先生成term项
	TreeNode * t = term();
	while ((token == PLUS) || (token == MINUS)) {
		TreeNode * p = newExpNode(OpK); // 构造操作符表达式结点
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			t->child[1] = term();
		}
	}
	return t;
}

TreeNode * term(void) {
	//对文法: term   : term TIMES factor
	//               | term OVER factor
	//               | factor
	//的处理
	TreeNode * t = factor();
	while ((token == TIMES) || (token == OVER)) {
		TreeNode * p = newExpNode(OpK); // 构造操作符表达式结点
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			p->child[1] = factor();
		}
	}
	return t;
}

TreeNode * factor(void) { //判断单词的类型, NUM | ID
	TreeNode * t = NULL;
	switch (token) {
	case BOOL:
		t = newExpNode(BoolK);
		if ((t != NULL) && (token == BOOL)) {
			t->attr.name = copyString(tokenString);
		}
		match(BOOL);
		break;
	case STR:
		t = newExpNode(StrK);
		if ((t != NULL) && (token == STR))
			t->attr.name = copyString(tokenString);
		match(STR);
		break;
	case NUM:
		t = newExpNode(ConstK);
		if ((t != NULL) && (token == NUM))
			t->attr.val = atoi(tokenString);
		match(NUM);
		break;
	case ID:
		t = newExpNode(IdK);
		if ((t != NULL) && (token == ID))
			t->attr.name = copyString(tokenString);
		match(ID);
		break;
	case LPAREN:
		match(LPAREN);
		t = exp();
		match(RPAREN);
		break;
	default:
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	}
	return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode * parse(void) {
	TreeNode * t;
	token = getToken();
	t = declarations();
	if (t != NULL) {
		TreeNode * p = t;
		while (p->sibling != NULL)p = p->sibling;
		p->sibling = stmt_sequence();
	}
	else {
		t = stmt_sequence();
	}
	if (token != ENDFILE)
		syntaxError("Code ends before file\n");
	return t;
}
