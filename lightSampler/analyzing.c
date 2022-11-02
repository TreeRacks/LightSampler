#include "analyzing.h"
#include "sampling.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

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

            amountOfDips = getDips();
            settingMaxMinSampleV(sampleValue);
            settingMaxMinInterval(sampleValue);
            printf("Interval ms (%.3f, %.3f) avg=%.3f", minInterval/1000000.0, maxInterval/1000000.0, averageInterval/1000000.0);
            printf("    Samples V (%f, %f) avg=%f", minSampleV, maxSampleV, averageSampleV);
            printf("    # Dips: %d", amountOfDips);
            printf("    # Samples: %lld", currentNumberOfSample - previousNumberOfSample);
            previousNumberOfSample = currentNumberOfSample;
        }
        unlockMutex();
        free(sampleValue);
        sleepForMs(1000);
    }
    return NULL;
}

static int getDips(samplerDatapoint_t sampleValue[], int size, double avg){

}


static void calculateAverageExpV(samplerDatapoint_t sampleValue[], int i){
    double currentAverageV = 0;
    currentAverageV = sampleValue[i].sampleInV;
    if(averageSampleV == 0){
        averageSampleV = currentAverageV;
    } else{
        averageSampleV =  averageSampleV* WEIGHT + (1-WEIGHT)*currentAverageV;
    }

}

static void settingMaxMinSampleV(samplerDatapoint_t sampleValue[]){
    for(int i = 0; i < Sampler_getNumSamplesInHistory(); i++){
        double samplerVoltage = sampleValue[i].sampleInV;
        if(i == FIRSTSAMPLE){
            maxSampleV = samplerVoltage;
            minSampleV = samplerVoltage;
        } else if (minSampleV > samplerVoltage){
            minSampleV = samplerVoltage;
        } else if(maxSampleV < samplerVoltage){
            maxSampleV = samplerVoltage;
        }
        calculateAverageExpV(sampleValue, i);
    }
}

static long long initializingLastSampleTime(samplerDatapoint_t sampleValue[]){
    if(lastTimeSample == EMPTY){
        lastTimeSample = sampleValue[0].timestampInNanoS;
    }
    return 0;
}

static void calculateAverageInterval(long long sampleTimeSum){
    if(Sampler_getNumSamplesInHistory() == EMPTY){
        averageInterval = 0;
    } else{
        averageInterval = (sampleTimeSum / Sampler_getNumSamplesInHistory());
    }
}

static void settingMaxMinInterval(samplerDatapoint_t sampleValue[]){
    long long currentSampleTime, calculatedSampleTime, sampleTimeSum = 0;
    //not sure if needed
    // if(Sampler_getNumSamplesInHistory() <= 0){
    //     exit(1);
    // }
    initializingLastSampleTime(sampleValue);
    for(int i = 0; i < Sampler_getNumSamplesInHistory(); i++){
        currentSampleTime = sampleValue[i].timestampInNanoS;
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

void getMinInterval(){
    lock()
    {
        long long minSampleInterval = minInterval;
    }
    unlock();
    return minSampleInterval;
}

void getMaxInterval(){
    lock()
    {
        long long maxSampleInterval = maxInterval;
    }
    unlock();
    return maxSampleInterval;
}


void getMinValue(){
    lock()
    {
        long long minSampleValue = minSampleV;
    }
    unlock();
    return minSampleValue;
}

void getMaxValue(){
    lock()
    {
        long long maxSampleValue = maxSampleV;
    }
    unlock();
    return maxSampleValue;
}

void getNumberOfDips(){
    lock()
    {
        long long dips = amountOfDips;
    }
    unlock();
    return dips;
}
