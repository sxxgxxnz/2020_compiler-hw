#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 20

int num;
char* arr;
enum { NUMBER, PLUS, STAR, LPAREN, RPAREN, END } token;
void* get_token(char* arr);
int term(char* arr);
int factor(char* arr);
int expression(char* arr);
void resultfunc(char* arr);

void cError(int i);

void* get_token(char* arr) {

    char tmp = 0;


    if ('0' <= (*arr) && (*arr) <= '9') {
        token = NUMBER;
        num = (*arr) - '0';

    }
    else if ((*arr) == '+') {
        token = PLUS;
    }
    else if ((*arr) == '*') {
        token = STAR;
    }
    else if ((*arr) == '(') {
        token = LPAREN;
    }
    else if ((*arr) == ')') {
        token = RPAREN;
    }
    else if ((*arr) == EOF) {
        token = END;
    }

    tmp = *(++arr);
    return arr;
}


int expression(char* arr) {
    int result;

    result = term(arr);
    while (token == PLUS) {

        arr = get_token(arr);
        result = result + term(arr);
    }
    return (result);
}
int term(char* arr) {
    int result;
    result = factor(arr);
    while (token == STAR) {
        arr = get_token(arr);
        result = result * factor(arr);
    }
    return (result);
}
int factor(char* arr) {
    int result;
    
    if (token == NUMBER) {
        result = num;

        arr = get_token(arr);


    }
    else if (token == LPAREN) {
        arr = get_token(arr);
        result = expression(arr);
        if (token == RPAREN)
            get_token(arr);
        else
            cError(2);
    }
    else
        cError(1);
    return (result);
}

void main() {
    char* arr = (char*)malloc(sizeof(char) * MAX_SIZE);

    printf("input :");
    scanf("%s", arr);

    arr = get_token(arr);

    resultfunc(arr);

    free(arr);
}

void resultfunc(char* arr) {
    int result;
    result = expression(arr);

    if (token != END)
        cError(3);
    else
        printf("%d\n", result);
    return arr;

}
void cError(int i) {
    switch (i) {
    case 1: printf("error: number or '(' expected\n"); break;
    case 2: printf("error: ')' expected\n"); break;
    case 3: printf("error: EOF expected\n"); break;
    }
    exit(1);
}
