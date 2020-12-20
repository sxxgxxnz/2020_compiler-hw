digit [0-9]
letter [a-zA-Z_]
delim [ \t]
line [\n]
ws {delim}+
%{
#include "y.tab.h"
#include "type.h"
extern int yylval;
extern int line_no;
extern A_ID *current_id;
char *makeString();
int checkIdentifier();
%}
%%
{ws} { }
{line} { line_no++;}
auto { return(AUTO_SYM); }
break { return(BREAK_SYM); }
case { return(CASE_SYM); }
continue { return(CONTINUE_SYM); }
default { return(DEFAULT_SYM); }
do { return(DO_SYM); }
else { return(ELSE_SYM); }
enum { return(ENUM_SYM); }
for { return(FOR_SYM); }
if { return(IF_SYM); }
return { return(RETURN_SYM); }
sizeof { return(SIZEOF_SYM); }
static { return(STATIC_SYM); }
struct { return(STRUCT_SYM); }
switch { return(SWITCH_SYM); }
typedef { return(TYPEDEF_SYM); }
union { return(UNION_SYM); }
while { return(WHILE_SYM); }
"\+\+" { return(PLUSPLUS); }
"\-\-" { return(MINUSMINUS); }
"\->" { return(ARROW); }
"<" { return(LSS); }
">" { return(GTR); }
"<=" { return(LEQ); }
">=" { return(GEQ); }
"==" { return(EQL); }
"!=" { return(NEQ); }
"&&" { return(AMPAMP); }
"||" { return(BARBAR); }
"\.\.\." { return(DOTDOTDOT); }
"\(" { return(LP); }
"\)" { return(RP); }
"\[" { return(LB); }
"\]" { return(RB); }
"\{" { return(LR); }
"\}" { return(RR); }
"\:" { return(COLON); }
"\." { return(PERIOD); }
"\," { return(COMMA); }
"\!" { return(EXCL); }
"\*" { return(STAR); }
"\/" { return(SLASH); }
"\%" { return(PERCENT); }
"\&" { return(AMP); }
"\;" { return(SEMICOLON); }
"\+" { return(PLUS); }
"\-" { return(MINUS); }
"\=" { return(ASSIGN); }
{digit}+ { yylval=atoi(yytext); return(INTEGER_CONSTANT);}
{digit}+\.{digit}+ { yylval=makeString(yytext); return(FLOAT_CONSTANT);}
{letter}({letter}|{digit})* { return(checkIdentifier(yytext)); }
\"([^"\n]|\\["\n])*\" { yylval=makeString(yytext); return(STRING_LITERAL);}
\'([^'\n]|\'\')\' { yylval=*(yytext+1); return(CHARACTER_CONSTANT);}
"//"[^\n]* { }
%%

char *makeString(char *s) {
	char *t;
	t=malloc(strlen(s)+1);
	strcpy(t,s);
	return(t);
}


int checkIdentifier(char *s) {
// current_id 로부터 시작하여 파라미터로 주어진 이름(s)을 검색
// 이름이 없으면 IDENTIFIER 로 리턴
// 이름 스트링을 힙에 저장하고, yylval 은 그 스트링 포인터
// 이름이 있는 이미 경우는
// 이름의 종류가 typedef(ID_TYPE)로 정의한 이름인 경우는
//TYPE_IDENTIFIER 로 리턴, yylval 은 그 타입 포인터
// 아닐때는 IDENTIFIER 로 리턴, yylval 은 그 이름의 스트링 포인터
	A_ID *id;
	char *t;
	id=current_id;
	while(id){
		if(strcmp(id->name,s)==0)
			break;
		id=id->prev;
	}
	if(id==0){
		yylval=makeString(s);
		return(IDENTIFIER);
	}
	else if(id->kind==ID_TYPE){
		yylval=id->type;
		return(TYPE_IDENTIFIER);
	}
	else{
		yylval=id->name;
		return(IDENTIFIER);
	}


}
