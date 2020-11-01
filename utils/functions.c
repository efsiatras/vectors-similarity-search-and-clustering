#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>

#include "functions.h"

int pseudoUniDistr(int rangeL, int rangeH) { // Pseudo-uniform distribution 
	double randNum = rand()/(1.0 + RAND_MAX); 
	int range = rangeH - rangeL + 1;

	int scaledRandNum = (randNum * range) + rangeL;

	return scaledRandNum;
}

float pseudoUniDistrFloat(float rangeH) { // Pseudo-uniform distribution for floaters from 0 to rangeH

	return ((float) rand() / (float)(RAND_MAX)) * rangeH;
}

int modExp(int a, int b, int m) {
	int result = 1;
	if (1 & b)
		result = a;
	while (b) {
		b >>= 1;
		a = (a * a) % m;
		if (b & 1)
			result = (result * a) % m;
	}
	return result;
}

long int concatenate(long int a, long int b) { // Concatenation of two numbers
	long int p = 10;
	while (b >= p) {
		p *= 10;
	}

	return a * p + b;
}

unsigned int mod(int a, int m) { // Calculate the modulo of two numbers
	if (a % m < 0) {

		return (unsigned int) a % m + m;
	}

	else {

		return (unsigned int) a % m;
	}

}

int hFunc(imagePtr im, int d, int w, int k) { // Function h ; Return for hash function g
	int M = 1 << (32 / k);

	int *a;
	a = malloc(d * sizeof(int)); // Malloc a array
	if (a == NULL) { // Check malloc failure

		return -1;
	}

	uint8_t *pixels = imageGetPixels(im);

	int maxA = INT_MIN; // Store max a
	for (int i = 0; i < d; i++) {

		int numerator = pixels[i] - pseudoUniDistr(0, w - 1);
		a[i] = numerator / w;
		if (numerator < 0) {
			a[i]--;
		}

		if (maxA < a[i]) {

			maxA = a[i];
		}
	}

	int m = (maxA + M/2) / 2; // max a < m < M/2

	int result = 0;
	for (int i = 0; i < d; i++) {
		int subRes1 = modExp(m, d - i - 1, M);
		int subRes2 = mod(a[i], M);

		result += mod((subRes1 * subRes2), M);
	}
	free(a);

	return mod(result, M);
}

void swap(int* candDist1, int* candDist2, imagePtr* candIm1, imagePtr* candIm2) { // Swap two values in both distances and images arrays
    int tempDist = *candDist1;
    imagePtr tempIm = *candIm1;
    
    *candDist1 = *candDist2;
    *candIm1 = *candIm2;

    *candDist2 = tempDist; 
    *candIm2 = tempIm; 
} 
  
void sortNeighbors(imagePtr **candImages, int **candDist, int knearest) { // Sort nearest neighbors from closest to furthest
	int minDist;

	for (int i = 0; i < knearest - 1; i++) { 

		minDist = i; // Current distance is the minimum
		for (int j = i + 1; j < knearest; j++) {

			if ((*candDist)[j] < (*candDist)[minDist]) { // If current value is smaller than the minimum values, set the new index of minDist
                minDist = j; 
			}
		}

		swap(&(*candDist)[minDist], &(*candDist)[i], &(*candImages)[minDist], &(*candImages)[i]); 
	}
	
	return;
}
