<pthread fucntion>

pthread를 써야하는 이유는 단일 프로세서에서는 동시에 여러 작업을 하는 것처럼 유저에게 보여야하는 일이 있는데 
간단한 예로 테트리스 게임같이 블록이 일정시간마다 자동적으로 내려가도록 하는 부분과 키 입력을 받을 때만 블록을 
입력받은 방향으로 이동시키는 부분등... 
이걸 멀티 쓰레드로 만들지 않는다면 아마 프로그래밍이 상당히 복잡해지고 구현하는데도 상당히 어려울 것이다. 
이를 쉽게 처리하기위해서 pthread API 함수를 이용하여 쉽게 구현할 수 있다.

pthread API 함수를 쓰기 위해서 pthread.h 파일을 include 해야만 한다.
#include <pthread.h>  

그리고 필히 컴파일시 -lpthread 옵션 추가해주어야만 한다. 빠뜨리면 컴파일 오류가 일어나거나 아니면 쓰레드가 정상 작동하지 않을 것이다.

pthread_t : pthread의 자료형을 의미

int pthread_create( pthread_t *th_id, const pthread_attr_t *attr, void* 함수명, void *arg );
 - pthread 생성한다.
첫 번째 인자 : pthread 식별자로 thread가 성공적으로 생성되면 thread 식별값이 주어진다.
두 번째 인자 : pthread 속성(옵션), 기본적인 thread 속성을 사용할 경우 NULL
세 번째 인자 : pthread로 분기할 함수. 반환값이 void* 타입이고 매개변수도 void* 으로 선언된 함수만 가능하다. 
              ex) void* handler (void* arg) { ... }
네 번째 인자 : 분기할 함수로 넘겨줄 인자값. 어떤 자료형을 넘겨줄 지 모르기 때문에 void형으로 넘겨주고 상황에 맞게 분기하는 함수내에서 
              원래의 자료형으로 캐스팅해서 사용하면 된다.
리턴 값 : 성공적으로 pthread가 생성될 경우 0 반환

int pthread_join( pthread_t th_id, void** thread_return );
 - 특정 pthread가 종료될 때까지 기다리다가 특정 pthread가 종료시 자원 해제시켜준다.
첫 번째 인자 : 어떤 pthread를 기다릴 지 정하는 식별자
두 번째 인자 : pthread의 return 값, 포인트로 값을 받아오는 점을 주의할 것.

int pthread_detach( pthread_t th_id );
 - th_id 식별자를 가지는 pthread가 부모 pthread로부터 독립한다. 
   즉 이렇게 독립된 pthread는 따로 pthread_join()이 없어도 종료시 자동으로 리소스 해제된다.

void pthread_exit( void* ret_value );
 - 현재 실행중인 thread를 종료시킬 대 사용한다. 보통 pthread_exit가 호출되면 cleanup handler가 호출되며 보통 리소스 해제하는 일을 수행한다.

void pthread_cleanup_push( void* (함수명), void* arg );
 - pthread_exit()가 호출될 때 호출된 handler를 정하는 함수.  보통 자원 해제용이나 mutex lock 해제를 위한 용도로 사용된다. 

void pthread_cleanup_pop(int exec);
- 설정된 cleanup handler를 제거하기 위해서 사용되는 함수.  
  exec 값을 0일 경우 바로 cleanup handler 제거하고 그외의 값을 가질 경우 cleanup handler를 한번 실행한 후 제거한다.

pthread_t pthread_self(void);
 - 현재 동작중인 pthread의 식별자를 리턴한다
 
* compile : gcc -o thread thread.c -lpthread
============================================================================================================================= 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
 
// 쓰레드 함수
void *t_function(void *data)
{
    int id;
    int i = 0;
    id = *((int *)data);
 
    while(1)
    {
        printf("%d : %d\n", id, i);
        i++;
        sleep(1);
    }
}
 
int main()
{
    pthread_t p_thread[2];
    int thr_id;
    int status;
    int a = 1;
    int b = 2;
 
    // 쓰레드 생성 아규먼트로 1 을 넘긴다. 
    thr_id = pthread_create(&p_thread[0], NULL, t_function, (void *)&a);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }
 
    // 쓰레드 생성 아규먼트로 2 를 넘긴다.
    thr_id = pthread_create(&p_thread[1], NULL, t_function, (void *)&b);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }
 
    // 쓰레드 종료를 기다린다.
    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);
 
    return 0;
}
