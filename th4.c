#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <semaphore.h>
#include <pthread.h>

///const int NTHREADS = 4;
#define NTHREADS 4
char *messages [NTHREADS] = {
    "Good day!",
    "Konnichiwa!",
    "Mingalarbar!",
    "Xin chao!"
}; 


struct thdata {
    char                *letter;
    pthread_t           th;
    sem_t               sync;
    sem_t               start;
};


void *thread_function(void *thdata)
{

    struct thdata       *priv = (struct thdata *)thdata;

    /* sync */
    sem_post(&priv->sync);
    sem_wait(&priv->start);

    /* write my letter */
    printf("%s\n",priv->letter);

    /* sync */
    sem_post(&priv->sync);

    /* done */
    return (void *) NULL;

}


int main (void)
{

    int                 rtn, i;
    struct thdata       *thdata;

    /* initialize thread data */
    thdata = calloc(sizeof(struct thdata), NTHREADS);
    if (thdata == NULL) {
        perror("calloc()");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NTHREADS; i++) {
        thdata[i].letter = messages[i];
        sem_init(&thdata[i].sync, 0, 0);
        sem_init(&thdata[i].start, 0, 0);
        rtn = pthread_create(&thdata[i].th, NULL, thread_function, (void *) (&thdata[i]));
        if (rtn != 0) {
            fprintf(stderr, "pthread_create() #%0d failed for %d.", i, rtn);
            exit(EXIT_FAILURE);
        }
    }


    /* synchronization */
    for (i = 0; i < NTHREADS; i++) {
        sem_wait(&thdata[i].sync);
    }

    /* let thread write his letter */
    for (i = 0; i < NTHREADS; i++) {
        sem_post(&thdata[i].start);
        sem_wait(&thdata[i].sync);
    }

    /* join */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(thdata[i].th, NULL);
        sem_destroy(&thdata[i].sync);
        sem_destroy(&thdata[i].start);
    }


    free(thdata);
    exit(EXIT_SUCCESS);

}