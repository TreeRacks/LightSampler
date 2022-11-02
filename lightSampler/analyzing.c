#include "analyzing.h"
#include "sampling.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

static double minSampleV, maxSampleV, averageSampleV = 0; 
static long long minInterval, maxInterval, averageInterval = 0;
static int amountOfDips = 0;
static long long lastTimeSample = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void lockMutex(){
    pthread_mutex_lock(&mutex);
}

void unlockMutex(){
    pthread_mutex_unlock(&mutex);
}

static void *analyzing(void* args){
    long long previousNumberOfSample = 0;
    while(true){
        samplerDatapoint_t* sampleValue = Sampler_extractAllValues();
        lockMutex();
        {
            long long currentNumberOfSample = Sampler_getNumSamplesTaken();
            settingMaxMinSampleV(sampleValue);
            settingMaxMinInterval(sampleValue);
            printf("Interval ms (%f, %f) avg=%f", minInterval/1000000, maxInterval/1000000, averageInterval/1000000);
            printf("    Samples V (%f, %f) avg=%f", minSampleV, maxSampleV, averageSampleV);
            //printf("    # Dips: %d", numberOfDips);
            printf("    # Samples: %lld", currentNumberOfSample - previousNumberOfSample);
            previousNumberOfSample = currentNumberOfSample;
        }
        unlockMutex();
        free(sampleValue);
        sleepForMs(1000);
    }
    return NULL;
}

static void calculateAverageExpV(samplerDatapoint_t sampleVaue[], int i){
    double currentAverageV = 0;
    currentAverageV = sampleVaue[i].sampleInV;
    if(averageSampleV == 0){
        averageSampleV = currentAverageV;
    } else{
        averageSampleV =  averageSampleV* WEIGHT + (1-WEIGHT)*currentAverageV;
    }
}

static void settingMaxMinSampleV(samplerDatapoint_t sampleVaue[]){
    for(int i = 0; i < Sampler_getNumSamplesInHistory(); i++){
        long long sumSamplerV
        double samplerVoltage = sampleVaue[i].sampleInV;
        if(i == FIRSTSAMPLE){
            maxSampleV = samplerVoltage;
            minSampleV = samplerVoltage;
        } else if (minSampleV > samplerVoltage){
            minSampleV = samplerVoltage;
        } else if(maxSampleV < samplerVoltage){
            maxSampleV = samplerVoltage;
        }
        calculateAverageExpV(sampleV, i);
    }
}

static long long initializingLastSampleTime(samplerDatapoint_t sampleVaue[]){
    if(lastTimeSample == EMPTY){
        lastTimeSample = sampleVaue[0].timestampInNanoS;
    }
}

static void calculateAverageInterval(long long sampleTimeSum){
    if(Sampler_getNumSamplesInHistory() == EMPTY){
        averageInterval = 0;
    } else{
        averageInterval = (sampleTimeSum / Sampler_getNumSamplesInHistory());
    }
}

static void settingMaxMinInterval(samplerDatapoint_t sampleVaue[]){
    long long currentSampleTime, calculatedSampleTime, sampleTimeSum = 0;
    //not sure if needed
    // if(Sampler_getNumSamplesInHistory() <= 0){
    //     exit(1);
    // }
    initializingLastSampleTime(sampleVaue);
    for(int i = 0; i < Sampler_getNumSamplesInHistory(); i++){
        currentSampleTime = sampleVaue[i].timestampInNanoS;
        calculatedSampleTime = currentSampleTime - lastTimeSample;
        sampleTimeSum = sampleTimeSum + calculatedSampleTime;

        if(i == FIRSTSAMPLE){
            minInterval = calculatedSampleTime;
            maxInterval = calculatedSampleTime;
        } else if(minInterval > calculatedSampleTime){
            minInterval = calculatedSampleTime;
        } else if(maxInterval < calculatedSampleTime){
            minInterval = calculatedSampleTime;
        }
        lastTimeSample = currentSampleTime;
    }
    calculateAverageInterval(sampleTimeSum);
}

