/*
Class: 346 01
Assignment: assignment 6
Team Leader: Myanna Harris mharris5
11-4-15
Producer and consumer problem using semaphores
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

//Semaphore struct explanation
/*
 struct sembuf 
 {
   short sem_num;
   short sem_op;  -1 is down, 1 is up
   short sem_flg;
 }
*/

//Rename so we can use the name semaphore
typedef struct sembuf semaphore;

void criticalSection(int);
void producer(semaphore[], semaphore[], int, int, int);
void consumer(semaphore[], semaphore[], int, int, int);

//Constants to ID producer and consumer in critical section
const int PROD = 0;
const int CONS = 1;

//Run problem
int main(int argc, char* argv[])
{
  //create operation semaphores
  semaphore sem_down[1], sem_up[1];

 //create memory variables
 int mutex;
 int empty;
 int full;
 short int outM[1];
 short  int outE[1];
  short int outF[1];

  outM[0];
  outE[0];
  outF[0];

 //instantiate incrementing and decrementing semaphores
 sem_down[0].sem_num = 0;
 sem_down[0].sem_op = -1; //decrement
 sem_down[0].sem_flg = SEM_UNDO; 

 sem_up[0].sem_num = 0;
 sem_up[0].sem_op = 1; //increment
 sem_up[0].sem_flg = SEM_UNDO; 

 //create semaphores set of 1  semaphore
 mutex = semget(0, 1, 0777 | IPC_CREAT);
 empty = semget(1, 1, 0777 | IPC_CREAT);
 full = semget(2, 1, 0777 | IPC_CREAT);

 /*initialize  values of semaphores*/
 semctl(mutex, 0 , SETVAL, 1);
 semctl(empty, 0 , SETVAL, 100);
 semctl(full, 0, SETVAL, 0);

  //print values before
 semctl(mutex, 1, GETALL, outM);
 semctl(empty, 1, GETALL, outE);
 semctl(full, 1, GETALL, outF);
  printf("Initial semaphore values: mutex = %d, empty = %d, full = %d\n", outM[0], outE[0], outF[0]);

    //parbegin
 //fork here
  if (fork() == 0)
    producer(sem_down, sem_up, mutex, empty, full);
  else 
    consumer(sem_down, sem_up, mutex, empty, full);
    //parend

   //print values after
 semctl(mutex, 1, GETALL, outM);
 semctl(empty, 1, GETALL, outE);
 semctl(full, 1, GETALL, outF);
    printf("Final semaphore values: mutex = %d, empty = %d, full = %d\n", outM[0], outE[0], outF[0]);

 /*cleanup*/
 semctl(mutex, 1, IPC_RMID, NULL);
 semctl(empty, 1, IPC_RMID, NULL);
 semctl(full, 1, IPC_RMID, NULL);
}

void producer(semaphore sem_down[], semaphore sem_up[], int mutex, int empty, int full)
{
  int i;
  //add items to buffer
  for(i = 0; i < 10; i++)
   {
     //delete an empty spot and decrement mutex so CS can't be interrupted
    semop(empty, sem_down, 1);
    semop(mutex, sem_down, 1);

     //Critical section
     printf("\n\n\n");
     printf("About to enter producer's critical section\n");
     criticalSection(PROD);
     printf("Leaving producer's critical section\n");
     printf("\n\n\n");

    //add full spot and  increment mutex so consumer can consume
    semop(mutex, sem_up, 1);
    semop(full, sem_up, 1); //experiment with putting delays after this line
    //sleep(1);
   }
}

void consumer(semaphore sem_down[], semaphore sem_up[], int mutex, int empty, int full)
{
  int i;
  //consume items from buffer
  for (i = 0; i < 10; i++)
   {
     //delete a full spot and decrement mutex so CS can't be interrupted
    semop(full, sem_down, 1);
    semop(mutex, sem_down, 1);

    //critical section
    printf("\n\n\n");
    printf("About to enter consumer's critical section\n");
    criticalSection(CONS);
    printf("Leaving consumer's critical section\n");
    printf("\n\n\n");

     //add empty spot and  increment mutex so producer can produce
    semop(mutex, sem_up, 1);
    semop(empty, sem_up, 1); //experiment with putting delays after this line
    //sleep(1);
 }
}

//print message in critical section
void criticalSection(int who)
{
  if (who == PROD)
     printf("Make an item and send it to the consumer in a full container\n");
 else
    printf("Use an item and send back the empty\n");
 } 