#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include "sampling.h"

long long totalSamples = 0;
static samplerDatapoint_t *sampleHistory = NULL;
pthread_mutex_t historyMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(historyMutex, NULL);

void lockMutex(){
    pthread_mutex_lock(&historyMutex);
}

void unlockMutex(){
    pthread_mutex_unlock(&historyMutex);
}

static void sampling(void* args){

}

void Sampler_startSampling(void){
    lockMutex();
    sampleHistory = malloc(sizeof(*sampleHistory) * 4096*10);
    //memset(sampleHistory, 0, 4096*10 * sizeof(*sampleHistory));
}