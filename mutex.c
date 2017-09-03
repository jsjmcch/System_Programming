뮤텍스는 pthread에서 제공하는 동기화 매커니즘으로 공유 자원 공간에 대한 접근 시간 제어로 동기화를 달성한다. 
기본적인 매커니즘은 세마포어(:12)와 비슷하다. 특히 POSIX(:12) 세마포어와 비슷하며, 동기화 매커니즘으로 뮤텍스 대신 세마포어를 사용할 수도 있다. 
동기화 매커니즘의 핵심은 상호 배제로 다음과 같이 달성 한다.
global int v = 1;
lock()
{
    while(1)
    {
        if (v==1)
            break;
    }
    v = 0;
    return 1;
}

unlock()
{
    v = 1;
    break;
}

뮤텍스를 생성하기 위해서 우리는 먼저, 뮤텍스정보를 저장하기 위한 타입인 pthread_mutex_t 를 선언해주고 이것을 초기화 해주어야 한다. 
선언과 초기화의 가장간단한 방법은 PTHREAD_MUTEX_INITIALIZER 상수를 할당하는 것으로 아래와 같이 사용할수 있다.
pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER;

뮤텍스 잠금을 위한 함수로는 pthread_mutex_lock() 함수를 제공한다. 
이 함수는 해당 뮤텍스에 대해서 잠금을 시도하는데, 만약 잠그려는 뮤텍스가 다른 쓰레드에 의해서 이미 잠겨있다면, 
잠금을 얻을수 있을때까지 - 이미 잠근 다른 쓰레드가 뮤텍스의 잠금을 해제할때까지 - 봉쇄(블럭)되게 된다. 

다음은 이러한 뮤텍스 잠금을 얻기 위한 지원함수들이다.
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_destory(pthread_mutex_t *mutex);

pthread_mutex_trylock() 를 사용하면 잠금을 얻을수 없을경우 해당 코드에서 블럭되지 않고 바로 에러코드를 돌려준다. 
즉 pthread_mutex_lock 의 비봉쇄 버젼이라고 생각하면 된다.
뮤텍스 잠금을 얻은후 해당 영역에서의 작업을 마친후 잠금을 해제하기 위해서 사용한다. 
사용되는 함수는 pthread_mutex_unlock(3) 이며 함수원형은 다음과 같다.

int pthread_mutex_unlock(pthread_mutex_t *mutex);

다음은 쓰레드간 공유되는 자원을 위해서 잠금을 어떻게 사용하는지를 보여주는 간단한 예제다.

#include <stdio.h>  
#include <unistd.h>  
#include <pthread.h>  
 
int ncount;    // 쓰레드간 공유되는 자원 
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER; // 쓰레드 초기화 
 
void* do_loop(void *data) 
{ 
    int i; 
    for (i = 0; i < 10; i++) 
    { 
        pthread_mutex_lock(&mutex); // 잠금을 생성한다. 
        printf("loop1 : %d\n", ncount); 
        ncount ++; 
        if(i == 10) return;
        pthread_mutex_unlock(&mutex); // 잠금을 해제한다. 
        sleep(1); 
    } 
} 
 
void* do_loop2(void *data) 
{ 
    int i; 
 
    // 잠금을 얻으려고 하지만 do_loop 에서 이미 잠금을  
    // 얻었음으로 잠금이 해제될때까지 기다린다.   
    for (i = 0; i < 10; i++) 
    { 
        pthread_mutex_lock(&mutex); // 잠금을 생성한다. 
        printf("loop2 : %d\n", ncount); 
        ncount ++; 
        pthread_mutex_unlock(&mutex); // 잠금을 해제한다. 
        sleep(2); 
    } 
}     
 
int main() 
{ 
    int       thr_id; 
    pthread_t p_thread[2]; 
    int status; 
    int a = 1; 
 
    ncount = 0; 
    thr_id = pthread_create(&p_thread[0], NULL, do_loop, (void *)&a); 
    sleep(1); 
    thr_id = pthread_create(&p_thread[1], NULL, do_loop2, (void *)&a); 
 
    pthread_join(p_thread[0], (void *) &status); 
    pthread_join(p_thread[1], (void *) &status); 
 
    status = pthread_mutex_destroy(&mutex); 
    printf("code  =  %d", status); 
    printf("programing is end"); 
    return 0; 
} 
위의 코드를 우선 mutex 잠금을 하지 않은체 컴파일후 실행해보자. 간단하게 pthread_mutext_lock 와 pthread_mutex_unlock 부만 주석처리하면 된다. 
그러면 do_loop2 와 do_loop 가 일정한 간격을 두고 ncount 자원에 접근하는 것을 볼수 있을것이다. 
그러나 우리는 do_loop 가 ncount 자원을 접근하고 있는동안 다른 쓰레드가 접근하지 않기를 원할때가 있을것이다. 이럴때 뮤텍스 잠금을 사용하면 된다.
위의 코드에서 뮤텍스 잠금 부분의 주석을 풀고 다시 컴파일해서 실행시켜보면, 
do_loop 쓰레드가 ncount 증가 작업을 모두 마칠때까지 do_loop2 쓰레드는 해당 영역에서 블럭됨을 알수 있을것이다. 
이런식으로 하나의 쓰레드가 특정자원에 접근할때 다른 쓰레드가 접근하지 못하도록(한번에 하나의 쓰레드만 해당 자원에 접근할수 있도록) 제어할수 있다.
컴파일 방법은 gcc -o mutex_lock mutex_lock.c -lpthread 이다
더이상 뮤텍스를 사용할일이 없다면 pthread_mutex_destory 를 이용해서 뮤텍스 자원을 제거(free) 하도록 한다. 
만일 뮤텍스자원을 사용하는 쓰레드가 하나라도 존재한다면 에러코드(EBUSY)를 리턴한다. 
그러므로 모든 쓰레드의 뮤텍스에 대해서 pthread_mutex_unlock 을 이용해서 잠겨져야만 뮤텍스 제거가 성공할수 있다. 성공할경우 0을 넘겨준다.

============================================================================================
