#ifndef FARRAY_H
#define FARRAY_H

#include <stdio.h>
#include <stdint.h>

typedef struct fArray *fArrayPtr;

fArrayPtr fArrayInit(int, int); // Initialize fArray 
									//Return fArray 

void fArrayPush(fArrayPtr, int, int, int); // Add f value to fArray

int fArrayGetValue(fArrayPtr, int, int); // Check if f value is in fArray

void fArrayFree(fArrayPtr); // Free fArray

#endif