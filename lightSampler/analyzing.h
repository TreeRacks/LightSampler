#ifndef _ANALYZING_H_
#define _ANALYZING_H_

#define EMPTY 0
#define FIRSTSAMPLE 0
#define WEIGHT 0.999

// static void settingMaxMinSampleV(samplerDatapoint_t sampleValue[]);
// static void settingMaxMinInterval(samplerDatapoint_t sampleValue[]);
// static void calculateAverageInterval(long long sampleTimeSum);
// static long long initializingLastSampleTime(samplerDatapoint_t sampleV[]);
// static void *analyzing(void* args);
double getMaxValue();
double getMinValue();
long long getMaxInterval();
long long getMinInterval();
long long getNumberOfDips();
//static int  getDipsInSample(samplerDatapoint_t sampleValue[], int size, double avg);
void startAnalyzing(void);
void stopAnalyzing(void);

#endif