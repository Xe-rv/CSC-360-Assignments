#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  /* Need this for `sleep` */
#include <pthread.h> 

/* Plain-old C function that is executed as a thread 
 * when its name is specified in pthread_create().
 * Notice the function signature.

void *some_thread(void *argp) 
{ 
	sleep(1); 
	printf("Printing some kind of sentence\n"); 
	return NULL; 
} 

int main() 
{ 
	pthread_t thread_id; 

	printf("Music from '2001: A Space Oddysey' ...\n"); 
	pthread_create(&thread_id, NULL, some_thread, NULL); 
	pthread_join(thread_id, NULL); 
	printf("Sad trombone music ...\n"); 

	exit(0); 
}

*/

struct three_int_t {
    int first, second, third;
};

void *some_thread(void *argp) { 
    sleep(1);
    if (argp != NULL) { 
        struct three_int_t *numbs = (struct three_int_t *) argp;
        int num = numbs->first * numbs->second * numbs->third;
        printf("%d\n", num);
    } else {
        printf("You have not entered 3 numbers!\n"); 
    }
    return NULL; 
} 

int main(int argc, char *argv[]) { 
    pthread_t thread_id; 
    struct three_int_t values;
    
    if (argc < 4) {
        pthread_create(&thread_id, NULL, some_thread, NULL);
    } else {
        //struct three_int_t values;
        values.first = atoi(argv[1]);
        values.second = atoi(argv[2]);
        values.third = atoi(argv[3]);
        
        pthread_create(&thread_id, NULL, some_thread, (void *)&values); 
    }
    pthread_join(thread_id, NULL);  
    
    exit(0); 
}