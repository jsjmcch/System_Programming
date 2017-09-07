<queue_circle.c>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define QUEUE_MAX_SIZE	100
#define MSG_SIZE 1024

typedef struct listNode { 
	char *Data[QUEUE_MAX_SIZE];
	int size;
	int rear;
	int front;
} qNode; 

qNode *Node = NULL;

void queue_init(){
	int cnt;
	Node = (qNode*)malloc(sizeof(qNode));
	if( Node == NULL ) { printf("malloc error1\n"); return; }
	for(cnt=0; cnt<QUEUE_MAX_SIZE; cnt++) {
		Node->Data[cnt] = (char *) malloc(sizeof(char) * MSG_SIZE); 
		if( Node->Data[cnt] == NULL ) { printf("malloc error2\n"); exit(2); }
		memset( Node->Data[cnt], 0x0, MSG_SIZE );
	}
	Node->rear = 0;
	Node->front = 0;
	Node->size = 0;
}

void insert_queue(char *iData) {
	// queue is full
	printf("+++++++++++++++++++++enqueue_sta++++++++++++++++++++\n");
	if (Node->size == QUEUE_MAX_SIZE) {
		printf("queue is full..\n");
		return;
	}	

	printf("Node->rear = %d\n", Node->rear);
	
	memset( Node->Data[Node->rear], 0x0, MSG_SIZE );	//  珥덇린??
	memcpy( Node->Data[Node->rear], iData, strlen(iData) ); 
	Node->rear = (Node->rear + 1) % QUEUE_MAX_SIZE; 
	Node->size++;	
	printf("Node->size = %d\n", Node->size);
}

int delete_queue(char *oData) {
	printf("+++++++++++++++++++++dequeue_sta++++++++++++++++++++\n");
	if (Node->rear == Node->front && Node->size == 0) {
		printf("queue is empty..\n");
		return -1;
	}
	
	printf("Node->front = %d\n", Node->front);
	
	memcpy( oData, Node->Data[Node->front], strlen(Node->Data[Node->front]) );
	Node->front = (Node->front + 1) % QUEUE_MAX_SIZE;
	
	Node->size--;
	printf("Node->size = %d\n", Node->size);
	printf("+++++++++++++++++++++dequeue_end++++++++++++++++++++\n");
	
	return 0;
}

void print_queue() {
	int i = 0;

	if (Node->front >= Node->rear && Node->size != 0) { 
		for (i=Node->front; i < QUEUE_MAX_SIZE; i++) {
			printf("%s \n", Node->Data[i]);
		}

		for (i=0; i < Node->rear; i++) {
			printf("%s \n", Node->Data[i]);
		}		
	}
	else if (Node->size != 0) {
		for (i=Node->front; i < Node->rear; i++ ) {
			printf("%s \n", Node->Data[i]);
		}
	}
}

void deleteAllNode() {
	int cnt;
	for(cnt=0; cnt<QUEUE_MAX_SIZE; cnt++) {
		if ( Node->Data[cnt] != NULL )
			free(Node->Data[cnt]);
	}
	Node->front = 0;
	Node->rear = 0;
	Node->size = 0;
	free(Node);
}
===============================================================================
<client_socket.c>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFF_SIZE 1024
#define PORT 4000

int sendCount; 	// 스레드 간 공유되는 자원

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	// 스레드 초기화

void *startThreadFunction1(void *a) {
	int client_socket;
	struct sockaddr_in server_addr;
	char buff[BUFF_SIZE+5];
	char buf[50] = "OPP#its time to say goodbye";

	printf("=========startThreadFunction1=============\n");

#if 0
	client_socket = socket( PF_INET, SOCK_STREAM, 0 );
	if( -1 == client_socket ) {
		printf( "socket1 create error\n");
		exit(1);
	}

	memset( &server_addr, 0, sizeof( server_addr ));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( PORT );
	server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
#endif

	while(1) {
		//pthread_mutex_lock( &mutex );
		printf("loop1 : %d\n", sendCount);
		sendCount++;
		//pthread_mutex_unlock( &mutex );
		//printf("#1 thread\n");
		//sleep(1);
		client_socket = socket( PF_INET, SOCK_STREAM, 0 );
		if( -1 == client_socket ) {
			printf( "socket1 create error\n");
			exit(1);
		}

		memset( &server_addr, 0, sizeof( server_addr ));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons( PORT );
		server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

		if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr )))
		{
			printf( "connect1 failed\n");
			exit(2);
		}

		if( strlen(buf) != write( client_socket, buf, strlen(buf) )) {
			printf( "write1 error\n");
			exit(3);
		}
	
	
		printf( "==============write1 finished============\n" ); 
		read( client_socket, buff, BUFF_SIZE );

		printf( "recv1 data : %s[%d]\n", buff, strlen(buff) );
		printf( "==============read1 finished=============\n" ); 
		close( client_socket );		

		sleep(1);
	}
}

void *startThreadFunction2(void *b) {
	int client_socket;
	struct sockaddr_in server_addr;
	char buff[BUFF_SIZE+5];
	char buf[50] = "ERR#its time to say goodbye,too";

	printf("=========startThreadFunction2=============\n");

#if 0
	client_socket = socket( PF_INET, SOCK_STREAM, 0 );
	if( -1 == client_socket ) {
		printf( "socket2 create error\n");
		exit(1);
	}

	memset( &server_addr, 0, sizeof( server_addr ));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( PORT );
	server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
#endif

	while(1) {
		//pthread_mutex_lock( &mutex );
		printf("loop2 : %d\n", sendCount);
		sendCount++;
		//pthread_mutex_unlock( &mutex );
		//printf("#2 thread\n");
		//sleep(1);
		client_socket = socket( PF_INET, SOCK_STREAM, 0 );
		if( -1 == client_socket ) {
			printf( "socket2 create error\n");
			exit(1);
		}

		memset( &server_addr, 0, sizeof( server_addr ));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons( PORT );
		server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	
		if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr )))
		{
			printf( "connect2 failed\n");
			exit(2);
		}

		if( strlen(buf) != write( client_socket, buf, strlen(buf) )) {
			printf( "write2 error\n");
			exit(3);
		}
	
	
		printf( "==============write2 finished============\n" ); 
		read( client_socket, buff, BUFF_SIZE );

		printf( "recv2 data : %s[%d]\n", buff, strlen(buff) );
		printf( "==============read2 finished=============\n" ); 
		close( client_socket );		

		sleep(1);
	}
}

void *startThreadFunction3(void *c) {
	int client_socket;
	struct sockaddr_in server_addr;
	char buff[BUFF_SIZE+5];
	char buf[100] = "SEC#its time to say goodbye,too. it's very interesting.";

	printf("=========startThreadFunction3=============\n");

#if 0
	client_socket = socket( PF_INET, SOCK_STREAM, 0 );
	if( -1 == client_socket ) {
		printf( "socket2 create error\n");
		exit(1);
	}

	memset( &server_addr, 0, sizeof( server_addr ));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( PORT );
	server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
#endif

	while(1) {
//		pthread_mutex_lock( &mutex );
		printf("loop3 : %d\n", sendCount);
		sendCount++;
//		pthread_mutex_unlock( &mutex );

		client_socket = socket( PF_INET, SOCK_STREAM, 0 );
		if( -1 == client_socket ) {
			printf( "socket3 create error\n");
			exit(1);
		}

		memset( &server_addr, 0, sizeof( server_addr ));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons( PORT );
		server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	
		if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr )))
		{
			printf( "connect3 failed\n");
			exit(2);
		}

		if( strlen(buf) != write( client_socket, buf, strlen(buf) )) {
			printf( "write3 error\n");
			exit(3);
		}
	
	
		printf( "==============write3 finished============\n" ); 
		read( client_socket, buff, BUFF_SIZE );

		printf( "recv3 data : %s[%d]\n", buff, strlen(buff) );
		printf( "==============read3 finished=============\n" ); 
		close( client_socket );		

		sleep(1);
	}
}


int main(int argc, char *argv[])
{
	int client_socket;
	struct sockaddr_in server_addr;
	char buff[BUFF_SIZE+5];
	char *ptr = NULL;
	char buf[5] = {0,};
	pthread_t p_thread[3];
	int thr_id = 0;
	int status = 0;
	int a, b, c;
#if 0
	client_socket = socket( PF_INET, SOCK_STREAM, 0 );
	if( -1 == client_socket ) {
		printf( "socket 생성 실패\n");
		exit(1);
	}
	
	if( argc != 2 ) {
		printf( "argc error\n" );
		exit(1);
	}
#endif

	// thread 1 start
	thr_id = pthread_create( &p_thread[0], NULL, startThreadFunction1, (void *)&a);
	if( thr_id < 0 ) {
		perror("thread 1 error : ");
		exit(1);
	}

	// thread 2 start
	thr_id = pthread_create( &p_thread[1], NULL, startThreadFunction2, (void *)&b);
	if( thr_id < 0 ) {
		perror("thread 2 error : ");
		exit(2);
	}

	// thread 3 start
	thr_id = pthread_create( &p_thread[2], NULL, startThreadFunction3, (void *)&c);
	if( thr_id < 0 ) {
		perror("thread 3 error : ");
		exit(3);
	}

#if 0
	ptr = argv[1];

	memset( &server_addr, 0, sizeof( server_addr ));
	server_addr.sin_family = AF_INET;	// set IPV4
	server_addr.sin_port   = htons( PORT );	// set PORT 
	server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

	if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr )))
	{
		printf("접속 실패\n");
		exit(1);
	}

#if 0	
	// send length
	sprintf(buf, "%d", strlen(ptr));
	printf( "send length data = %s[%d]\n", buf, strlen(buf));
		
	write( client_socket, buf, strlen(buf)); 

	//sleep(1);
	usleep(100);
#endif

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
#endif

	pthread_join(p_thread[0], (void **)&status); 
	pthread_join(p_thread[1], (void **)&status);
	pthread_join(p_thread[2], (void **)&status);

	//status = pthread_mutex_destroy( &mutex );

	return 0;
}
===============================================================================
<socket_select.c>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define MAX_LINE 5

void error_handling(char* message);
void queue_init();
int delete_queue(char *);
void insert_queue(char *);
void saveFile(char *buf);

static int index1;
static int index2;
static int index3; 

char opp_name[30]; 
char err_name[30];
char sec_name[30];

FILE *fp1, *fp2, *fp3;
int wc1, wc2, wc3;

const char oppN[8] = "OPPLOG_";
const char errN[8] = "ERRLOG_";
const char secN[8] = "SECLOG_";
const char txtN[5] = ".TXT";

/////////////////////////////////////////////////////
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *startSaveFile(void *a) {
	char *ptr = NULL;
	int ret = 0;
	printf("--------------startSaveFile_end--------------\n");
	ptr = (char *) malloc(sizeof(char) * BUF_SIZE);
	if( ptr == NULL ) {printf("malloc error3."); exit(4);}

	while(1) {	
		ret = delete_queue(ptr);
		if (ret == -1) { 
			sleep(1);
			continue;
		}
		saveFile(ptr);
		memset(ptr, 0x0, BUF_SIZE);
		sleep(1);
	}

	if(ptr != NULL) free(ptr);
	printf("--------------startSaveFile_end--------------\n");
}

void makeFileName(char *p, int num, int digit) {
	int i,j;
	char buf[30] = {0,};	
	int offset = 0;
	char tmpB[10] = {0,};
	memset(p, 0x0, sizeof(p));
	
	if( digit == 1 ) { // OPP
		strcpy(buf, oppN);
		offset += strlen(oppN); 
		sprintf(tmpB, "%d", num);
		strcat(&buf[offset], tmpB);
		offset += strlen(tmpB);
		strcat(&buf[offset], txtN); 
	} else if( digit == 2 ) { // ERR
		strcpy(buf, errN);
		offset += strlen(errN); 
		sprintf(tmpB, "%d", num);
		strcat(&buf[offset], tmpB);
		offset += strlen(tmpB);
		strcat(&buf[offset], txtN); 
	} else if( digit == 3 ) { // SEC
		strcpy(buf, secN);
		offset += strlen(secN); 
		sprintf(tmpB, "%d", num);
		strcat(&buf[offset], tmpB);
		offset += strlen(tmpB);
		strcat(&buf[offset], txtN); 
	}
	printf("buf = %s\n", buf);
	strcpy(p, buf);
}

void saveFile(char *buf) {
	char *ptr;

	printf("=============saveFile start==============\n");

	if( strncmp(buf, "OPP", 3 ) == 0) {
		if( fp1 == NULL) {
			makeFileName(opp_name, index1, 1); 
			fp1 = fopen(opp_name, "w");
			if( fp1 == NULL ) { perror("file open error : "); exit(1); }
		} 

		ptr = strchr(buf, '#');
		if( ptr != NULL ) {
			fputs(ptr+1, fp1);
		}
		wc1 = wc1 + 1;
		if( wc1 >= MAX_LINE ) {
			printf("++++++++++++++++++++++++++++++++++++++opplog max\n");
			fclose(fp1);
			fp1 = NULL;
			wc1 = 0;
			index1 = index1 + 1;
		} 
	} else if( strncmp(buf, "ERR", 3 ) == 0) {
		
		if( fp2 == NULL) {
			makeFileName(err_name, index2, 2); 
			fp2 = fopen(err_name, "w");
			if( fp2 == NULL ) { perror("file open error : "); exit(1); }
		} 

		ptr = strchr(buf, '#');
		if( ptr != NULL ) {
			fputs(ptr+1, fp2);
		}
		wc2 = wc2 + 1;
		if( wc2 >= MAX_LINE ) {
			printf("++++++++++++++++++++++++++++++++++++++errlog max\n");
			fclose(fp2);
			fp2 = NULL;
			wc2 = 0;
			index2 = index2 + 1;
		} 
	
	} else if( strncmp(buf, "SEC", 3 ) == 0) {
		
		if( fp3 == NULL) {
			makeFileName(sec_name, index3, 3); 
			fp3 = fopen(sec_name, "w");
			if( fp3 == NULL ) { perror("file open error : "); exit(1); }
		} 

		ptr = strchr(buf, '#');
		if( ptr != NULL ) {
			fputs(ptr+1, fp3);
		}
		wc3 = wc3 + 1;
		if( wc3 >= MAX_LINE ) {
			printf("++++++++++++++++++++++++++++++++++++++seclog max\n");
			fclose(fp3);
			fp3 = NULL;
			wc3 = 0;
			index3 = index3 + 1;
		} 
	
	} else {
		printf("Data is not recognized\n");
	}

	printf("===============saveFile end===============\n");

}

void do_encryption(char *msg) {



}

void do_decryption(char *msg) {



}


int main(int argc, char*argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval  timeout; 
	pthread_t p_thread;
	int thr_id = 0;
/*
struct timeval {
  long tv_sec;    // 초단위 정보
  long tv_usec;  // 마이크로 초 단위 정보
}
다음과 같이 timeval 구조체가 정의 되어 있고
select()함수의 마지막 매개변수로 쓰이고 있네요
select 함수는 관찰중인 파일 디스크립터에 변화가 생겨야 반환을 합니다.
때문에 변화가 없으면 무한정!!~~~~~~~~~~대기상태(블로킹)가 됩니다.
이러한 성질을 막기위해서 타임아웃을 정합니다.
이 변수 주소값을 select()함수의 마지막 인자로 전달해서
파일디스크립터값에 변화가 없으면 select함수는 0을 반환합니다.
타임아웃을 설정하기 싫다면!! NULL로 지정!!
*/


    fd_set reads, cpy_reads;              // 파일디스크립터 보관용(원본, 사본)
    socklen_t adr_sz;                           
    int fd_max;                                 // select함수 리턴값 보관
    int str_len;
    int fd_num;
    int i;
	int status;
    char buf[BUF_SIZE];
	char tmpBuf[1024] = {0,};
	int a;

    if(argc!=2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }
	
	// queue 초기화
	queue_init();
	

	thr_id = pthread_create( &p_thread, NULL, startSaveFile, (void*)a);
	if( thr_id < 0 ) {
		perror("saveFile Thread error : ");
		exit(1);
	}

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error!");


    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))== -1)
        error_handling("bind() error!");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    FD_ZERO(&reads);                   // fd_set 변수인 reads를 모두 0으로 초기화
    FD_SET(serv_sock, &reads);      // serv_sock 디스크립터 저장하여 reads로... 
    fd_max = serv_sock;                 // max 값
    
    while(1)
    {
        cpy_reads = reads;              // reads 값을 cpy_reads 에 복사
        timeout.tv_sec = 5;               
        timeout.tv_usec = 5000;

        if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;                          // 반환값이 -1이면 오류...

        if(fd_num == 0) {                    // 반환값이 0이면 타임아웃에 의해서...
			printf("time out\n");
            continue;
		}

        for(i=0; i<fd_max+1; i++)        // serv_sock +1 값으로 변화된 fd_set 검색
        {
            if(FD_ISSET(i, &cpy_reads))// 검색결과가 변화가 있다면 양수반환
           {
                if(i== serv_sock)           // 서버소켓이라면..
                {
                    adr_sz = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);
                    if(clnt_sock == -1)
                        error_handling("accept() error!");

                    FD_SET(clnt_sock, &reads); // clnt_sock을 다시 reads로 설정
                    if(fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client : %d \n", clnt_sock);
                }  // 연결됨을 확인

                else
                {
					memset(buf, 0x0, BUF_SIZE);
                    str_len = read(i, buf, BUF_SIZE);
					printf("str_len = %d\n", str_len);
                    if(str_len == 0)            // EOF일때는 종료한다
                    {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d\n", i);

                    }
                    else
                    {
						//pthread_mutex_lock( &mutex );
                        write(i, buf, str_len);  // 에코실행
						buf[str_len] = '\n';
						insert_queue( buf );	// insert data in queue
						printf("=============insert finished=============\n");
						//delete_queue( tmpBuf );	// retrieve data in queue
						//saveFile(tmpBuf);			// save
						//pthread_mutex_unlock( &mutex );
                    }
                }
            }
        }
    }    
	
	pthread_join(p_thread, (void **)&status);
	//status = pthread_mutex_destroy( &mutex );	

    close(serv_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
===============================================================================
<base64_exam.c>
#include <stdlib.h>
#include <string.h>

// BASE64
static char __base64_table[] ={
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
   'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
   'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static char __base64_pad = '=';

unsigned char *__base64_encode(const unsigned char *str, int length, int *ret_length) {
   const unsigned char *current = str;
   int i = 0;
   unsigned char *result = (unsigned char *)malloc(((length + 3 - length % 3) * 4 / 3 + 1) * sizeof(char));

   while (length > 2) { /* keep going until we have less than 24 bits */
      result[i++] = __base64_table[current[0] >> 2];
      result[i++] = __base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
      result[i++] = __base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
      result[i++] = __base64_table[current[2] & 0x3f];

      current += 3;
      length -= 3; /* we just handle 3 octets of data */
   }

   /* now deal with the tail end of things */
   if (length != 0) {
      result[i++] = __base64_table[current[0] >> 2];
      if (length > 1) {
         result[i++] = __base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
         result[i++] = __base64_table[(current[1] & 0x0f) << 2];
         result[i++] = __base64_pad;
      }
      else {
         result[i++] = __base64_table[(current[0] & 0x03) << 4];
         result[i++] = __base64_pad;
         result[i++] = __base64_pad;
      }
   }
   if(ret_length) {
      *ret_length = i;
   }
   result[i] = '\0';
   return result;
}

/* as above, but backwards. :) */
unsigned char *__base64_decode(const unsigned char *str, int length, int *ret_length) {
   const unsigned char *current = str;
   int ch, i = 0, j = 0, k;
   /* this sucks for threaded environments */
   static short reverse_table[256];
   static int table_built;
   unsigned char *result;

   if (++table_built == 1) {
      char *chp;
      for(ch = 0; ch < 256; ch++) {
         chp = strchr(__base64_table, ch);
         if(chp) {
            reverse_table[ch] = chp - __base64_table;
         } else {
            reverse_table[ch] = -1;
         }
      }
   }

   result = (unsigned char *)malloc(length + 1);
   if (result == NULL) {
      return NULL;
   }

   /* run through the whole string, converting as we go */
   while ((ch = *current++) != '\0') {
      if (ch == __base64_pad) break;

      /* When Base64 gets POSTed, all pluses are interpreted as spaces.
         This line changes them back.  It's not exactly the Base64 spec,
         but it is completely compatible with it (the spec says that
         spaces are invalid).  This will also save many people considerable
         headache.  - Turadg Aleahmad <turadg@wise.berkeley.edu>
      */

      if (ch == ' ') ch = '+';

      ch = reverse_table[ch];
      if (ch < 0) continue;

      switch(i % 4) {
      case 0:
         result[j] = ch << 2;
         break;
      case 1:
         result[j++] |= ch >> 4;
         result[j] = (ch & 0x0f) << 4;
         break;
      case 2:
         result[j++] |= ch >>2;
         result[j] = (ch & 0x03) << 6;
         break;
      case 3:
         result[j++] |= ch;
         break;
      }
      i++;
   }

   k = j;
   /* mop things up if we ended on a boundary */
   if (ch == __base64_pad) {
      switch(i % 4) {
      case 0:
      case 1:
         free(result);
         return NULL;
      case 2:
         k++;
      case 3:
         result[k++] = 0;
      }
   }
   if(ret_length) {
      *ret_length = j;
   }
   result[k] = '\0';
   return result;
}

#include "encode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
   unsigned char *str, *dst;
   char *source = "hello world";
   int  size;

   str = __base64_encode((unsigned char *)source, strlen(source), &size);
   printf("%s : %d\n", str, size);
   dst = __base64_decode(str, strlen(str), &size);
   printf("%s : %d\n", dst, size);

   free(str);
   free(dst);

   return 0;
}

encode.h
unsigned char *__base64_encode(const unsigned char *str, int length, int *ret_length);
unsigned char *__base64_decode(const unsigned char *str, int length, int *ret_length);





	
===============================================================================	
