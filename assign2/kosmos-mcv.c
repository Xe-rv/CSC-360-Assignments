/*
 * kosmos-mcv.c (mutexes & condition variables)
 *
 * For UVic CSC 360, Spring 2024
 *
 * Here is some code from which to start.
 *
 * PLEASE FOLLOW THE INSTRUCTIONS REGARDING WHERE YOU ARE PERMITTED
 * TO ADD OR CHANGE THIS CODE. Read from line 133 onwards for
 * this information.
 */

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logging.h"


/* Random # below threshold that particular atom creation. 
 * This code is a bit fragile as it depends upon knowledge
 * of the ordering of the labels.  For now, the labels 
 * are in alphabetical order, which also matches the values
 * of the thresholds.
 */

#define C_THRESHOLD 0.2
#define H_THRESHOLD 0.8
#define O_THRESHOLD 1.0
#define DEFAULT_NUM_ATOMS 40

#define MAX_ATOM_NAME_LEN 10
#define MAX_KOSMOS_SECONDS 5

/* Global / shared variables */
int  cNum = 0, hNum = 0, oNum = 0;
long numAtoms;


/* Function prototypes */
void kosmos_init(void);
void *c_ready(void *);
void *h_ready(void *);
void *o_ready(void *);
void make_radical(int, int, int, int, int, char *);
void wait_to_terminate(int);


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
	int *pi = (int *)malloc(sizeof(int));
	assert( pi != NULL);
	*pi = i;
	return pi;
}




int main(int argc, char *argv[])
{
	long seed;
	numAtoms = DEFAULT_NUM_ATOMS;
	pthread_t **atom;
	int i;
	int status;
    double random_value;

	if ( argc < 2 ) {
		fprintf(stderr, "usage: %s <seed> [<num atoms>]\n", argv[0]);
		exit(1);
	}

	if ( argc >= 2) {
		seed = atoi(argv[1]);
	}

	if (argc == 3) {
		numAtoms = atoi(argv[2]);
		if (numAtoms < 0) {
			fprintf(stderr, "%ld is not a valid number of atoms\n",
				numAtoms);
			exit(1);
		}
	}

    kosmos_log_init();
	kosmos_init();

	srand(seed);
	atom = (pthread_t **)malloc(numAtoms * sizeof(pthread_t *));
	assert (atom != NULL);
	for (i = 0; i < numAtoms; i++) {
		atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
        random_value = (double)rand() / (double)RAND_MAX;

		if ( random_value <= C_THRESHOLD ) {
			cNum++;
			status = pthread_create (
					atom[i], NULL, c_ready,
					(void *)dupInt(cNum)
				);
		} else if (random_value <= H_THRESHOLD ) {
			hNum++;
			status = pthread_create (
					atom[i], NULL, h_ready,
					(void *)dupInt(hNum)
				);
		} else if (random_value <= O_THRESHOLD) {
			oNum++;
			status = pthread_create (
					atom[i], NULL, o_ready,
					(void *)dupInt(oNum)
				);
        } else {
            fprintf(stderr, "SOMETHING HORRIBLY WRONG WITH ATOM GENERATION\n");
            exit(1);
        } 

		if (status != 0) {
			fprintf(stderr, "Error creating atom thread\n");
			exit(1);
		}
	}

    /* Determining the maximum number of ethynyl radicals is fairly
     * straightforward -- it will be the minimum of the number of
     * cNum, oNum, and hNum / 3.
     */
    int max_radicals = 0;

    if (cNum <= oNum && cNum < hNum / 3) {
        max_radicals = cNum;
    } else if (oNum <= cNum && oNum < hNum / 3) {
        max_radicals = oNum;
    } else {
        max_radicals = (int)(hNum / 3);
    }
#ifdef VERBOSE
    printf("Maximum # of radicals expected: %d\n", max_radicals);
#endif

    wait_to_terminate(max_radicals);
}

/*
* Now the tricky bit begins....  All the atoms are allowed
* to go their own way, but how does the Kosmos ethynyl-radical
* problem terminate? There is a non-zero probability that
* some atoms will not become part of a radical; that is,
* many atoms may be blocked on some semaphore of our own
* devising. How do we ensure the program ends when
* (a) all possible radicals have been created and (b) all
* remaining atoms are blocked (i.e., not on the ready queue)?
*/



/*
 * ^^^^^^^
 * DO NOT MODIFY CODE ABOVE THIS POINT.
 *
 *************************************
 *************************************
 *
 * ALL STUDENT WORK MUST APPEAR BELOW.
 * vvvvvvvv
 */


/* 
 * DECLARE / DEFINE NEEDED VARIABLES IMMEDIATELY BELOW.
 */

int radicals = 0;
int num_free_c = 0;
int num_free_h = 0;
int num_free_o = 0;
int gen = 0;
int stop = 0;

pthread_mutex_t mutex;
pthread_cond_t wait_c;
pthread_cond_t wait_h;
pthread_cond_t wait_o;
pthread_cond_t wait;
pthread_cond_t staging_area;

// variables used for logging
int hAtom[3];
int hAtomCount = 0;
int cAtom;
int oAtom;


/*
 * FUNCTIONS YOU MAY/MUST MODIFY.
 */

void kosmos_init() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&wait_h, NULL);
    pthread_cond_init(&wait_c, NULL);
    pthread_cond_init(&wait_o, NULL);
    pthread_cond_init(&wait, NULL);
    pthread_cond_init(&staging_area, NULL);
}

void radicalize(char name[], char* atom, int ID)
{
    pthread_mutex_lock(&mutex);
    // removes the amount of free atoms
    num_free_c--;
    num_free_h -= 3;
    num_free_o--;
    gen++;
    
    // checks which type of atom triggered the creation to see which threads need to be unlocked
    if(strncmp(atom, "h", MAX_ATOM_NAME_LEN) == 0){
        hAtom[hAtomCount] = ID;
        hAtomCount++;
    } else {
        pthread_cond_signal(&wait_h);
        pthread_cond_wait(&staging_area, &mutex);
    }

    if(strncmp(atom, "c", MAX_ATOM_NAME_LEN) == 0){
        cAtom = ID;
    } else {
        pthread_cond_signal(&wait_c);
        pthread_cond_wait(&staging_area, &mutex);
    }

    if(strncmp(atom, "o", MAX_ATOM_NAME_LEN) == 0){
        oAtom = ID;
    } else {
        pthread_cond_signal(&wait_o);
        pthread_cond_wait(&staging_area, &mutex);
    }
    
    pthread_cond_signal(&wait_h);
    pthread_cond_wait(&staging_area, &mutex);
    pthread_cond_signal(&wait_h);
    pthread_cond_wait(&staging_area, &mutex);

    // logs and prints the radical for checking
    make_radical(cAtom, oAtom, hAtom[0], hAtom[1], hAtom[2], name);
    radicals++;
    hAtomCount = 0;
    pthread_mutex_unlock(&mutex);
}


void *h_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "h%03d", id);

#ifdef VERBOSE
	printf("%s now exists\n", name);
#endif

    pthread_mutex_lock(&mutex);

    num_free_h++;

    // checks if all atoms have been made for the radical and triggers it with the current atom if it is
    if(num_free_h >= 3 && num_free_c >= 1 && num_free_o >= 1) {
        pthread_mutex_unlock(&mutex);
        radicalize(name, "h", id);
        return NULL;
    }

    int my_gen = gen;
    while(my_gen == gen) {
        pthread_cond_wait(&wait_h, &mutex);
    }

    // passes atom into array for logging
    hAtom[hAtomCount] = id;
    hAtomCount++;

    pthread_cond_signal(&staging_area);

    pthread_mutex_unlock(&mutex);
	return NULL;
}


void *c_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "c%03d", id);

#ifdef VERBOSE
	printf("%s now exists\n", name);
#endif

    pthread_mutex_lock(&mutex);

    num_free_c++;

    // checks if all atoms have been made for the radical and triggers it with the current atom if it is
    if(num_free_h >= 3 && num_free_c >= 1 && num_free_o >= 1) {
        pthread_mutex_unlock(&mutex);
        radicalize(name, "c", id);
        return NULL;
    }

    // waits for radical trigger
    int my_gen = gen;
    while(my_gen == gen) {
        pthread_cond_wait(&wait_c, &mutex);
    }

    // passes atom into variable for logging
    cAtom = id;

    pthread_cond_signal(&staging_area);

    pthread_mutex_unlock(&mutex);
    
	return NULL;
}


void *o_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "o%03d", id);

#ifdef VERBOSE
	printf("%s now exists\n", name);
#endif

    pthread_mutex_lock(&mutex);

    num_free_o++;

    // checks if all atoms have been made for the radical and triggers it with the current atom if it is
    if(num_free_h >= 3 && num_free_c >= 1 && num_free_o >= 1) {
        pthread_mutex_unlock(&mutex);
        radicalize(name, "o", id);
        return NULL;
    }

    //waits for radical trigger
    int my_gen = gen;
    while(my_gen == gen) {
        pthread_cond_wait(&wait_o, &mutex);
    }

    // passes atom into variable for logging
    oAtom = id;

    pthread_cond_signal(&staging_area);
    
    pthread_mutex_unlock(&mutex);
	return NULL;
}


/* 
 * Note: The function below need not be used, as the code for making radicals
 * could be located with c_ready, h_ready, or o_ready. However, it is
 * perfectly possible that you have a solution which depends on such a
 * function having a purpose as intended by the function's name.
 */
void make_radical(int c, int o, int h1, int h2, int h3, char *maker)
{
#ifdef VERBOSE
	fprintf(stdout, "A methoxy radical was made: c%03d  o%03d  h%03d h%03d h%03d \n",
		c, o, h1, h2, h3);
#endif
    kosmos_log_add_entry(radicals, c, o, h1, h2, h3, maker);
}


void wait_to_terminate(int expected_num_radicals) {
    /* A rather lazy way of doing it, for now. */
    sleep(MAX_KOSMOS_SECONDS);
    kosmos_log_dump();
    exit(0);
}
