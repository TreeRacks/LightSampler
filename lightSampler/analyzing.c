#include "analyzing.h"
#include "sampling.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

static double minSampleV, maxSampleV, averageSampleV = 0; 
static long long minInterval, maxInterval, averageInterval, lastTimeSample, numberOfSamplesInHistory = 0;
static int amountOfDips = 0;
static pthread_t thread;
static pthread_mutex_t analyzerMutex = PTHREAD_MUTEX_INITIALIZER;
static bool threadIsStopped = false;


void lockMutex(){
    pthread_mutex_lock(&analyzerMutex);
}

void unlockMutex(){
    pthread_mutex_unlock(&analyzerMutex);
}

static int getDipsInSample(samplerDatapoint_t sampleValue[]){
    bool valueHasDippedBelowAvg = false;
    int dips = 0;
    for(int i = 0; i < numberOfSamplesInHistory; i++){
        double difference = (averageSampleV - sampleValue[i].sampleInV); //difference from sample voltage to average. 
        if(valueHasDippedBelowAvg == true){
            if(difference < 0.1 - 0.03){
                valueHasDippedBelowAvg = false;
            }
        }
        else {
            if(difference > 0.1){
                valueHasDippedBelowAvg = true;
                dips++;
            }
        }
    }
    return dips;

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
    for(int i = 0; i < numberOfSamplesInHistory; i++){
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

void initializingLastSampleTime(samplerDatapoint_t sampleValue[]){
    if(lastTimeSample == EMPTY){
       lastTimeSample = sampleValue[0].timestampInNanoS;
    }
}

static void calculateAverageInterval(long long sampleTimeSum){
    if(numberOfSamplesInHistory == EMPTY){
        averageInterval = 0;
    } else{
        averageInterval = (sampleTimeSum / numberOfSamplesInHistory);
    }
}

static void settingMaxMinInterval(samplerDatapoint_t sampleValue[]){
    long long currentSampleTime, calculatedSampleTime, sampleTimeSum = 0;
    initializingLastSampleTime(sampleValue);
    for(int i = 0; i < numberOfSamplesInHistory; i++){
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

static void *analyzing(void* args){
    long long previousNumberOfSample = 0;
    while(true){
        if(threadIsStopped == false){
        
        sleepForMs(1000);

        samplerDatapoint_t* sampleValue = Sampler_extractAllValues(&numberOfSamplesInHistory);
        long long currentNumberOfSample = 0; 

        lockMutex();
        {
            currentNumberOfSample = Sampler_getNumSamplesTaken();
            amountOfDips = getDipsInSample(sampleValue);
            settingMaxMinSampleV(sampleValue);
            settingMaxMinInterval(sampleValue);
            printf("Interval ms (%.3f, %.3f) avg=%.3f", minInterval/1000000.0, maxInterval/1000000.0, averageInterval/1000000.0);
            printf("    Samples V (%.3f, %.3f) avg=%.3f", minSampleV, maxSampleV, averageSampleV);
            printf("    # Dips: %d", amountOfDips);
            printf("    # Samples: %lld \n", currentNumberOfSample - previousNumberOfSample);
            previousNumberOfSample = currentNumberOfSample;
        }
        unlockMutex();
        free(sampleValue);
        }
        else{
            return NULL;
        }
    }
}

void startAnalyzing(){

    pthread_create(&thread, NULL, &analyzing, NULL);
}

void stopAnalyzing(){
    threadIsStopped = true;
    pthread_join(thread,NULL);
}

long long getMinInterval(){
    long long minSampleInterval = 0;
    lockMutex();
    {
        minSampleInterval = minInterval;
    }
    unlockMutex();
    return minSampleInterval;
}

long long getMaxInterval(){
    long long maxSampleInterval = 0;
    lockMutex();
    {
        maxSampleInterval = maxInterval;
    }
    unlockMutex();
    return maxSampleInterval;
}


double getMinValue(){
    double minSampleValue = 0;
    lockMutex();
    {
        minSampleValue = minSampleV;
    }
    unlockMutex();
    return minSampleValue;
}

double getMaxValue(){
    double maxSampleValue = 0;
    lockMutex();
    {
        maxSampleValue = maxSampleV;
    }
    unlockMutex();
    return maxSampleValue;
}

long long getNumberOfDips(){
    long long dips = 0;
    lockMutex();
    {
        dips = amountOfDips;
    }
    unlockMutex();
    return dips;
}
