#include<stdio.h>
void A(){
    B();B();B();B();B();B();B();
    E();E();
}
void B(){
    C();C();C();C();C();C();
}
void C(){
    D();D();D();D();D();
    int a=16;int b=16;int c=32;int d=64;
        c = a+b;
        c = c-b;
        a = c*b;
        b = a/c;
        c = a*b;
        d = d*c;
        d = d-a;
        a = d*b;
        c = a*d;
}
void D(){
    printf("D\n");
    int a=16;int b=16;int c=32;int d=64;
        c = a+b;
        c = c-b;
        a = c*b;
        b = a/c;
        c = a*b;
        d = d*c;
        d = d-a;
        a = d*b;
        c = a*d;
        c = a+b;
}
void E(){
    C();C();C();C();C();C();C();C();
    F();
    int a=16;int b=16;int c=32;int d=64;
        c = a+b;
        c = c-b;
        a = c*b;
b = a/c;
        c = a*b;
        d = d*c;
        d = d-a;
        a = d*b;
        c = a*d;
        c = a+b;
        c = c-b;

}

void F(){
    G();
}

void G(){
    printf("G\n");
    int a=16;int b=16;int c=32;int d=64;
        c = a+b;
        c = c-b;
        a = c*b;
        b = a/c;
        c = a*b;
        d = d*c;
        d = d-a;
        a = d*b;
c = a*d;
        c = a+b;
        c = c-b;
        a = c*b;

}

int main(){
    A();
    return 0;
}


