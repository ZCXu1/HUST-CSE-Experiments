#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>



#define BufferSize 10 // Size of the buffer
#define countOfProducer 2
#define countOfConsumer 3

sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int buffer[BufferSize]={0};

pthread_mutex_t mutex;

void *producer(void *pno)
{   
    int item;
    while(1){
        item = rand()%1000+1000*(*((int *)pno)); // Produce an random item
        int time = rand()%900+100;
        usleep(time);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("Producer %d: Produce Item %d at %d\n", *((int *)pno),buffer[in],in);
        in = (in+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer(void *cno)
{   
    while(1){
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %d: Consume Item %d from %d\n",*((int *)cno),item, out);
        out = (out+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        int time = rand()%900+100;
        usleep(time);
    }
}

int main()
{   

    pthread_t pro[countOfProducer],con[countOfConsumer];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BufferSize);
    sem_init(&full,0,0);

    int a[3] = {1,2,3}; //Just used for numbering the producer and consumer

    for(int i = 0; i < countOfProducer; i++) {
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
    for(int i = 0; i < countOfConsumer; i++) {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    for(int i = 0; i < countOfProducer; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < countOfConsumer; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
    
}
