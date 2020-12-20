#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define MAX_SIZE 20

int num;
char* ptr;
enum token {
    NUMBER, PLUS, STAR, LPAREN, RPAREN, END }token;
enum t {
    INTTYPE,FLOATTYPE
};
static struct s {
    enum t type;
   // enum token token;
    union {
        int i;
        float f;
    }value;
}s1;

void* get_token(char* ptr);
int term(char* ptr);
int factor(char* ptr);
int expression(char* ptr);


void cError(int i);

void* get_token(char* ptr) {
    struct s s1;
    char tmp = 0;
    //static char ch=' ';
    int j = 1;
    
    
    //ptr = ch;
    //ptr = get_token(ptr);

    if ('0'<=ptr&&'9'>=ptr) {
        do {
            num = ptr - '0';
            tmp = ptr;
          // printf("%d\n", tmp);
            *ptr= getchar();
            
            //정수 두자릿수까지만 받을수 있음
            if ('0'<=ptr&&'9'>=ptr) {
                int k = 10;
                
                j+=1;
                tmp *= pow(k, j);
                ptr += tmp;
                s1.value.i = ptr - '0';
                num = s1.value.i;
                ptr = getchar();
                s1.type = INTTYPE;

                
                return (NUMBER);
            }
            
        } while ('0'<=ptr&&'9'>=ptr);
        
        if (ptr == '.') {
            //실수는 첫째자리까지만 받음
            ptr = getchar();
            
            s1.value.f = atof(ptr);
            s1.value.f /= 10;
            (float)num += s1.value.f;

            ptr = getchar();
            s1.type = FLOATTYPE;
            return (NUMBER);
            
        }
    }
    else if (ptr == '+') {
        //token = PLUS;
       
        return (PLUS);
    }
    else if (ptr == '*') {
        return (STAR);
    }
    else if (ptr == '(') {
        return LPAREN;
    }
    else if (ptr == ')') {
        return  RPAREN;
    }
    else if (ptr == EOF) {
        return  END;
    }
    else
        cError(1);

    *(ptr++);
    return ptr;
}


int expression(char* ptr) {
    int result;

    result = term(ptr);
    while (token == PLUS) {

        ptr=get_token(ptr);
        result = result + term(ptr);
    }
    return (result);
}
int term(char* ptr) {
    int result;
    result = factor(ptr);
    while (token == STAR) {
        ptr=get_token(ptr);
        result = result * factor(ptr);
    }
    return (result);
}
int factor(char* ptr) {
    int result;

    if (token == NUMBER) {
        result = num;

         ptr=get_token(ptr);


    }
    else if (token == LPAREN) {
         ptr=get_token(ptr);
        result = expression(ptr);
        if (token == RPAREN)
            ptr=get_token(ptr);
        else
            cError(2);
    }
    else
        cError(1);
    return (result);
}

void main() {
    
    char* ptr = (char*)malloc(sizeof(char) * MAX_SIZE);
    printf("input :");
   char ch = getchar();
    
    ptr=get_token(ch);
    int result;
    result = expression(ptr);

    if (token != END)
        cError(3);
    else
        printf("%d\n", result);

    free(ptr);
}


void cError(int i) {
    switch (i) {
    case 1: printf("error: number or '(' expected\n"); break;
    case 2: printf("error: ')' expected\n"); break;
    case 3: printf("error: EOF expected\n"); break;
    }
    exit(1);
}
