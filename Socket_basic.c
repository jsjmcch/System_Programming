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
                                                           
