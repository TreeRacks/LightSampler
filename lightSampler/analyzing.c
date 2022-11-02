#include "analyzing.h"
#include "sampling.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

//averageSample was -1
static double minSampleV, maxSampleV, averageSampleV = 0; 
static long long minInterval, maxInterval, averageInterval = 0;
static int amountOfDips = 0;
static long long lastTimeSample = 0;

static void settingMaxMinSampleV(samplerDatapoint_t sampleV[]){
    for(int i = 0; i < Sampler_getNumSamplesInHistory(); i++){
        double samplerVoltage = sampleV[i].sampleInV;
        if(i == FIRSTSAMPLE){
            maxSampleV = samplerVoltage;
            minSampleV = samplerVoltage;
        } else if (minSampleV > samplerVoltage){
            minSampleV = samplerVoltage;
        } else if(maxSampleV < samplerVoltage){
            maxSampleV = samplerVoltage;
        }
        
    }

}

static long long initializingLastSampleTime(samplerDatapoint_t sampleV[]){
    if(lastTimeSample == EMPTY){
        lastTimeSample = sampleV[0].timestampInNanoS;
    }
}

static void calculateAverageInterval(long long sampleTimeSum){
    if(Sampler_getNumSamplesInHistory() == EMPTY){
        averageInterval = 0;
    } else{
        averageInterval = (sampleTimeSum / Sampler_getNumSamplesInHistory());
    }
}

static void settingMaxMinInterval(samplerDatapoint_t sampleV[]){
    long long currentSampleTime, calculatedSampleTime, sampleTimeSum = 0;
    //not sure if needed
    if(Sampler_getNumSamplesInHistory() <= 0){
        exit(1);
    }
    initializingLastSampleTime(sampleV);
    for(int i = 0; i < Sampler_getNumSamplesInHistory(); i++){
        currentSampleTime = sampleV[i].timestampInNanoS;
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
