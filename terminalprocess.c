#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>


sem_t* semaphore;
pid_t otherPid;
sigset_t sigSet;


void signalHandler1(int signum){

  printf("Caught signal: %d\n",  signum);
    printf("Exit child Process .\n");
    sem_post(semaphore);
  _exit(0);

}


void signalHandler2(int signum){

  printf("I am alive\n");


}

void childProcess(){

  signal(SIGUSR1, signalHandler1);
    signal(SIGUSR2, signalHandler2);


    int value;


    sem_getvalue(semaphore, &value);
    printf("child process semaphore count is %d.\n", value);
    printf("child process  is grabbing  semaphore\n");
    sem_wait(semaphore);
    sem_getvalue(semaphore, &value);
    printf("child process semaphore count is %d.\n", value);


    printf("starting very long child process.\n");

    for(int x=0; x<60; ++x){
      printf(".\n" );
      sleep(1);

    }

    sem_post(semaphore);

    printf("Exit child process\n" );
    _exit(0);
  }


  void *checkHungChild(void *a){


    int* status = a;

    printf("checking for hung child process ......\n" );
    sleep(10);
    if(sem_trywait(semaphore) != 0){


      printf("Child process appears to be hung\n" );
      *status = 1;

    }
    else{

      printf("child process appears to ruuning fine ...\n" );

      *status = 0;
    }
    return NULL;
  }

void parentProcess(){

  sleep(2);
  if(getpgid(otherPid) >= 0 ){

    printf("Child process is running ....\n" );

  }

  int value;

  sem_getvalue(semaphore, &value);
  printf("In the parent process wit the semaphore count of %d.\n", value );


  if(sem_trywait(semaphore) != 0){


    pthread_t tid1;


    int status = 0;

    printf("Detected child is hung or running too long ....\n" );
    if(pthread_create(&tid1, NULL, checkHungChild, &status)){

      printf("ERROR creating timer thread.\n" );
      _exit(1);


    }
    if(pthread_join(tid1, NULL)){

      printf("ERROR joining timer thread.\n" );
      _exit(1);


  }


  if(status == 1){


    printf("Going  to kill child process with ID of %d\n",otherPid );
    kill(otherPid, SIGTERM);
    printf("Killed child process\n" );

    printf("Now proving child process is killed (you should see no dots and no response from SIGUSR2 signal\n" );
      sleep(5);
      kill(otherPid, SIGUSR2);
      sleep(1);
      printf("Done proving child is killed \n");

      sem_getvalue(semaphore, &value);
      printf("In the parent process with the semaphore count of %d.\n",value );
      if(sem_trywait(semaphore) != 0){

        if(value == 0)
          sem_post(semaphore);
          printf("cleaned up and finally got the semaphore\n" );
          sem_getvalue(semaphore, &value);
          printf("In the parent process wit the semaphore count of %d.\n",value );

      }
      else{

        printf("Finally got the semaphore\n" );
      }
  }
}

printf("Exit parent process .\n" );
_exit(0);


}

int main(int argc, char*argv[]){

  pid_t pid;

  semaphore = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
  if(sem_init(semaphore, 1, 1)!= 0){

    printf("failed to create semaphore.\n" );
    exit(EXIT_FAILURE);

  }

  pid = fork();
  if(pid == -1){

    printf("Cant fork , error.\n" );
    exit(EXIT_FAILURE);

  }
  pid = fork();

  if(pid == -1){

    printf("cant fork, error\n" );
    exit(EXIT_FAILURE);

  }


  if(pid == 0){

    printf("started child process with process id of %d....\n",getpid());
    otherPid = getppid();
    childProcess();

  }
  else{
      printf("started child process with process id of %d....\n",getpid());
    otherPid = getppid();
    parentProcess();

  }

  sem_destroy(semaphore);

  return 0;
}
