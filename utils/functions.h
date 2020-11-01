#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "../images/image.h"

typedef struct neighbors *neighborsPtr;

int pseudoUniDistr(int, int); // Pseudo-uniform distribution

float pseudoUniDistrFloat(float); // Pseudo-uniform distribution for floaters from 0 to rangeH

int modExp(int, int, int); // Modular exponentiation

long int concatenate(long int, long int); // Concatenation of two numbers

unsigned int mod(int, int); // Calculate the modulo of two numbers

int hFunc(imagePtr, int, int, int); // Function h 	

void swap(int*, int*, imagePtr*, imagePtr*); // Swap two values in both distances and images arrays

void sortNeighbors(imagePtr **, int **, int); // Sort nearest neighbors from closest to furthest

#endif