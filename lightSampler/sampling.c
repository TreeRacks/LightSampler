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

bool threadIsStopped = false;
long long totalSamples = 0;
void *defaultArg = NULL;
int endOfHistory = 0;
static samplerDatapoint_t* outputArray = NULL;
static pthread_t thread;
static pthread_mutex_t historyMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(historyMutex, NULL);

int readingLightSampler(char* buffer, int length){
    
    // Open file
    int file = open(A2D_FILE, 0);
    int bytes = read(file, buffer, length-1);
    close(file);
    buffer[bytes] = 0;
    return bytes;
}
//   FILE *f = fopen(A2D_FILE, "r");
//   if (!f) {
//     printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
//     printf(" Check /boot/uEnv.txt for correct options.\n");
//     exit(-1);
//   }
  // Get reading
int readLightAsInt(){
    int buffLen = 1024;
    char buff[buffLen];
    readingLightSampler(buff, buffLen);
    return atoi(buff);

}
//   int a2dReading = 0;
//   int itemsRead = fscanf(f, "%d", &a2dReading);
//   if (itemsRead <= 0) {
//     printf("ERROR: Unable to read values from voltage input file.\n");
//     exit(-1);
//   }
//   // Close file
//   fclose(f);
//   return a2dReading;

// }

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
            int lightReading = readLightAsInt();
            //printf("lightreading is %d\n", lightReading);
            
            double lightVoltage = ((double)lightReading/4095) * 1.8;
            
            addSampleToOutputArr(lightVoltage);
            
            sleepForMs(1);
        }
        else{
            //printf("you are here\n");
            return NULL;
        }
    
    }
}

samplerDatapoint_t *Sampler_extractAllValues(int *length){
    
    samplerDatapoint_t *outputArrayCopy = 0;
    lockMutex();
    {
        
    outputArrayCopy = malloc(sizeof(*outputArray) * endOfHistory);
    for(int i = 0; i < endOfHistory; i++){
        memcpy(&outputArrayCopy[i], &outputArray[i], sizeof(samplerDatapoint_t));
    }
    int size = endOfHistory;
    *length = size;
    endOfHistory = 0;
    }
    unlockMutex();
    
    return outputArrayCopy;

}

void Sampler_startSampling(void){
    
    lockMutex();
    {
        
    outputArray = malloc(sizeof(*outputArray) * 4096*10);
    memset(outputArray, 0, 4096*10 * sizeof(*outputArray));
    
    }
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
    {
    memset(outputArray, EMPTY, sizeof(outputArray)*4096*10);
    endOfHistory = 0;
    }
    unlockMutex();

}

int Sampler_getNumSamplesInHistory(){
    int size = endOfHistory;
    return size;
}

long long Sampler_getNumSamplesTaken(void){
    return totalSamples;
}