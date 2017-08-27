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

   arg.val  =  1;                // ¼¼¸¶Æ÷¾î °ªÀ» 1·Î ¼³Á¤
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
