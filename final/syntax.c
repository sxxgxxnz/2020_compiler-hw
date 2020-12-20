#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "y.tab.h"
//#include "syntax.h"
//#include <stdlib.h>
extern char *yytext;
A_TYPE *int_type, *char_type, *void_type, *float_type, *string_type;
A_NODE *root;
A_ID *current_id=NIL;
int syntax_err=0;
int line_no=1;
int current_level=0;
A_NODE *makeNode (NODE_NAME,A_NODE *,A_NODE *,A_NODE *);
A_NODE *makeNodeList (NODE_NAME,A_NODE *,A_NODE *);
A_ID *makeIdentifier(char *);
A_ID *makeDummyIdentifier();
A_TYPE *makeType(T_KIND);
A_SPECIFIER *makeSpecifier(A_TYPE *,S_KIND);
A_ID *searchIdentifier(char *,A_ID *);
A_ID *searchIdentifierAtCurrentLevel(char *,A_ID *);
A_SPECIFIER *updateSpecifier(A_SPECIFIER *, A_TYPE *, S_KIND);
void checkForwardReference();
void setDefaultSpecifier(A_SPECIFIER *);
A_ID *linkDeclaratorList(A_ID *,A_ID *) ;
A_ID *getIdentifierDeclared(char *);
A_TYPE *getTypeOfStructOrEnumRefIdentifier(T_KIND,char *,ID_KIND);
A_ID *setDeclaratorInit(A_ID *,A_NODE *);
A_ID *setDeclaratorKind(A_ID *,ID_KIND);
A_ID *setDeclaratorType(A_ID *,A_TYPE *);
A_ID *setDeclaratorElementType(A_ID *,A_TYPE *);
A_ID *setDeclaratorTypeAndKind(A_ID *,A_TYPE *,ID_KIND);
A_ID *setDeclaratorListSpecifier(A_ID *,A_SPECIFIER *);
A_ID *setFunctionDeclaratorSpecifier(A_ID *, A_SPECIFIER *);
A_ID *setFunctionDeclaratorBody(A_ID *, A_NODE *);
A_ID *setParameterDeclaratorSpecifier(A_ID *, A_SPECIFIER *);
A_ID *setStructDeclaratorListSpecifier(A_ID *, A_TYPE *);
A_TYPE *setTypeNameSpecifier(A_TYPE *, A_SPECIFIER *);
A_TYPE *setTypeElementType(A_TYPE *,A_TYPE *);
A_TYPE *setTypeField(A_TYPE *,A_ID *);
A_TYPE *setTypeExpr(A_TYPE *,A_NODE *);
A_TYPE *setTypeAndKindOfDeclarator(A_TYPE *,ID_KIND,A_ID *);
A_TYPE *setTypeStructOrEnumIdentifier(T_KIND,char *,ID_KIND);
BOOLEAN isNotSameFormalParameters(A_ID *, A_ID *);
BOOLEAN isNotSameType(A_TYPE *, A_TYPE *);
BOOLEAN isPointerOrArrayType(A_TYPE *);
void syntax_error();
void initialize();

// make new node for syntax tree
A_NODE *makeNode (NODE_NAME n, A_NODE *a, A_NODE *b, A_NODE *c) {
	A_NODE *m;
	m = (A_NODE*)malloc(sizeof(A_NODE));
	m->name=n;
	m->llink=a;
	m->clink=b;
	m->rlink=c;
	m->type=NIL;
	m->line=line_no;
	m->value=0;
	return (m);
}


A_NODE *makeNodeList (NODE_NAME n, A_NODE *a, A_NODE *b) {
// 리스트 형태의 신택스 트리 마지막에 신택스 트리 b를 추가 연결
	A_NODE *m,*k;
	k=a;
	while (k->rlink)
		k=k->rlink;
	
	m = (A_NODE*)malloc(sizeof(A_NODE));
	m->name= k->name;
	m->llink=NIL;
	m->clink=NIL;
	m->rlink=NIL;
	m->type=NIL;
	m->line=line_no;
	m->value=0;
	k->name=n;
	k->llink=b;
	k->rlink=m;
	return(a);
}


// make a new declarator for identifier
A_ID *makeIdentifier(char *s) {
	A_ID *id;
	id = malloc(sizeof(A_ID));
	id->name = s;
	id->kind = 0;
	id->specifier = 0;
	id->level = current_level;
	id->address = 0;
	id->init = NIL;
	id->type = NIL;
	id->link = NIL; // 타입이 나온 줄번호
	id->line = line_no;
	id->value=0;
	id->prev = current_id;
	current_id= id;
	return(id);
}


// make a new declarator for dummy identifier
//심볼테이블을 하나 만드는데 현재 사용할 수 있는 이름에서는 제외
A_ID *makeDummyIdentifier() {
	A_ID *id;
	id = malloc(sizeof(A_ID));
	id->name = "";
	id->kind =0;
	id->specifier = 0;
	id->level = current_level;
	id->address = 0;
	id->init = NIL;
	id->type = NIL;
	id->link = NIL;
	id->line = line_no; // 타입이 나온 줄번호
	id->value=0;
	id->prev =0;
	return(id);
}


// make a new type
A_TYPE *makeType(T_KIND k) {
	A_TYPE *t;
	t = malloc(sizeof(A_TYPE));
	t->kind = k;
	t->size=0;
	t->local_var_size=0;
	t->element_type = NIL;
	t->field = NIL;
	t->expr = NIL;
	t->check=FALSE;
	t->prt=FALSE;
	t->line=line_no; // 타입이 나온 줄번호
	return(t);
}


// make a new specifier
A_SPECIFIER *makeSpecifier(A_TYPE *t, S_KIND s) {
	A_SPECIFIER *p;
	p = malloc(sizeof(A_SPECIFIER));
	p->type = t;
	p->stor=s;
	p->line=line_no;
	return(p);
}


A_ID *searchIdentifier(char *s, A_ID *id) {
	//명칭 목록 id로부터 시작하여 모든 심볼테이블에서 이름 s를 탐색
	
	while(id){
		if(strcmp(id->name,s)==0)
			break;
		id=id->prev;

	}
	return(id);
}


A_ID *searchIdentifierAtCurrentLevel(char *s, A_ID *id) {
// 명칭 목록 id 로부터 시작하여 현재 level 과 같은 level의 명칭 s 를 탐색
	while (id) {
		
		if(id->level<current_level)
			return(0);
		if (strcmp(id->name,s)==0)
			break;
		id=id->prev;
	}
	return(id);
}


void checkForwardReference() {
	A_ID *id;
	A_TYPE *t;
	id=current_id;
// 현재의 level에서 이름의 종류가 정해지지 않았거나
// 미완성 구조체 선언이 있는지 검사
	while (id) {
		if(id->level<current_level)
			break;
		t=id->type;
		if(id->kind==ID_NULL)
			syntax_error(31,id->name);
		else if((id->kind==ID_STRUCT || id->kind==ID_ENUM) && t->field==NIL)
			syntax_error(32,id->name);
		id=id->prev;
	}
}


// set default specifier
void setDefaultSpecifier(A_SPECIFIER *p) {
// 정해지지 않은 specifier p의 타입과 storage_class를 각각 int 와 auto 로
// 지정한다
	A_TYPE *t;
	if(p->type==NIL)
		p->type=int_type;
	if(p->stor==S_NULL)
		p->stor=S_AUTO;
}


// merge & update specifier
A_SPECIFIER *updateSpecifier(A_SPECIFIER *p, A_TYPE *t, S_KIND s) {
	if (t)
		if ( p->type)
			if (p->type==t)
				;
			else
				syntax_error(24);
		else
			p->type=t;
	if (s) {
		if (p->stor)
			if(s==p->stor) ;
			else
				syntax_error(24);
		else
			p->stor=s; }
	return (p);
}


A_ID *linkDeclaratorList(A_ID *id1, A_ID *id2) {
// 심볼테이블 목록 id1 뒤에 목록 id2를 연결한다
	A_ID *m=id1;
	if(id1==NIL)
		return(id2);
	while(m->link)
		m=m->link;
	m->link=id2;
	return (id1);
}


// check if the identifier ( in primary expression) is already declared in the
// declarations
A_ID *getIdentifierDeclared(char *s) {
	A_ID *id;
	id=searchIdentifier(s,current_id);
	if(id==NIL)
		syntax_error(13,s);
	return(id);
}


// get type of struct identifier
A_TYPE * getTypeOfStructOrEnumRefIdentifier(T_KIND k,char *s, ID_KIND kk) {
	A_ID * id;
	A_TYPE *t;
	id=searchIdentifier(s,current_id);
	if (id)
		//struct 혹은 enum으로 정의된 이름인지 검사하고
		//그 타입(테이블 포인터)를 리턴한다.
		if(id->kind=kk && id->type->kind==k)
			return(id->type);
		else
			syntax_error(11,s);
	//make a new struct (or enum) identifier	
	t=makeType(k);
	id=makeIdentifier(s);
	id->kind=kk;
	id->type=t;
	return(t);
}


// 초기화 수식을 위한 신택스 트리를 심볼테이블안에 (init필드) 연결 저장
A_ID *setDeclaratorInit(A_ID *id, A_NODE *n) {
	id->init=n;
	return(id);
}


// set declarator kind
A_ID *setDeclaratorKind(A_ID *id, ID_KIND k) {
	A_ID *a;
// enum의 명칭상수 혹은 파라미터 명칭으로 선언된 declarator 명칭이 중복
// 선언되었는지 검사하고 그 명칭의 종류를 k 로 결정
	a=searchIdentifierAtCurrentLevel(id->name,id->prev);
	if(a)
		syntax_error(12,id->name);
	id->kind=k;
	return(id);
}


// set declarator type
A_ID *setDeclaratorType(A_ID *id, A_TYPE *t) {
	id->type=t;
	return(id);
}


// set declarator type (or element type)
A_ID *setDeclaratorElementType(A_ID *id, A_TYPE *t) {
	A_TYPE *tt;
	//명칭 목록 id의 마지막 타입으로 t를 연결
	
	if(id->type==NIL)
		id->type=t;
	else{
		tt=id->type;
		while(tt->element_type)
			tt=tt->element_type;
		tt->element_type=t;
		}
	return (id);
}


// set declarator element type and kind
A_ID *setDeclaratorTypeAndKind(A_ID *id, A_TYPE *t,ID_KIND k) {
	id=setDeclaratorElementType(id,t);
	id=setDeclaratorKind(id,k);
	return(id);
}


// check function declarator and return type
A_ID *setFunctionDeclaratorSpecifier(A_ID *id, A_SPECIFIER *p) {
	A_ID *a;
	// storage class 검사
	if(p->stor)
		syntax_error(25,"");
	setDefaultSpecifier(p);
	// specifier 가 생략된 경우 보정
	// 명칭이 함수형인가 검사, check function identifier immediately before'(‘
			if(id->type->kind!=T_FUNC){
				syntax_error(21,"");
				return(id);
			}
			else {
				id=setDeclaratorElementType(id,p->type);
				id->kind=ID_FUNC;
			}
	// 함수의 리턴 타입을 완성하고 명칭의 종류를 ID_FUNC 로 지정한다
			a=searchIdentifierAtCurrentLevel(id->name,id->prev);
	// 함수명칭으로 중복선언 검사
			if(a)
				if(a->kind!=ID_FUNC||a->type->expr)
					syntax_error(12,id->name);				// 프로토타입이 선언되어있는 경우 파라미터와 갯수와 타입이 일치하며
	// 리턴 타입이 일치하는지 검사
				else{
					if(isNotSameFormalParameters(a->type->field,id->type->field))
						syntax_error(22,id->name);
					if(isNotSameType(a->type->element_type,id->type->element_type))
						syntax_error(26,a->name);
					}
	// 파라미터를 함수 바디안에서 사용할수 있는 이름으로 스코프 조정
					a=id->type->field;
					while(a){
							if(strlen(a->name))
								current_id=a;
							else if(a->type)
								syntax_error(23,"");
							a=a->link;
						}
	return(id);
}


A_ID *setFunctionDeclaratorBody(A_ID *id, A_NODE *n) {
// 심볼테이블에 (타입이 함수형이라고 가정하고) 바디를 expr 필드에 연결
	id->type->expr=n;
	return(id);
}


// decide the type and kind of the declarator_list based on the storage class
A_ID *setDeclaratorListSpecifier(A_ID *id, A_SPECIFIER *p) {
	A_ID *a;
	// specifier P 의 선언 보정
	setDefaultSpecifier(p);
	a=id;
	// id 로부터 시작하는 심볼테이블 목록에 대하여
	// 중복 선언 검사
	// 명칭의 최종 타입 연결
	// 명칭의 종류로 ID_TYPE, ID_FUNC, 및 ID_VAR 등을 구분하여 저장
	// specifier p 에 있는 명칭의 storage class 저장
	while(a){
		if(strlen(a->name)&&searchIdentifierAtCurrentLevel(a->name,a->prev))
			syntax_error(12,a->name);
		a=setDeclaratorElementType(a,p->type);
		if(p->stor==S_TYPEDEF)
			a->kind=ID_TYPE;
		else if(a->type->kind==T_FUNC)
			a->kind=ID_FUNC;
		else
			a->kind=ID_VAR;
		a->specifier=p->stor;
		if(a->specifier==S_NULL)
			a->specifier=S_AUTO;
		a=a->link;
	}	
	return(id);
}


// set declarator_list type and kind
A_ID *setParameterDeclaratorSpecifier(A_ID *id, A_SPECIFIER *p) {
	// 중복선언 검사
	// 파라미터의 storage class 와 void type 여부 검사
	// 파라미터의 타입 완성
	// 명칭의 종류를 결정 (ID_PARM)
	if(searchIdentifierAtCurrentLevel(id->name,id->prev))
		syntax_error(12,id->name);
	if(p->stor||p->type==void_type)
		syntax_error(14,"");
	setDefaultSpecifier(p);
	id=setDeclaratorElementType(id,p->type);
	id->kind=ID_PARM;
	return(id);
}


A_ID *setStructDeclaratorListSpecifier(A_ID *id, A_TYPE *t) {
	A_ID *a;
	a=id;
	while (a) {
		// 구조체 필드 명칭의 중복선언 검사
		// 필드명칭의 타입완성
		// 명칭의 종류 결정 (ID_FIELD)
		if(searchIdentifierAtCurrentLevel(a->name,a->prev))
			syntax_error(12,a->name);
		a=setDeclaratorElementType(a,t);
		a->kind=ID_FIELD;
		a=a->link; }

	return(id);
}


// set type name specifier
A_TYPE *setTypeNameSpecifier(A_TYPE *t, A_SPECIFIER *p) {
// sizeof (타입) 의 수식에서 타입속에 storage class 가 들어있나 검사하고
// declaration_specifier 부분과 declarator에 나오는 타입 부분을 결합한다
	if(p->stor)
		syntax_error(20);
	setDefaultSpecifier(p);
	t=setTypeElementType(t,p->type);

	return(t);
}


// set type element type
A_TYPE *setTypeElementType(A_TYPE *t, A_TYPE *s) {
	A_TYPE *q;
	// t 의 마지막 원소의 타입으로 s 타입을 연결
	if(t==NIL)
		return(s);
	q=t;
	while(q->element_type)
		q=q->element_type;
	q->element_type=s;
	return(t);
}


// set type field
A_TYPE *setTypeField(A_TYPE *t, A_ID *n) {
	// 타입테이블의 field 칸에 명칭목록을 연결 저장
	t->field=n;
	return(t);
}


// set type initial value (expression tree)
A_TYPE *setTypeExpr(A_TYPE *t, A_NODE *n) {
	t->expr=n;
	return(t);
}


// set type of struct iIdentifier
A_TYPE *setTypeStructOrEnumIdentifier(T_KIND k, char *s, ID_KIND kk) {
	A_TYPE *t;
	A_ID *id, *a;
	// 구조체나 enum 태그명칭이 새로운 선언이면 새로운 심볼테이블과 관련된
	// 타입테이블을 만들어 연결하고
	// 이미 나온 이름인 경우 중복선언 혹은 전방 참조여 부를 검사
	a=searchIdentifierAtCurrentLevel(s,current_id);
	if(a)
		if(a->kind==kk && a->type->kind==k)
			if(a->type->field)
				syntax_error(12,s);
			else
				return(a->type);
		else
			syntax_error(12,s);
	id=makeIdentifier(s);
	t=makeType(k);
	id->type=t;
	id->kind=kk;

	return(t);
}


// set type and kinf of identifier
A_TYPE *setTypeAndKindOfDeclarator(A_TYPE *t, ID_KIND k, A_ID *id) {
// 중복 선언된 이름이 아닌경우에 그 타입과 종류를 저장
//
	if(searchIdentifierAtCurrentLevel(id->name,id->prev))
		syntax_error(12,id->name);
	id->type=t;
	id->kind=k;

	return(t);
}


// check function parameters with protype
// 함수선언의 파라미터와 프로토타입의 파라미터의 타입과 개수가 일치하는지 검사
BOOLEAN isNotSameFormalParameters(A_ID *a, A_ID *b) {
	if (a==NIL) // no parameters in prototype
		return(FALSE);
	while(a) {
		if (b==NIL || isNotSameType(a->type,b->type))
			return(TRUE);
		a=a->link;
		b=b->link; }
	if (b)
		return(TRUE);
	else
		return(FALSE);
}


BOOLEAN isNotSameType(A_TYPE *t1, A_TYPE *t2) {
	if (isPointerOrArrayType(t1) || isPointerOrArrayType(t2))
		return (isNotSameType(t1->element_type,t2->element_type));
	else
		return (t1!=t2);
}
BOOLEAN isPointerOrArrayType(A_TYPE *t) {
	if(t->kind==T_POINTER || t->kind==T_ARRAY)
		return TRUE;
	else
		return FALSE;

}


void initialize() {
// 기본 타입 (int, float, char, void) 과 가상적인 스트링 타입을 위한
// 타입테이블 생성 및 초기화
	int_type=setTypeAndKindOfDeclarator(
	makeType(T_ENUM),ID_TYPE,makeIdentifier("int"));
	float_type=setTypeAndKindOfDeclarator(
	makeType(T_ENUM),ID_TYPE,makeIdentifier("float"));
	char_type= setTypeAndKindOfDeclarator(
	makeType(T_ENUM),ID_TYPE,makeIdentifier("char"));
	void_type=setTypeAndKindOfDeclarator(
	makeType(T_VOID),ID_TYPE,makeIdentifier("void"));
	string_type=setTypeElementType(makeType(T_POINTER),char_type);
	int_type->size=4; int_type->check=TRUE;
	float_type->size=4; float_type->check=TRUE;
	char_type->size=1; char_type->check=TRUE;
	void_type->size=0; void_type->check=TRUE;
	string_type->size=4; string_type->check=TRUE;
	// 라이브러리 함수 명칭과 그 파라미터와 리턴타입등을 미리 심볼테이블에
	// 넣어 사용할수 있게 초기화
	// printf(char *, ...) library function
	setDeclaratorTypeAndKind(
		makeIdentifier("printf"),
		setTypeField(
			setTypeElementType(makeType(T_FUNC),void_type),
			linkDeclaratorList(
				setDeclaratorTypeAndKind(makeDummyIdentifier(),string_type,ID_PARM),
	setDeclaratorKind(makeDummyIdentifier(),ID_PARM))),
ID_FUNC);
// scanf(char *, ...) library function
setDeclaratorTypeAndKind(
	makeIdentifier("scanf"),
	setTypeField(
		setTypeElementType(makeType(T_FUNC),void_type),
		linkDeclaratorList(
setDeclaratorTypeAndKind(makeDummyIdentifier(),string_type,ID_PARM),
setDeclaratorKind(makeDummyIdentifier(),ID_PARM))),
ID_FUNC);
// malloc(int) library function
setDeclaratorTypeAndKind(
makeIdentifier("malloc"),
setTypeField(
setTypeElementType(makeType(T_FUNC),string_type),
setDeclaratorTypeAndKind(makeDummyIdentifier(),int_type,ID_PARM)),
ID_FUNC);
}


void syntax_error(int i,char *s) {
	syntax_err++;
	printf("line %d: syntax error: ", line_no);
	switch (i) {
		case 11: printf("illegal referencing struct or union identifier %s",s);
		break;
		case 12: printf("redeclaration of identifier %s",s); break;
		case 13: printf("undefined identifier %s",s); break;
		case 14: printf("illegal type specifier in formal parameter"); break;
		case 20: printf("illegal storage class in type specifiers"); break;
		case 21: printf("illegal function declarator"); break;
		case 22: printf("conflicting parm type in prototype function %s",s);
				break;
		case 23: printf("empty parameter name"); break;
		case 24: printf("illegal declaration specifiers"); break;
		case 25: printf("illegal function specifiers"); break;
		case 26: printf("illegal or conflicting return type in function %s",s);
			break;
		case 31: printf("undefined type for identifier %s",s); break;
		case 32: printf("incomplete forward reference for identifier %s",s);
			break;
		default: printf("unknown"); break;
	}
	if (strlen(yytext)==0)
		printf(" at end\n");
	else
		printf(" near %s\n", yytext);
	exit(1);
	}
