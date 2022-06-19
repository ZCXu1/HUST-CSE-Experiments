#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<pthread.h>

void* pthread_A(void* arg)
{
    for(int i = 0; i < 1000; i++)
    {
        printf("A:%d\n",i);
        sleep(0.2);
    }
}

int main()
{

    pthread_t tid;
    int res = pthread_create(&tid,NULL,pthread_A,NULL);
   
    for(int i = 1000; i > 0; i--)
    {
        printf("B:%d\n",i);
        sleep(0.2);
    }

}


