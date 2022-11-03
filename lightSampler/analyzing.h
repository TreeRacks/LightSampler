#ifndef _ANALYZING_H_
#define _ANALYZING_H_

#define EMPTY 0
#define FIRSTSAMPLE 0
#define WEIGHT 0.999

double getMaxValue();
double getMinValue();
long long getMaxInterval();
long long getMinInterval();
long long getNumberOfDips();
void startAnalyzing(void);
void stopAnalyzing(void);

#endif