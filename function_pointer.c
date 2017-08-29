=================================================================================
/*
 * 함수 포인터
 * 함수포인터를 함수의 인자로 사용할 수 있다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//문자열을 대문자로 수정한다.
void toUpper(char *str)
{
    while(*str)
    {
        *str = toupper(*str);
        str++;
    }
}

//문자열을 소문자로 수정한다.
void toLower(char *str)
{
    while(*str)
    {
        *str = tolower(*str);
        str++;
    }
}

//함수 포인터를 typedef로 사용하기 편하게 정의한다.
typedef void (*FP)(char *);

//함수 포인터를 인자로 받는다.
void show(FP f, char *str)
{
    f(str);
    puts(str);
}

int main(void) {
    setbuf(stdout, NULL);

    char str[100]="This is LG CNS";
    char ans;

    puts("대문자로 만들려면 u, 소문자로 만들려면 l을 입력하시오:");
    ans = getchar();

    FP fp;

    switch(ans)
    {
        case 'u':
        case 'U':
            fp = toUpper;
            break;
        case 'l':
        case 'L':
            fp = toLower;
            break;
        default:
            puts("잘못된 입력\n");
            exit(EXIT_FAILURE);
    }

    //fp(str);
    //puts(str);
    //TODO: 위 두 줄을 주석 처리하고,  show 함수를 사용해서 출력해본다.
    show(fp, str);

    return EXIT_SUCCESS;
}
=================================================================================
#include <stdio.h>

int fa()
    { return 1; }
int fb()
    { return 2; }
int fc()
    { return 3; }

#if 0
typedef int (*pFunc) ();

int func(pFunc pf[3]) {
    int i;
    int sum = 0;

    for (i=0; i < 3; i++) {
        sum += pf[i]();
    }

    return sum;
}
#else
//int func(int (**func1)()) {
int func(int (*func1[3])()) {
    int i;
    int sum = 0;

    for (i=0; i < 3; i++) {
        sum += func1[i]();
    }

    return sum;
}
#endif

int main(void) {
    int ( *fp[3] )() = {fa, fb, fc};
    printf( "%d", func(fp) );

    return 0;
};

* output
6
=================================================================================
#include <stdio.h>
#include <string.h>

int test(int num){
    printf("input num is %d.\n", num);
}

int main()
{
    int (*testptr)(int);
    testptr = test;
    testptr(100);

    return 0;
}

* output
input num is 100.

=================================================================================
#include <stdio.h>
#include <ctype.h>

int plus(int, int);
int minus(int, int);
int mul(int, int);
int div(int, int);

//typedef int (*handler) (int, int);

int main(void) {
    char op;
    int num1, num2;
    //handler fp;
    int (*handler) (int, int);

    while (1) {
        printf("Enter Operator : ");
        scanf("%c", &op);

        switch(op) {
#if 0
            case '+': { fp = plus; break; }
            case '-': { fp = minus; break; }
            case '*': { fp = mul; break; }
            case '/': { fp = div; break; }
#endif
            case '+': { handler = plus; break; }
            case '-': { handler = minus; break; }
            case '*': { handler = mul; break; }
            case '/': { handler = div; break; }
            default :
                printf("default\n");
        }

        printf("input two digit : ");
        scanf("%d %d", &num1, &num2);

        printf("output = %d.\n", (*handler) (num1, num2));

        while(getchar() != '\n') {};
    }

    return 0;
}

int plus(int a, int b) {
    return (a+b);
}

int minus(int a, int b) {
    return (a-b);
}

int mul(int a, int b) {
    return (a*b);
}

int div(int a, int b) {
    return (a/b);
}

* output
$ ./function_pointer4.exe
Enter Operator : +
input two digit : 20 30
output = 50.

=================================================================================
#include <stdio.h>
#include <stdlib.h>

int plus(int, int);
int minus(int, int);
int mul(int, int);
int div1(int, int);

typedef int (*handler) (int, int);

int main(void) {
    char op;
    int num1, num2;
    handler fp;

    while (1) {
        printf("Enter Operator : ");
        scanf("%c", &op);

        switch(op) {
            case '+': { fp = plus; break; }
            case '-': { fp = minus; break; }
            case '*': { fp = mul; break; }
            case '/': { fp = div1; break; }
            case 'q': exit(0);
        }

        printf("input two digit : ");
        scanf("%d %d", &num1, &num2);

        printf("output = %d.\n", fp(num1, num2));

        while(getchar() != '\n') {};
    }

    return 0;
}

int plus(int a, int b) {
    return (a+b);
}

int minus(int a, int b) {
    return (a-b);
}

int mul(int a, int b) {
    return (a*b);
}

int div1(int a, int b) {
    return (a/b);
}
=================================================================================
#include <stdio.h>

typedef int (*pF)();

int add(int a, int b) {
    return a+b;
}

int sub(int a, int b) {
    return a-b;
}

int mul(int a, int b) {
    return a*b;
}

void func(int (*p)(int, int)) {
    printf("%d\n", p(3,4));
}

int (*p)();
int (*pa[3])() = {add, sub, mul};

/* 배열에서 값을 꺼내오는 함수 */
int (*compute(int op))() {
    return pa[op];
}

// typedef를 통해 함수선언 단순화
// 리턴값이 함수
pF compute1(int op) {
    return pa[op];
}

void main() {
    p = add;
    printf("p(3,4) : %d\n", p(3,4));
    printf("pa[0](3,4) : %d\n", pa[0](3,4));    // (*pa[0])(3,4)
    printf("(*pa)(3,4) : %d\n", (*pa)(3,4));    //
    printf("compute(0)(3,4) : %d\n", compute(0)(3,4));
    printf("compute1(0)(3,4) : %d\n", compute1(0)(3,4));
}

// 참고로 함수포인터의 경우 *p, **p, *******p는 모두 동일.

*output
$ ./function_pointer6.exe
p(3,4) : 7
pa[0](3,4) : 7
(*pa)(3,4) : 7
compute(0)(3,4) : 7
compute1(0)(3,4) : 7

=================================================================================
#include <stdio.h>

typedef int (*pf_Return_Int)();
typedef void (*pf)();       // function
typedef int (*ary_2)[3];    // 2-Array
typedef int ary_1[3];       // 1-Array
typedef int *iP;            // Integer Pointer

int returnInt() {
    return 1;
};

void aaa() {
    printf("AAA\n");
};

void bbb() {
    printf("BBB\n");
};

void ccc() {
    printf("CCC\n");
};

// 3개의 함수포인터 배열 선언. 리턴값 함수포인터
pf funcTable[3] = {aaa, bbb, ccc};

// 함수를 인자로 받아 함수를 리턴하는 함수
pf func(pf_Return_Int arg) {
    return funcTable[arg()];
}

void main() {
    int temp = 5;
    iP val1 = &temp;
    int temp2[3][3] = {1,2,3,4,5,6,7,8,9};
    // 2차원 배열 연결
    ary_2 val2 = temp2;
    // 3칸 temp3 배열 내부에 ary_1(한칸짜리 배열)을 넣어줌
    ary_1 temp3[3];
    // temp4에 temp2[0]에 존재하는 하나의 배열(행)의 첫번째 값을 의미
    iP temp4 = temp2[0];
    printf("%d\n", *(temp4+2)); //print_temp2[0][2]
    printf("%d\n", *(temp4+4)); //print_temp2[1][1]
    printf("%d\n", *(temp4+6)); //print_temp2[2][0]
    // func라는 함수에 returnInt라는 함수를 인자로 넣고 함수테이블에 존재하는 값을 리턴
    func(returnInt)();
}

*output
$ ./function_pointer7.exe
3
5
7
BBB
=================================================================================

=================================================================================

=================================================================================

=================================================================================

=================================================================================
