<select함수로 입출력 다중화>

- reference : https://www.joinc.co.kr/w/Site/system_programing/File/select
- Multi process vs Multi plexing
 1) Multi process
   : big data and small client
 2) Multi plexing
   : small data and many client

입출력 다중화는 여러 개의 파일에서 발생하는 입출력을 함께 관리하는 기술이다. 원리는 간단하다. 
입출력을 관리하고자 하는 파일의 그룹을 fd_set이라는 파일 비트 배열에 집어 넣고, 비트 배열의 값이 변했는지를 확인하는 방식이다.
리눅스는 BSD select함수로 입출력 다중화를 달성한다. dd

int select (int nfds, fd_set *readfds, fd_set *writefds, 
    fd_set *exceptfds, struct timeval *timeout);
다음은 select함수의 매개 변수다.
nfds : 관리하는 파일의 개수를 등록한다. 파일의 개수는 최대 파일 지정 번호 + 1로 지정하면 된다.
fd_set : 관리하는 파일의 지정번호가 등록되어 있는 비트 배열 구조체
readfds : 읽을 데이터가 있는지 검사하기 위한 파일 목록
writefds : 쓰여진 데이터가 있는지 검사하기 위한 파일 목록
exceptfds : 파일에 예외 사항들이 있는지 검사하기 위한 파일 목록
timeout : select함수는 fd_set에 등록된 파일들에 데이터 변경이 있는지를 timeout동안 기다린다. 
만약 timeout시간동안 변경이 없다면 0을 반환 한다. timeout을 NULL로 하면, 데이터가 있을 때까지 무한정 기다리고, 멤버 값이 모두 0이면 즉시 반환한다.

fd_set 구조체는 은 1024크기를 가지는 비트 배열을 포함하고 있는데, 파일 지정 번호는 각 비트 배열 첨자에 대응되는 구조를 가지고 있다. 
예를 들어 파일 지정번호가 3이라면 4번째 비트배열에 대응된다. (배열은 0부터 시작하기 때문)

만약 변경된 데이터가 있다면 해당 비트값이 1로 설정이 되고, 프로그램은 이 비트 값을 검사함으로써 어떤 파일 지정 번호에 변경된 데이터가 있는지 
확인해서 읽기/쓰기를 하면 된다.
select 함수는 데이터가 변경된 파일의 개수 즉 fd_set에서 비트 값이 1인 필드의 개수를 반환한다. 데이터가 변경된 파일의 목록을 반환하지 않는다는 
것에 주의해야 한다. 그러므로 만약 최대 파일 지정 번호가 1000 이고 select가 1을 반환 했다면, 
0번 부터 루프를 순환하면서 어떤 파일이 변경되었는지를 검사해야 한다. 최악의 경우 1000번 루프를 돌아야 한다. 
select의 작동방식에 따른 근본적인 문제인데, 연결 파일의 목록을 별도의 배열에 유지 하는 것으로 어느 정도 문제를 해결할 수 있기는 하다. 
(완전한 방법은 아니다. 연결된 파일이 하나고 이 파일의 지정 번호가 1000이라면, 1번만 비교할 수 있지만, 
연결된 파일이 1000이라면 여전히 1000번의 루프를 돌아야 할 수 있다.)
    
FD_ZERO는 fdset에 있는 모든 값을 0으로 초기화합니다. 변수를 선언하면 초기화하는 것은 당연합니다. 처음 fd_set를 선언후 반드시 호출해야 합니다.
FD_CLR는 fdset의 값에서 fd에 해당 하는 비트(0부터 시작하기에 실제는 fd-1비트)를 0으로 클리어합니다.
FD_SET는 fdset의 값에서 fd에 해당 하는 비트(0부터 시작하기에 실제는 fd-1비트)를 1으로 설정합니다.
FD_ISSET는 fdset의 값에서 fd에 해당 하는 비트(0부터 시작하기에 실제는 fd-1비트)가 1로 설정되었는지 확인합니다. 
    즉, 1로 설정되었다면 1를 반환, 0으로 설정되었다면 0으로 반환합니다.


출처: http://ospace.tistory.com/147 [JaPa2]

* example #1
이 예제 프로그램은 /tmp/testfile 과 /tmp/testfile2 두개의 파일을 읽어서 파일에 내용이 추가될 때마다 화면에 뿌려주는 일을 한다.
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 

int main()
{
    int fd[2];
    int i;
    int n;
    int state;

    char buf[255];

    struct timeval tv;

    fd_set readfds, writefds;

    if ((fd[0] = open("/tmp/testfile", O_RDONLY)) == -1)
    {
        perror("file open error : ");
        exit(0);
    }
    if ((fd[1] = open("/tmp/testfile2", O_RDONLY)) == -1)
    {
        perror("file open error : ");
        exit(0);
    }

    memset (buf, 0x00, 255);


    for(;;)
    {
        FD_ZERO(&readfds);
        FD_SET(fd[0], &readfds);
        FD_SET(fd[1], &readfds);

        state = select(fd[1]+1, &readfds, NULL, NULL, NULL);
        switch(state)
        {
            case -1:
                perror("select error : ");
                exit(0);
                break;

            default :
                for (i = 0; i < 2; i++)
                {
                    if (FD_ISSET(fd[i], &readfds))
                    {
                        while ((n = read(fd[i], buf, 255)) > 0)
                            printf("(%d) [%d] %s", state, i, buf);
                    }
                }
                memset (buf, 0x00, 255);
                break;
        }
        usleep(1000);
    }
}

위의 프로그램은 초기에 "/tmp/testfile" 와 "/tmp/testfil2" 2개의 파일을 열어서 출력을 하고 파일 끝까지 가더라도 프로그램을 종료하지 않고, 
select 를 이용해서 파일에 새로운 내용이 입력되는지 기다리는지를 조사해서 새로운 내용이 입력되면 화면에 출력하도록 한다.
우리는 fd[0]과 fd[1] 의 2개의 파일에 대해서 읽을수 있는 데이타가 있는지에 관심을 가지고 있음으로, FD_SET 을 이용 readfds 의 비트배열에 fd 값을 
할당한다. (값을 할당한다라기 보다는 비트배열의 인덱스값이 fd 를 가르킨다 라는게 좀더 적당한 표현일듯 하다) 
그다음 select 를 이용해서 readfds 의 비트값을 가져오고, FD_ISSET 을 이용해서 각 비트값을 검사하게 된다.
이코드에서는 for 루프를 돌때 usleep 를 이용해서 약간의 시간지연을 두었는데, 이는 CPU 점유율을 무한대(남는 만큼 다)로 점유하는걸 막기 위해서 이다. 
남는만큼의 CPU를 점유하고, 다른응용프로그램이 필요로 하면 돌려주기는 하지만 기분이 찜찜해서..
timeval 구조체를 이용해서 시간 제한을 두지 않는 이유는 정규파일을 select 했을경우 파일 끝을 만나더라도, readfds 비트의 설정을 제대로 하지 못하기 때문이다.

* example #2
이번에는 timeval 구조체를 이용해서, 제한시간내에 입력이 있는지 없는지 검사하는 프로그램을 만들어보도록 하자.
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 

int main()
{
    int        fd;
    char    buf[255];
    int        state; 

    struct    timeval tv; 
    fd_set    readfds, writefds;
    fd = fileno(stdin);
    FD_ZERO(&readfds);

    for (;;)
    {
        FD_SET(fd, &readfds);

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        state = select(fd + 1, &readfds, (fd_set *)0, (fd_set *)0, &tv);
        switch(state)
        {
            case -1:
                perror("select error : ");
                exit(0);
                break;    
            case 0:
                printf("Time over\n");            
                close(fd);
                exit(0);
                break;
            default:
                fgets(buf, 255, stdin);
                printf("%s", buf);
                break;
        }
    }    
}
매우 간단한 프로그램이다. 저 위에서 언급한 select.c 를 약간 수정만 했을 따름 이다. timeval 구조체의 세팅을 10초로 했다는 
정도만 눈여겨 보면 될것이다. select 를 이용해서 10초 동안 블럭이 되는데, 
그 10초 안에 fd 에 어떠한 입력이 발생하지 않는다면 select 는 시간이 0 을 넘겨주고, 여기에 대해 적절한 조치를 취해주기만 하면 된다. 
이것은 간단한 예제로 alarm(2)을 통해서 구현할수도 있을것이다.
========================================================================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUFSIZE 30

int main(int argc, char *argv[])
{
    fd_set reads, temps;
    int result;

    char message[BUFSIZE];
    int str_len;
    struct timeval timeout;

    FD_ZERO(&reads);    // 0으로 초기화
    FD_SET(0, &reads);  // 파일디스크립터 0(stdin) 설정

    while(1)
    {
        temps = reads;  // backup

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        result = select(1, &temps, 0, 0, &timeout);
        if( result == -1 ) {
            puts("select() : error");
            exit(1);
        }
        else if( result == 0 ) {
            puts( "select() : time out" );
        }
        else {
            if( FD_ISSET( 0, &temps )) {
                str_len = read(0, message, BUFSIZE);
                message[str_len] = '\0';
                //fputs(message, stdout);
                printf("message : %s\n", message);
            }
        }
    }

    return 0;
}

* output
$ ./select_exam.exe
good
message : good

select() : time out
select() : time out
select() : time out
select() : time out


========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

========================================================================================================================

