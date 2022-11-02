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

// static void sampling(void* args){

// }

void Sampler_startSampling(void){
    lockMutex();
    sampleHistory = malloc(sizeof(*sampleHistory) * 4096*10);
    //memset(sampleHistory, 0, 4096*10 * sizeof(*sampleHistory));
}

long long getTimeInMs(void){
 struct timespec spec;
 clock_gettime(CLOCK_REALTIME, &spec);
 long long seconds = spec.tv_sec;
 long long nanoSeconds = spec.tv_nsec;
 long long milliSeconds = seconds * 1000
 + nanoSeconds / 1000000;
 return milliSeconds;
}

long long getTimeInNs(void){
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  long long seconds = spec.tv_sec;
  long long nanoSeconds = spec.tv_nsec + seconds *1000000000;
  return nanoSeconds;
}

void sleepForMs(long long delayInMs){
 const long long NS_PER_MS = 1000 * 1000;
 const long long NS_PER_SECOND = 1000000000;
 long long delayNs = delayInMs * NS_PER_MS;
 int seconds = delayNs / NS_PER_SECOND;
 int nanoseconds = delayNs % NS_PER_SECOND;
 struct timespec reqDelay = {seconds, nanoseconds};
 nanosleep(&reqDelay, (struct timespec *) NULL);
}

long long intervalSinceTimeStarted(long long initialTime){
  long long currentTime = getTimeInNs();
  long long interval = currentTime - initialTime;
  return interval;
}