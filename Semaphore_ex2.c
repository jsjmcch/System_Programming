#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_snd(void *arg);
void *thread_rcv(void *arg);

sem_t bin_sem;	// 세마포어~
int number = 0;	// global variable, 모든 쓰레드 접근가능

// Three thread
// A Thread : number를 1로, B/C Thread는 number를 0으로 만듬
char thread1[] = "A Thread";
char thread2[] = "B Thread";
char thread3[] = "C Thread";

int main(int argc, char **argv)
{
	pthread_t t1, t2, t3;
	void *thread_result;
	int state;
	
	state = sem_init(&bin_sem,0,0);	// 초기화, 처음에 0으로 설정
	if (state != 0) 
		puts("Error semaphore initialization");
	
	// create Thread.
	pthread_create(&t1, NULL, thread_snd, &thread1);
	pthread_create(&t2, NULL, thread_rcv, &thread2);
	pthread_create(&t3, NULL, thread_rcv, &thread3);
	
	// 프로세스가 먼저 종료되어 쓰레드가 중간에 종료되지 않도록 함
	pthread_join(t1, &thread_result);
	pthread_join(t2, &thread_result);
	pthread_join(t3, &thread_result);
	
	printf("Last number : %d\n", number);
	sem_destroy(&bin_sem);	// destroy semaphore
	return 0;	
} 

void *thread_snd(void *arg){
	int i;
	for (i=0; i<4; i++) {
		while (number != 0)	// 만일 number가 0이 아니라면
			sleep(1);	// number가 0이 될때까지 기다림
			
		number++;
		printf("Execution : %s, number : %d\n", (char *)arg, number);
		sem_post(&bin_sem);	//semaphore를 1로 만드는 것 
	}
}

void *thread_rcv(void *arg) {
	int i;
	for (i=0; i<2; i++) {
		sem_wait(&bin_sem);	// semaphonre를 0으로 만듬
		number--;
		printf("Execution : %s, number : %d\n", (char *)arg, number); 
	}
}
