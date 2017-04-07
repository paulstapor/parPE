#ifndef CPP_MISC_H
#define CPP_MISC_H

#include <stdlib.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#include <stdbool.h>
#endif

//void printMatlabArray(const double *buffer, int len);

void createDirectoryIfNotExists(char *dirName);

void strFormatCurrentLocaltime(char *buffer, size_t bufferSize, const char *format);

void shuffle(int *array, size_t numElements);

void runInParallelAndWaitForFinish(void *(*function)(void *), void **args, int numArgs);

EXTERNC void printBacktrace(int depth);

double randDouble(double min, double max);

/**
 * @brief fillArrayRandomDoubleIndividualInterval Fill "buffer" with "length" random double values, drawn from an interval [min, max] given for each value.
 * @param min
 * @param max
 * @param length
 * @param buffer
 */
void fillArrayRandomDoubleIndividualInterval(const double *min, const double *max, int length, double *buffer);

void fillArrayRandomDoubleSameInterval(double min, double max, int length, double *buffer);
#endif
