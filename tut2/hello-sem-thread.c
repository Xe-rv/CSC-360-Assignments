#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <time.h>
#include <semaphore.h>

sem_t phase_zero_to_one;
sem_t phase_one_to_two;

void *phase_zero(void *argp)  
{ 
    printf("Phase zero: start\n");
    struct timespec ts = {3, 0}; 
    nanosleep(&ts, NULL);
    printf("Phase zero: end\n");
    sem_post(&phase_zero_to_one);
    
    return NULL;
} 


void *phase_one(void *argp)  
{
    printf("Phase 1: start\n");
    struct timespec ts = {3, 0}; 
    nanosleep(&ts, NULL);
    sem_wait(&phase_zero_to_one);
    printf("Phase 1: end\n");
    sem_post(&phase_one_to_two);

    return NULL;
}


void *phase_two(void *argp) {
    printf("Phase 2: start\n");
    struct timespec ts = {3, 0}; 
    nanosleep(&ts, NULL);
    sem_wait(&phase_one_to_two);
    printf("Phase 2: end\n");

    return NULL;
}

int main()  
{ 
    //int i;  
    pthread_t tid[3]; 

    sem_init(&phase_zero_to_one, 0, 0);
    sem_init(&phase_one_to_two, 0, 0);

    /* Create the three phases. */
    int r2 = pthread_create(&tid[2], NULL, phase_two, NULL);
    int r0 = pthread_create(&tid[0], NULL, phase_zero, NULL);
    int r1 = pthread_create(&tid[1], NULL, phase_one, NULL);

    if (r0 || r1 || r2) {
        /* If any of the return values from pthread_create are non-zero,
         * then there was an error, and we must terminate.
         */
        fprintf(stderr, "Error when creating threads.\n");
        exit(1);
    }
    pthread_exit(NULL); 
    return 0; 
} 