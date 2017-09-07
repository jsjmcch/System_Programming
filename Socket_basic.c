<Server Socket>
/*  1) 전화기를 준비한다. (소켓을 생성하는 함수 : socket)
    2) 전화국으로부터 전화번호를 부여 받는다. (소켓에 IP주소를 할당하는 함수 : bind)
    3) 케이블에 연결하고 전화 오기만을 기다린다. (소켓을 연결 요청이 가능한 상태로 만들어주는 함수 : listen)
    4) 전화벨이 울리면 받는다. (요청을 수락하는 함수 : accept)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024
#define PORT 4000

int main(void)
{
    int server_socket;
    int client_socket;
    int client_addr_size;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char buff_rcv[BUFF_SIZE+5];
    char buff_snd[BUFF_SIZE+5];

    server_socket = socket( PF_INET, SOCK_STREAM, 0);   // socket creation
    if( -1 == server_socket ) {
        printf( "server socket creation fail\n");
        exit(1);
    }

    memset( &server_addr, 0, sizeof( server_addr ));    // server_addr init
    server_addr.sin_family = AF_INET;   // set IPV4
    server_addr.sin_port   = htons( PORT ); // set PORT
    server_addr.sin_addr.s_addr = htonl( INADDR_ANY );  // set IP

    // 소켓에 IP주소와 포트번호를 지정
    // bind( int sockfd, struct sockaddr *myaddr, socklen_t addrlen );
    // int sockfd : socket descriptor
    // struct sockaddr *myaddr : AF_INET(인터넷이용) or AF_UNIX(시스템 내부통신) 설정
    //      - AF_INET : struct sockaddr_in 사용
    //      - AF_UNIX : struct sockaddr 사용
    // socklen_t addrlen : myaddr 구조체의크기
    // 성공 : 0, 실패 : -1
    if( -1 == bind( server_socket, (struct sockaddr*)&server_addr, sizeof( server_addr )))
    {
        printf("bind() error\n");
        exit(1);
    }

    while(1)
    {
        // listen(int s, int backlog);
        // int s : 소켓 디스크립터
        // int backlog : 대기 메시지큐의 개수
        if( -1 == listen( server_socket, 5 ))
        {
            printf( "대기모드 설정 실패\n");
            exit(1);
        }

        // accept(int s, struct sockaddr *addr, socklen_t *addrlen);
        // int s : 소켓 디스크립터
        // struct sockaddr *addr : 클라이언트 주소 정보를 가지고 있는 포인터
        // socklen_t *addrlen : struct sockaddr *addr 포인터가 가르키는 구조체의 크기
        // 성공 : 새로운 소켓 디스크립터, 실패 : -1
        client_addr_size = sizeof( client_addr );
        client_socket = accept( server_socket, (struct sockaddr*)&client_addr, &client_addr_size );
        if( -1 == client_socket ) {
            printf( "클라이언트 연결 수락 실패\n");
            exit(1);
        }

        // read( int fd, void *buf, size_t nbytes );
        // inf fd : 소켓 디스크립터
        // void *buf : 읽어들일 버퍼
        // size_t nbytes : 버퍼의 크기
        read( client_socket, buff_rcv, BUFF_SIZE );
        printf( "receive: %s\n", buff_rcv );
#if 0
        int i;
        for( i=0; i < strlen(buff_rcv); i++) {
            printf("%c - ", buff_rcv[i]);
        }
        printf("\n");
#endif

        // write( int fd, const void *buf, size_t n );
        // int fd : 소켓 디스크립터
        // const void *buf : 클라이언트에 전송할 버퍼
        // size_t n : 전송할 바이트 수
        // 성공 : 전송한 바이트 수, 실패 : -1
        printf("send msg length = %d\n", strlen( buff_rcv ));
        sprintf( buff_snd, "%d : %s", strlen( buff_rcv ), buff_rcv);
        write( client_socket, buff_snd, strlen( buff_snd )+1 ); // NULL 포함해서 전송
        close( client_socket );
    }

    return 0;
}
                                                           
<Client Socket>                                                           
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024
#define PORT 4000

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buff[BUFF_SIZE+5];
    char *ptr = NULL;

    client_socket = socket( PF_INET, SOCK_STREAM, 0 );
    if( -1 == client_socket ) {
        printf( "socket 생성 실패\n");
        exit(1);
    }

    if( argc != 2 ) {
        printf( "argc error\n" );
        exit(1);
    }

    ptr = argv[1];
  
    memset( &server_addr, 0, sizeof( server_addr ));
    server_addr.sin_family = AF_INET;   // set IPV4
    server_addr.sin_port   = htons( PORT ); // set PORT
    server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

    if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr )))
    {
        printf("접속 실패\n");
        exit(1);
    }

    printf( "send data : %s[%d]\n", ptr, strlen(ptr) );

    if ( strlen(ptr) != write( client_socket, ptr, strlen(ptr)))
    {
        printf( "write error\n" );
        exit(1);
    }

    printf( "==============write finished============\n" );
    read( client_socket, buff, BUFF_SIZE);

    printf( "recv data : %s[%d]\n", buff, strlen(buff) );
    printf( "==============read finished=============\n" );
    close( client_socket );

    return 0;
}
                                                           
<OUTPUT>
$ ./server_socket.exe
receive: test message
send msg length = 13

$ ./client_socket.exe "test message"
send data : test message[12]
==============write finished============
recv data : 13 : test message[18]
==============read finished=============
    
===================================================================================
다양한 입출력 함수들

13-1 send & recv 입출력 함수

#include <sys/socket.h>

ssize_t send(int sockfd, const void * buf, size_t nbytes, int flags);
	-> 성공 시 전송된 바이트 수, 실패 시 -1 반환

	- sockfd : 데이터 전송 대상과의 연결을 의미하는 소켓의 파일 디스크립터 전달.
	- buf : 전송할 데이터를 저장하고 있는 버퍼의 주소 값 전달.
	- nbytes : 전송할 바이트 수 전달.
	- flags : 데이터 전송 시 적용할 다양한 옵션 정보 전달. 

flags 값에는 아래와 같은 옵션이 들어갈 수 있다. 

MSG_OBB : 긴급 데이터(Out-ofband data)의 전송을 위한 옵션. write()함수보다 우선순위가 높다. 

MSG_DONTROUTE : 데이터 전송과정에서 라우팅(Routing) 테이블을 참조하지 않을 것을 요구하는 옵션,
				따라서 로컬(Local) 네트워크상에서 목적지를 찾을 때 사용되는 옵션
MSG_DONTWAIT : 입출력 함수 호출과저에서 블로킹이 되지 않을 것을 요구하기 위한 옵션. 


#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
	-> 성공 시 수신한 바이트 수(단 End Of File 전송 시 0), 실패 시 -1 반환

	- sockfd : 데이터 수신 대상과의 연결을 의미하는 소켓의 파일 디스크립터 전달.
	- buf : 수시한 데이터를 저장할 버퍼의 수고 값 전달.
	- nbytes : 수신할 수 있는 최대 바이트 수 전달.
	- flags : 데이터 수신 시 적용할 다양한 옵션 정보 전달. 

flags 값에는 아래와 같은 옵션이 들어갈 수 있다. 

MSG_OBB : 긴급 데이터(Out-ofband data)의 수신을 위한 옵션.
MSG_PEEK : 입력버퍼에 수신된 데이터의 존재유무 확인을 위한 옵션. 
MSG_DONTWAIT : 입출력 함수 호출과저에서 블로킹이 되지 않을 것을 요구하기 위한 옵션. 
MSG_WAITALL : 요청한 바이트 수에 해당하는 데이터가 전부 수신될 때까지, 호출된 함수가 반환되는 것을 막기 위한 옵션. 

긴급 데이터 수신 예제 코드
ex)
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 30
void error_handling(char *message);
void urg_handler(int signo);

int acpt_sock; 
int recv_sock;

int main(int argc, char *argv[])
{
	struct sockaddr_in recv_adr, serv_adr;
	int str_len, state;
	socklen_t serv_adr_sz;
	struct sigaction act;
	char buf[BUF_SIZE];
	if(argc != 2)
	{
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	act.sa_handler = urg_handler; // MSG_OOB의 긴급 메시지를 수신하게 되면, 운영체제는 SIGURG 시그널을 발생시켜서
								  // 프로세스가 등록한 시그널 핸들러가 호출되게 한다. 
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	acpt_sock = socket(PF_INET. SOCK_STREAM, 0);
	memeset(&recv_adr, 0, sizeof(recv_adr));
	recv_adr.sin_family = AF_INET;
	recv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	recv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(acpt_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr)) == -1)
		error_handling("bind() error");
	listen(acpt_sock, 5);

	serv_adr_sz = sizeof(serv_adr);
	recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_adr, &serv_adr_sz);

	fcntl(recv_sock, F_SETOWN, getpid());	// 밑에 설명 참조.
	state = sigaction(SIGURG, &act, 0);

	while((str_len = recv(recv_sock, buf, sizeof(buf), 0)) != 0)
	{
		if(str_len == -1)
			continue;
		buf[str_len] = 0;
		puts(buf);
	}
	close(recv_sock);
	close(acpt_sock);
	return 0;
}

void urg_handler(int signo) 
{
	int str_len;
	char buf[BUF_SIZE];
	str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB); // 핸들어 함수 내부에 긴급 메시지 수신을 위한 recv()함수 
	buf[str_len] = 0;
	pritnf("Urgent message : %s \n", buf);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

fcntl 함수는 파일 디스크립터의 컨트롤에 상용이 된다. 
fcntl(recv_sock, F_SETOWN, getpid());
위에서 사용된 의미하는
"파일 디스크립터 recv_sock이 가리키는 소켓의 소유자(F_SETWON)를 getpid함수가 반환하는 ID의 프로세스로 변경시키겠다."
다른 말로 하면
"파일 디스크립터 recv_sock이 가리키는 소켓에 의해 발생하는 SIGURG 시그널을 처리하는 프로세스를 getpid함수가 반환하는 ID의 프로세스로 변경시키겠다."

왜 이 코드가 필요하냐면, 
하나의 소켓에 대한 파일 디스크립터를 여러 프로세스가 함께 소유할 수 있기 때문이다.
따라서 SIGURG 시그널을 핸들링 할 때에는 반드시 시그널을 처리할 프로세스를 지정해줘야 한다. 

사실 MSG_OOB에서의 OOB는 Out-of-band를 의미한다. 이는 "전혀 다른 통신 경로로 전송되는 데이터"라는 뜻이다. 
하지만 TCP는 별도의 통신 경로를 제공하지 않고 있다. 다만 TCP에 존재하는 Urgent mode라는 것을 이용해서 데이터를 전송해줄 뿐이다. 

* 오프셋(Offset)이란.
"기본이 되는 위치를 바탕으로 상대적 위치를 표현하는 것."

13-2 readv & writev 입출력 함수
readv & writev 함수의 사용
"데이터를 모아서 전송하고, 모아서 수신하는 기능의 함수"

때문에 적절한 상황에서 사용을 하면 입출력 함수호출의 수를 줄일 수 있다.

#include <sys/uio.h>

ssize_t writev(int fieldes, const struct iovec * iov, int iovcnt);
	-> 성공 시 전송된 바이트 수, 실패 시 -1 반환

	- filedes : 데이터 전송의 목적지를 나타내는 소켓의 파일 디스크립터 전달, 단 소켓에만 제한된 함수가 아니기 때문에,
				read 함수처럼 파일이나 콘솔 대상의 파일 디스크립터도 전달 가능하다.
	- iov : 구조체 iovec 배열의 주소 값 전달, 구조체 iovec의 변수에는 전송할 데이터의 위치 및 크기 정보가 담긴다.
	- iovcnt : 두 번째 인자로 전달된 주소 값이 가리키는 배열의 길이정보 전달. 

iov의 형 구조체 iovec는 아래와 같이 정의되어 있다.
struct iovec
{
	void * iov_base;		// 버퍼의 주소 정보
	size_t iov_len;			// 버퍼의 크기 정보
}

#include <sys/uio.h>

ssize_t readv(int filedes, const struct iovec * iov, int iovcnt);
	-> 성공 시 수신된 바이트 수, 실패 시 -1 반환

	- filedes : 데이터를 수신할 파일(혹은 소켓)의 파일 디스크립터를 인자로 전달.
	- iov : 데이터를 저장할 위치와 크기 정보를 담고 있는 iovec 구조체 배열의 주소 값 전달. 
	- iovcnt : 두 번째 인자로 전달된 주소 값이 가리키는 배열의 길이 정보 전달. 
        
===================================================================================
while(total_count < sizeof(DBIF_REQ))
{
	again:
	if((count = read(conn_fd, ((void *) &reqData) + total_count, sizeof(DBIF_REQ) - total_count)) < 0)
	{
		if(errno == EINTR)
			goto again;

		Trace("read() : error");
		return -1;
	}
	else if(count == 0)
	{
		Trace("read() : closed");
		return 0;
	}
	total_count += count;
}


again:
if((count = recv(conn_fd, ((void *) &reqData), sizeof(DBIF_REQ), MSG_WAITALL)) < 0)
{
	if(errno == EINTR)
		goto again;

	Trace("read() : error");
	return -1;
}
else if(count == 0)
{
	Trace("read() : closed");
	return 0;
}     
===================================================================================
<tcpEchoServer.c>
/*
 * tcpEchoServer.c
 *
 *  Created on: 2017. 4. 19.
 *      Author: 72359
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAXLINE 4096
#define SERV_PORT 3000
#define LISTENQ 8

int main(int argc, char **argv) {
	int listenfd, connfd, n;
	socklen_t clilen;

	char buf[MAXLINE];
	struct sockaddr_in cliaddr, servaddr;

	// creation of the socket
	listenfd = socket (AF_INET, SOCK_STREAM, 0);	// 스트림 소켓 생성

	// preparation of the socket address
	servaddr.sin_family = AF_INET;	// IPv4 인터넷 프로토콜
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// Big Endian으로 변환 (Long)
	servaddr.sin_port = htons(SERV_PORT);	// Big Endian으로 변환 (Short)

	// 생성한 소켓을 서버 소켓으로 등록
	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	// 서버 소켓을 통해 클라이언트의 접속 요청을 확인하도록 설정
	listen(listenfd, LISTENQ);

	printf("%s\n", "Server running... waiting for connections.");

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		// 클라이언트 접속 요청 대기 및 허락, 클라이언트와 통신을 위해 새 socket을 생성
		// connfd : 클라이언트 접속 시 커널이 생성한 소켓
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
		printf("%s\n", "Received request...");

		while ((n = recv(connfd, buf, MAXLINE, 0)) > 0) {
			printf("%s", "String received from and resent to the client:");
			puts(buf);
			send(connfd, buf, n, 0);
		}

		if (n < 0) {
			perror("Read Error");
			exit(1);
		}

		close(connfd);
	}

	close(listenfd);
}

<tcpEchoClient.c>
/*
 * tcpEchoClient.c
 *
 *  Created on: 2017. 4. 19.
 *      Author: 72359
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/

/*
 * constructor 함수 : main 이전에 실행됨, GCC only
 *
 * eclipse에서 cygwin gdb 디버깅할 경우 gdb를 통한 프로그램의 콘솔 출력 버퍼가 정상동작 하지 않고 실행 종료 후 한꺼번에 출력됨.
 * 이를 피하기 위해 stdout, stderr의 buffering을 끄는 함수.
 * 만일 buffer가 꼭 필요하다면 아래 함수를 주석처리 할 것.
 */
void __attribute__((constructor)) console_setting_for_eclipse_debugging( void ){
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
}

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE], recvline[MAXLINE];

	//basic check of the arguments
	//additional checks can be inserted
	if (argc !=2) {
		perror("Usage: TCPClient <IP address of the server");
		exit(1);
	}

	//Create a socket for the client
	//If sockfd<0 there was an error in the creation of the socket
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
		perror("Problem in creating the socket");
		exit(2);
	}

	//Creation of the socket
	//memset(&servaddr, 0, sizeof(servaddr));
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr= inet_addr(argv[1]);
	servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
	servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order

	//Connection of the client to the socket
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("Problem in connecting to the server");
		exit(3);
	}

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		send(sockfd, sendline, strlen(sendline), 0);

		if (recv(sockfd, recvline, MAXLINE, 0) == 0){
			//error: server terminated prematurely
			perror("The server terminated prematurely");
			exit(4);
		}
		printf("%s", "String received from the server: ");
		fputs(recvline, stdout);
	}

	exit(0);
}

===================================================================================
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define SOCK_READ_TIME_MAX 3

long getTimeGap( struct timeval *st, struct timeval *et)
{
    struct timeval rt;
    long diffT = 0L;

    rt.tv_sec = et->tv_sec - st->tv_sec;
    rt.tv_usec = et->tv_usec - st->tv_usec;

    if( rt.tv_usec < 0 ) {
        rt.tv_sec--;
        rt.tv_usec += 1000000L;
    }

    diffT = (rt.tv_sec*1000000L) + rt.tv_usec;

    return diffT;
}

int main() {
    struct timeval startTime;
    struct timeval endTime;
    long diffTime = 0L;

    gettimeofday(&startTime, NULL);

    while(1) {
        gettimeofday(&endTime, NULL);
        diffTime = getTimeGap(&startTime, &endTime);
        if(diffTime > (SOCK_READ_TIME_MAX * 1000000L)) {
            printf(" read time out \n");
            printf(" difftime = %ld \n", diffTime);
            gettimeofday(&startTime, NULL);
        }
    }

    return 0;
}
* output
$ ./gettimeofday3.exe
 read time out
 difftime = 3000115
 read time out
 difftime = 3000115
 read time out
 difftime = 3000115    
===================================================================================
http://thdev.net/175
===================================================================================
    
===================================================================================

===================================================================================
    
===================================================================================

===================================================================================
    
===================================================================================

===================================================================================    
