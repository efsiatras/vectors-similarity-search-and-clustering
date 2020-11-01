#include <stdlib.h>
#include <string.h>

#include "fArray.h"

struct fArray {
	int h;
	int k;
	int** values;
};

fArrayPtr fArrayInit(int h, int k) { // Initialize fArray ; Return fArray 
	fArrayPtr fa;
	fa = malloc(sizeof(struct fArray)); // Malloc fArray struct
	if (fa == NULL) { // Check malloc failure

		return NULL;
	}

	fa->h = h;
	fa->k = k;

	fa->values = (int **) malloc(fa->h * sizeof(int *)); // Malloc array
	if (fa->values == NULL) { // Check malloc failure
		
		return NULL;
	}

	for (int i = 0; i < fa->h; i++) {

		fa->values[i] = (int *) malloc(fa->k * sizeof(fa->values[0]));
	
		if (fa->values[i] == NULL) { // Check malloc failure

			return NULL;
		}
	}

	for (int i = 0; i < fa->h; i++) {

		for(int j = 0; j < fa->k; j++) {

			fa->values[i][j] = -1;
		}
	}

	return fa;
}

void fArrayPush(fArrayPtr fa, int h, int k, int value) { // Add f value to fArray
	
	fa->values[h][k] = value;
}

int fArrayGetValue(fArrayPtr fa, int h, int k) { // Check if f value is in fArray

	int value = fa->values[h][k];

	return (value != -1 ? value : -1); 
}

void fArrayFree(fArrayPtr fa) { // Free fArray
	for (int i = 0; i < fa->h; i++) {
		
		free(fa->values[i]);
	}

	free(fa->values);
	free(fa);
}
