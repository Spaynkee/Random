#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 1024
     
void sum1(); //sum 1 to .5 billion
void sum2(); //sum .5 billion to 1 billion
void sum3(); //sum 1 billion to 1.5 billion
void sum4(); //sum 1.5 billion to 2 billion

key_t key;

int shmid;

long *sumsLoc;


int main(){
  int status;
  char *sm2;
    /* make the key: */
    if ((key = ftok("smFar.c", 'R')) == -1) /*Here the file must exist */ 
    {
        perror("ftok");
        exit(1);
    }

    /*  create the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }
    
    //Gets our shared memory location
    sumsLoc = shmat(shmid, (void *)0, 0);
    
    pid_t pid1 = fork();
  
  //timer
  struct timeval start, end;
  long mtime, seconds, useconds;  
  gettimeofday(&start, NULL); //timer
   
  if(pid1 < 0){         //fork failed
    fprintf(stderr, "Fork Failed!");
    return 1;
  }else if(pid1 == 0){  //child process
    pid_t pid2 = fork();
    if(pid2==0){
        pid_t pid3 = fork();
        if(pid3==0){
            sum4();
        }else{
            sum3();
            wait(NULL);
        }
    }else{
      sum2();
      wait(NULL);
    }
  }else
  {
    sum1();
    wait(NULL);
	
	  gettimeofday(&end, NULL); //timer
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Elapsed time: %ld milliseconds on pid=%d\n", mtime, pid1); 
    
    //Prints out full sum by grabbing values at each of the addresses set in shared memory.
    long fullsum = sumsLoc[0] + sumsLoc[7] + sumsLoc[15] + sumsLoc[23];
    
    printf("%ld is the full sum.\n", fullsum);
     
    shmctl(shmid, IPC_RMID, NULL); //Removes the shared memory so I can stop typing ipcrm -M 0x5...
    
  }

  return 0;
}

void sum1(){
  for(int i=1;i<500000000;i++){
    sumsLoc[0] += i;
  }
  printf("Sum1: The current sum is %ld\n", sumsLoc[0]);
}

void sum2(){
  for(int i=500000000;i<1000000000;i++){
   sumsLoc[7] += i;
  }
  //Store the sum into shared memory?
  printf("Sum2: The current sum is %ld\n", sumsLoc[7]);
}
void sum3(){
  for(int i=1000000000;i<1500000000;i++){
    sumsLoc[15] += i;
  }
  printf("Sum3: The current sum is %ld\n", sumsLoc[15]);
}

void sum4(){
  for(int i=1500000000;i<2000000001;i++){
      sumsLoc[23] += i;
  }
  printf("Sum4: The current sum is %ld\n", sumsLoc[13]);
}
