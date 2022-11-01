// sampler.h
// Module to sample light levels in the background (thread).
// It provides access to the raw samples and then deletes them.
#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#define A2D_FILE "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"

typedef struct {
 double sampleInV;
 long long timestampInNanoS;
} samplerDatapoint_t;
int readingLightSampler();
// Begin/end the background thread which samples light levels.
void Sampler_startSampling(void);
void Sampler_stopSampling(void);
// Get a copy of the samples in the sample history, removing values
// from our history here.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: function provides both data and size to ensure consistency.
samplerDatapoint_t* Sampler_extractAllValues(int *length);
// Returns how many valid samples are currently in the history.
int Sampler_getNumSamplesInHistory();
// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void);
#endif