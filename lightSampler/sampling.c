#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <malloc.h>
#include <sched.h>
#include <math.h>
#include "sampling.h"

#define A2D_FILE "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define EMPTY 0

static bool threadIsStopped = false;
static long long totalSamples = 0;
static int endOfHistory = 0;
static samplerDatapoint_t* outputArray = NULL;
static pthread_t thread;
static pthread_mutex_t historyMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(historyMutex, NULL);

static int readingLightSampler(){
    char A2D_FILE_VOLTAGE1[500];
    sprintf(A2D_FILE_VOLTAGE1, A2D_FILE); 
    FILE *f = fopen(A2D_FILE_VOLTAGE1, "r");
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


static void lockMutex(){
    pthread_mutex_lock(&historyMutex);
}

static void unlockMutex(){
    pthread_mutex_unlock(&historyMutex);
}

static void addSampleToOutputArr(double sampleVoltage){
    lockMutex();
    {
    outputArray[endOfHistory].sampleInV = sampleVoltage;
    outputArray[endOfHistory].timestampInNanoS = getTimeInNs();
    endOfHistory++;
    }
    unlockMutex();
}

static void *sampling(void* args){
    //printf("you are here outside the while\n");
    
    while(true){
        //printf("you are here\n");
        //printf("thread bool is %d\n", threadIsStopped);
        
        if(threadIsStopped == false){
            
            totalSamples+=1;
            int lightReading = readingLightSampler();
            //printf("lightreading is %d\n", lightReading);
            
            double lightVoltage = ((double)lightReading/4095) * 1.8;
            
            addSampleToOutputArr(lightVoltage);
            
            sleepForMs(1);
        }
        else{
            return NULL;
        }
    
    }
}

samplerDatapoint_t *makeCopyOfOutput(){
    samplerDatapoint_t *outputCopy = EMPTY;
    outputCopy = malloc(sizeof(*outputArray) * endOfHistory);
    lockMutex();
    {
    for(int i = 0; i < endOfHistory; i++){
        memcpy(&outputCopy[i], &outputArray[i], sizeof(samplerDatapoint_t));
    }
    }
    unlockMutex();
    return outputCopy;
}

samplerDatapoint_t *Sampler_extractAllValues(int *length){
    
    samplerDatapoint_t *outputArrayCopy = makeCopyOfOutput();

    lockMutex();
    {
    
    int size = endOfHistory;
    *length = size;
    endOfHistory = 0;
    }
    unlockMutex();
    
    return outputArrayCopy;

}

void samplerMemoryStartup(){
    lockMutex();
    {
        
    outputArray = malloc(sizeof(*outputArray) * 4096*10);
    memset(outputArray, 0, 4096*10 * sizeof(*outputArray));
    
    }
    unlockMutex();
}

void Sampler_startSampling(void){
    
    samplerMemoryStartup();
    
    pthread_create(&thread, NULL, &sampling, NULL);
    
    
}


void samplerMemoryCleanup(){
    lockMutex();
    {
    memset(outputArray, EMPTY, sizeof(outputArray)*4096*10);
    endOfHistory = 0;
    }
    unlockMutex();
}

void Sampler_stopSampling(void){
    threadIsStopped = true;
    pthread_join(thread, NULL);
    samplerMemoryCleanup();

}

int Sampler_getNumSamplesInHistory(){
    int size = endOfHistory;
    return size;
}

long long Sampler_getNumSamplesTaken(void){
    return totalSamples;
}