#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

void hello()
{
    printf("Hello, ");
}

void numberToString(int n, char *buffer)
{
    sprintf(buffer, "%d", n);
}

int main()
{
    ①_________________________
    ②_________________________
    char buffer[20];
    
    fp1 = hello;
    fp2 = numberToString;

    fp1();
    fp2(100, buffer);
    printf("%s\n", buffer);

    return 0;
}

① void (*fp1)();
② void (*fp2)(int, char *);

해설
fp1에 hello 함수의 메모리 주소를 넣은 뒤 fp1을 호출하여 "Hello, "가 출력되고 있습니다. 
따라서 함수 fp1는 함수 포인터이며 매개변수와 반환값이 없으므로 void (*fp1)();과 같이 만들어줍니다. 
그리고 fp2에 numberToString 함수의 메모리 주소를 넣은 뒤 fp2를 호출하여 숫자 50을 문자열로 변환하고 있습니다. 
여기서 numberToString 함수는 매개변수가 int, char 포인터이고 반환값이 없으므로 void (*fp2)(int, char *);와 같이 만들면 됩니다.

=============================================================================================================
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int add(int *a, int *b)
{
    return *a + *b;
}

int sub(int *a, int *b)
{
    return *a - *b;
} 

int mul(int *a, int *b)
{
    return *a * *b;
} 

int div(int *a, int *b)
{
    return *a / *b;
}

int main()
{
    char funcName[10];
    int num1, num2;

    scanf("%s %d %d", funcName, &num1, &num2);

    ___________________________
    ___________________________
    ___________________________
    ___________________________
    ___________________________
    ___________________________
    ___________________________
    ___________________________
    ___________________________
  
    printf("%d\n", fp(&num1, &num2));

    return 0;
}

* answer
    int (*fp)(int *, int *);

    if(strncmp(funcName, "add", 3) == 0) {
        fp = add;
    } else if (strncmp(funcName, "sub", 3) == 0) {
        fp = sub;
    } else if (strncmp(funcName, "mul", 3) == 0) {
        fp = mul;
    } else if (strncmp(funcName, "div", 3) == 0) {
        fp = div;
    }

=============================================================================================================
#define _CRT_SECURE_NO_WARNINGS    // scanf 보안 경고로 인한 컴파일 에러 방지
#include <stdio.h>
#include <string.h>

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

int mul(int a, int b)
{
    return a * b;
}

int div(int a, int b)
{
    return a / b;
}

int main()
{
    int funcNumber;    // 함수 번호
    int num1, num2;
    int (*fp[4])(int, int);    // int형 반환값, int형 매개변수 두 개가 있는 함수 포인터 배열 선언

    fp[0] = add;    // 첫 번째 요소에 덧셈 함수의 메모리 주소 저장
    fp[1] = sub;    // 두 번째 요소에 뺄셈 함수의 메모리 주소 저장
    fp[2] = mul;    // 세 번째 요소에 곱셈 함수의 메모리 주소 저장
    fp[3] = div;    // 네 번째 요소에 나눗셈 함수의 메모리 주소 저장

    printf("함수 번호와 계산할 값을 입력하세요: ");
    scanf("%d %d %d", &funcNumber, &num1, &num2);    // 함수 번호와 계산할 값을 입력받음

    printf("%d\n", fp[funcNumber](num1, num2));    // 함수 포인터 배열을 인덱스로 접근하여 함수 호출

    return 0;
}

함수 번호와 계산할 값을 입력하세요: 0 10 20 (입력)
30


// int형 반환값, int형 매개변수 두 개가 있는 함수 포인터 배열 선언
//↓ 반환값 자료형
int (*fp[4])(int, int);    // ← 매개변수 자료형
//    ↑   ↖ 크기가 4인 배열
// 함수 포인터 이름
=============================================================================================================
#include <stdio.h>

int add(int a, int b)    // int형 반환값, int형 매개변수 두 개
{
    return a + b;
}
// 반환값자료형 함수이름(함수포인터반환값자료형 (*함수포인터이름)(함수포인터매개변수자료형1, 함수포인터매개변수자료형2)) {}
void executer(int (*fp)(int, int))    // 함수 포인터를 매개변수로 지정
{
    printf("%d\n", fp(10, 20));    // 30: 매개변수로 함수 호출
}

int main()
{
    executer(add);    // executer를 호출할 때 add 함수의 메모리 주소를 전달

    return 0;
}

* answer  30
=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================

=============================================================================================================
