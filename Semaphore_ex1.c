<Semaphore Function>
Reference site : https://www.joinc.co.kr/w/Site/system_programing/IPC/semaphores
===================================================================
- 헤더파일
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
 
- int semget(key_t key, int nsems, int semflg); 
만약 최초 생성이라면
    sem_num = 1;
그렇지 않고 만들어진 세마포어에 접근하는 것이라면
    sem_num = 0; 
sem_id = semget(12345, sem_num, IPC_CREAT|0660)) == -1)
{
    perror("semget error : ");
    return -1;
}
semget 은 성공할경우 int 형의 세마포어 식별자를 되돌려주며, 모든 세마포어에 대한 접근은 이 세마포어 실별자를 사용한다.
위의 코드는 key 12345 를 이용해서 세마포어를 생성하며 퍼미션은 0660으로 설정된다. 세마포어의 크기는 1로 잡혀 있다(대부분의 경우 1).
만약 기존에 key 12345 로 이미 만들어진 세마포어가 있다면 새로 생성하지 않고 기존의 세마포어에 접근할수 있는 세마포어 식별자를 되돌려주게 되고, 
커널은 semget 를 통해 넘어온 정보를 이용해서 semid_ds 구조체를 세팅한다.
   
- int semop (int semid, struct sembuf *sops, unsigned nsops); 
semop의 첫번째 semid 는 semget 을 통해서 얻은 세마포어 식별자이다. 2번째 아규먼트는 struct sembuf 로써, 
어떤 연산을 이루어지게 할런지 결정하기 위해서 사용된다. 구조체의 내용은 다음과 같으며, sys/sem.h 에 선언되어 있다.
struct sembuf
{
    short sem_num;    // 세마포어의수
    short sem_op;     // 세마포어 연산지정
    short sem_flg;    // 연산옵션(flag)
}
sem_num 멤버는 세마포어의 수로 여러개의 세마포어를 사용하지 않는다면(즉 배열이 아닐경우) 0을 사용한다. 
배열의 인덱스 사이즈라고 생각하면 될것이다. 보통의 경우 하나의 세마포어를 지정해서 사용하므로 0 이 될것이다. 
sem_op를 이용해서 실질적으로 세마포어 연산을 하게 되며, 이것을 이용해서 세마포어 값을 증가시키거나 감소 시킬수 있다. 
sem_op값이 양수일 경우는 자원을 다 썼으니, 세마포어 값을 증가시키겠다는 뜻이며, 음수일 경우에는 세마포어를 사용할것을 요청한다라는 뜻이다.
음수일 경우 세마포어값이 충분하다면 세마포어를 사용할수 있으며, 커널은 세마포어의 값을 음수의 크기의 절대값만큼을 세마포어에서 빼준다. 
만약 세마포어의 값이 충분하지 않다면 세번째 아규먼트인 sem_flg 에 따라서 행동이 결정되는데, sem_flg 가 IPC_NOWAIT로 명시되어 있다면, 
해당영역에서 기다리지 않고(none block) 바로 에러코드를 리턴한다. 그렇지 않다면 세마포어를 획득할수 있을때까지 block 되게 된다.
sem_flg 는 IPC_NOWAIT 와 SEM_UNDO 2개의 설정할수 있는 값을가지고 있다. 
IPC_NOWAIT 는 none block 모드 지정을 위해서 사용되며, SEM_UNDO 는 프로세스가 세마포어를 돌려주지 않고 종료해 버릴경우 
커널에서 알아서 세마포어 값을 조정(증가) 할수 있도록 만들어 준다.
ex.
현재 세마포어 값이 1 이라고 가정하자. 이때 A 프로세스가 semop 를 통해서 세마포어에 접근을 시도한다. 
A는 접근을 위해서 sem_op 에 -1 을 세팅한다. 즉 세마포어 자원을 1 만큼 사용하겠다라는 뜻이다.   
현재 준비된 세마포어 값은 1로 즉시 사용할수 있으므로, A는 자원을 사용하게 되며, 커널은 세마포어 값을 1 만큼 감소시킨다. 

이때 B 라는 프로세스가 세마포어 자원을 1 만큼 사용하겠다라고 요청을 한다. 
그러나 지금 세마포어 값은 0 이므로 B는 지금당장 세마포어 를 사용할수 없으며, 
기다리거나, 에러값을 리턴 받아야 한다(IPC_NOWAIT). B는 자원 사용가능할때까지 기다리기로 결정을 했다.  

잠수후 A는 모든 작업을 다마쳤다. 이제 세마포어를 되돌려줘야 한다. sem_op 에 1 을 세팅하면, 커널은 세마포어 값을 1증가시키게 된다. 

드디어 기다리던 B가 세마포어 자원을 사용할수 있는 때가 도래했다. 이제 세마포어 값은 1이 므로 B는 세마포어를 획득하게 된다.  
커널은 세마포어 값을 1 감소 시킨다.B는 원하는 작업을 한다.
   
int semctl(int semid, int semnum, int cmd, union semun arg); 
semctl 이란 함수를 이용해서 우리는 세마포어를 조정할수 있다. semctl 은 semid_ds 구조체를 변경함으로써 세마포어의 특성을 조정한다.
첫번째 아규먼트인 semid 는 세마포어 식별자다. semnum 은 세마포어 배열에서 몇 번째 세마포어를 사용할지를 선택하기 위해서 사용한다. 
세마포어의 크기가 1이라면 0이 된다. (배열은 0번 부터 시작하기 때문) 
cmd 는 세마포어 조작명령어 셋으로 다음과 같은 조작명령어들을 가지고 있다. 아래는 그중 중요하다고 생각되는 것들만을 설명하였다. 
더 자세한 내용은 semctl 에 대한 man 페이지를 참고하기 바란다.

IPC_STAT : 세마포어 상태값을 얻어오기 위해 사용되며, 상태값은 arg 에 저장된다.
IPC_RMID : 세마포어 를 삭제하기 위해서 사용한다.
IPC_SET  : semid_ds 의 ipc_perm 정보를 변경함으로써 세마포어에 대한 권한을 변경한다.

===================================================================
<example #1>
#include <sys/types.h> 
#include <sys/sem.h> 
#include <sys/ipc.h> 
#include <stdio.h> 
#include <unistd.h> 

#define SEMKEY 2345 

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
};

static int  semid;
int main(int argc, char **argv)
{
    FILE* fp;
    char buf[11];
    char count[11];

    union semun sem_union; 

    // open 과 close 를 위한 sembuf 구조체를 정의한다. 
    struct sembuf mysem_open  = {0, -1, SEM_UNDO}; // 세마포어 얻기
    struct sembuf mysem_close = {0, 1, SEM_UNDO};  // 세마포어 돌려주기
    int sem_num;

    memset(buf, 0x00, 11);
    memset(count, 0x00, 11);

    // 아규먼트가 있으면 생성자
    // 그렇지 않으면 소비자이다.
    if (argc > 1)
        sem_num = 1;
    else 
        sem_num = 0;            

    // 세마포설정을 한다. 
    semid = semget((key_t)234, sem_num, 0660|IPC_CREAT);
    if (semid == -1)
    {
        perror("semget error ");
        exit(0);
    }    

    // 세마포어 초기화
     sem_union.val = 1;
     if ( -1 == semctl( semid, 0, SETVAL, sem_union))
     {   
             printf( "semctl()-SETVAL 실행 오류\n");
             return -1; 
     }   
    // counter.txt 파일을 열기 위해서 세마포어검사를한다. 
    if(semop(semid, &mysem_open, 1) == -1)
    {
        perror("semop error ");
        exit(0);
    }

    if ((fp = fopen("counter.txt", "r+")) == NULL)
    {
        perror("fopen error ");
        exit(0);
    }
    // 파일의 내용을 읽은후 파일을 처음으로 되돌린다.  
    fgets(buf, 11, fp);
    rewind(fp);

    // 개행문자를 제거한다. 
    buf[strlen(buf) - 1] = 0x00;

    sprintf(count, "%d\n", atoi(buf) + 1); 
    printf("%s", count);
    // 10초를 잠들고 난후 count 를 파일에 쓴다. 
    sleep(10);
    fputs(count,fp);

    fclose(fp);
    // 모든 작업을 마쳤다면 세마포어 자원을 되될려준다
    semop(semid, &mysem_close, 1);
    return 1;
}
   
   
<example #2>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int    cnt   = 0;
static int  semid;

void p()
{
   struct sembuf pbuf;

   pbuf.sem_num   = 0;
   pbuf.sem_op    = -1;
   pbuf.sem_flg   = SEM_UNDO;

   if ( -1 == semop(semid, &pbuf, 1))
      printf( "p()-semop() execution error.\n");
}

void v()
{
   struct sembuf vbuf;

   vbuf.sem_num   = 0;
   vbuf.sem_op    = 1;
   vbuf.sem_flg   = SEM_UNDO;

   if ( -1 == semop( semid, &vbuf, 1))
      printf( "v()-semop() execution error.\n");
}

void *fun_thread1(void *arg)
{
   while( cnt < 5)
   {
      p();
      printf( "thread1 execute.\n");
      cnt++;
      usleep( 100);
      printf( "thread1 end.\n");
      v();
   }
   return NULL;
}

void *fun_thread2(void *arg)
{
   while( cnt < 5)
   {
      p();
      printf( "thread2 execute.\n");
      printf( " Counter = %d\n", cnt);
      usleep( 100);
      printf( "thread2 end.\n");
      v();
   }
   return NULL;
}

int main(int argc, char *argv[])
{
   pthread_t thread1;
   pthread_t thread2;
   union semun{
      int                  val;
      struct   semid_ds   *buf;
      unsigned short int  *arrary;
   }  arg;

   if ( -1 == (semid = semget( IPC_PRIVATE, 1, IPC_CREAT | 0666)))
   {
      printf( "semget() execute error.\n");
      return -1;
   }

   arg.val  =  1;                
   if ( -1 == semctl(semid, 0, SETVAL, arg))
   {
      printf( "semctl()-SETVAL execute error.\n");
      return -1;
   }

   pthread_create(&thread1, NULL, fun_thread1, NULL);
   pthread_create(&thread2, NULL, fun_thread2, NULL);
   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL);

   if ( -1 == semctl(semid, 0, IPC_RMID, arg))
   {
      printf( "semctl()-IPC_RMID execute error.\n");
      return -1;
   }
   printf( "program end.\n");
   return 0;
}
