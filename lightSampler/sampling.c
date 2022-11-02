#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include "sampling.h"

#define EMPTY 0

bool threadIsStopped = false;
long long totalSamples = 0;
void *defaultArg = NULL;
int size = 0;
samplerDatapoint_t* outputArray = NULL;
pthread_t thread;
pthread_mutex_t historyMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(historyMutex, NULL);

int readingLightSampler(){
    // Open file
  FILE *f = fopen(A2D_FILE, "r");
  if (!f) {
    printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
    printf(" Check /boot/uEnv.txt for correct options.\n");
    exit(-1);
  }
  // Get reading
  int a2dReading = 0;
  int itemsRead = fscanf(f, "%d", &a2dReading);
  if (itemsRead <= 0) {
    printf("ERROR: Unable to read values from voltage input file.\n");
    exit(-1);
  }
  // Close file
  fclose(f);
  return a2dReading;

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


void lockMutex(){
    pthread_mutex_lock(&historyMutex);
}

void unlockMutex(){
    pthread_mutex_unlock(&historyMutex);
}

void addSampleToOutputArr(double sampleVoltage){
    lockMutex();
    outputArray[size].sampleInV = sampleVoltage;
    outputArray[size].timestampInNanoS = getTimeInNs();
    size++;
    unlockMutex();
}

static void *sampling(void* args){
    while(true){
        if(threadIsStopped == false){
            totalSamples+=1;
            int lightReading = readingLightSampler();
            double lightVoltage = ((double)lightReading/4095) * 1.8;
            addSampleToOutputArr(lightVoltage);
            sleepForMs(1);
        }
        else{
            return NULL;
        }
    
    }
}

samplerDatapoint_t *Sampler_extractAllValues(){
    // samplerDatapoint_t *outputArrayCopy = NULL;
    lockMutex();
    // outputArrayCopy = malloc(sizeof(*outputArray) * size);
    // for(int i = 0; i < size; i++){
    //     memcpy(&outputArrayCopy[i], &outputArray[i], sizeof(samplerDatapoint_t));
    // }
    size = 0;
    unlockMutex();
    return outputArray;

}

void Sampler_startSampling(void){
    lockMutex();
    samplerDatapoint_t *sampleHistory = malloc(sizeof(*sampleHistory) * 4096*10);
    memset(sampleHistory, 0, 4096*10 * sizeof(*sampleHistory));
    unlockMutex();

    pthread_attr_t defaultAttr;
    pthread_attr_init(&defaultAttr);

    struct sched_param defaultParam;
    pthread_attr_getschedparam(&defaultAttr, &defaultParam);
    defaultParam.sched_priority +=1;
    pthread_attr_setschedparam(&defaultAttr, &defaultParam);

    pthread_create(&thread, &defaultAttr, &sampling, &defaultArg);
}

void Sampler_stopSampling(void){
    threadIsStopped = true;
    pthread_join(thread, NULL);
    lockMutex();

    memset(outputArray, EMPTY, sizeof(outputArray)*4096*10);
    size = 0;

    unlockMutex();

}

int Sampler_getNumberOfSamplesInHistory(){
    return size;
}

long long Sampler_getNumSamplesTaken(void){
    return totalSamples;
}