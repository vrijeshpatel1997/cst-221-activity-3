#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t mutex;
long counter;
time_t end_time;


void *counter_thread(void *args)
{

  int status;

  while(time (NULL) < end_time)
  {


    status = pthread_mutex_lock(&mutex);
    if(status == 0)
    printf("Counter thread : locked mutex for counter so it can be incremented\n" );
    ++counter;
    status = pthread_mutex_unlock(&mutex);

    if(status == 0)
    printf("Counter thread : unlocked mutex for counter so it can be incremented\n" );


sleep(1);
  }

  printf(" final Counter is %lu\n", counter );
  return NULL;


}


void *monitor_thread(void *arg)
{

  int status;
  int missses = 0;

  while(time (NULL) < end_time)
  {


    sleep(3);

    status = pthread_mutex_trylock(&mutex);
    if(status != EBUSY)
    {

      printf(" Monitor Thrad: the trylock will lock the mutex so we can safely read the counter \n" );
      printf(" Monitor Thrad: the Counter from monitor is   %ld\n", counter );
      status = pthread_mutex_unlock(&mutex);
      if(status == 0)
        printf(" Monitor Thrad: will now  unlock the mutex  since we are done with the counter\n" );

      }
       else
       {

        ++missses;
      }

    }

    printf(" Final Monitor  Thrad: misseed was   %d times.\n", missses );
    return NULL;



}


int main(int argc, char *argv[]){


  int status;

  pthread_t counter_thread_id;
  pthread_t monitor_thread_id;



  pthread_mutex_init(&mutex, 0);

  end_time = time(NULL) + 60;


  if(pthread_create(&counter_thread_id, NULL, counter_thread,NULL))
  printf("Create counter thread failed " );



    if(pthread_create(&monitor_thread_id, NULL, monitor_thread,NULL))
    printf("Create monitor thread failed " );



      if(pthread_join(counter_thread_id, NULL))
      printf("Joined counter thread failed " );



        if(pthread_join(monitor_thread_id, NULL))
        printf("Joined monitor thread failed " );

return 0;

}
