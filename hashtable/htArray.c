#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "htArray.h"
#include "../utils/functions.h"

struct htArray {
	int numOfHts; // Number of hashtables
	hashtablePtr *hts; // Array of hashtables
};

htArrayPtr htArrayInit(int numHts, int numBckts) { // Initialize array of hashtables
	htArrayPtr h;
	h = malloc(sizeof(struct htArray)); // Malloc htArray struct
	if (h == NULL) { // Check malloc failure

		return NULL;
	}

	h->numOfHts = numHts;

	h->hts = malloc(h->numOfHts * sizeof(hashtablePtr)); // Malloc hashtables' array
	if (h->hts == NULL) { // Check malloc failure

		return NULL;
	}

	for (int i = 0; i < h->numOfHts; i++) {
		h->hts[i] = hashtableInit(numBckts);
		if (h->hts[i] == NULL) {

			return NULL;
		}
	}

	return h;
}

void htArrayFree(htArrayPtr h) { // Free array of hashtables
	if (h == NULL) {

		return;
	}

	for (int i = 0; i < h->numOfHts; i++) {
		hashtableFree(h->hts[i]);
		h->hts[i] = NULL;
	}
	free(h->hts);

	free(h);
}

int htArrayPush(htArrayPtr h, imagePtr im, int d, int w, int k) { // Add image to array of hashtables
	if (h == NULL) {

		return INT_MIN;
	}

	for (int i = 0; i < h->numOfHts; i++) {
		if (hashtablePush(h->hts[i], im, d, w, k) < 0) {

			return INT_MIN;
		}
	}
}

void htArrayDeleteImage(htArrayPtr h, int number) { // Delete image from array of hashtables based on its image number
	if (h == NULL) {

		return;
	}

	for (int i = 0; i < h->numOfHts; i++) {

		hashtableDeleteImage(h->hts[i], number);
	}
}

hashtablePtr *htArrayGetHts(htArrayPtr h) { // Return array of hashtables
	if (h == NULL) {

		return NULL;
	}

	return h->hts;
}

int htArrayGetNumOfHts(htArrayPtr h) { // Return number of hashtables
	if (h == NULL) {

		return INT_MIN;
	}

	return h->numOfHts;
}

int apprKNN(htArrayPtr h, imagePtr q, int imageSize, int knearest, int w, int k, imagePtr **candImages, int **candDist) { // Approximate k-nearest neighbors with LSH ; Return array of images
	hashtablePtr *hts = htArrayGetHts(h);
	int numOfHashtables = htArrayGetNumOfHts(h);

	long int g;
	int key;

	*candImages = malloc(knearest * sizeof(imagePtr));
	if (*candImages == NULL) { // Check malloc failure

		return INT_MIN;
	}

	*candDist = malloc(knearest * sizeof(int)); // Keep k-best candidate distances
	if (*candDist == NULL) { // Check malloc failure

		return INT_MIN;
	}

	for (int i = 0; i < knearest; i++) { // Initialize max distances
		(*candImages)[i] = NULL;
		(*candDist)[i] = INT_MAX;
	}

	int worstDist = INT_MAX; // Keep the distance of the worst candidate 
	int worstCand = 0; // Keep the index of the worst candidate

	int numCheckedItems = 0; // Number of checked items
	for (int i = 0; i < numOfHashtables; i++) {
		g = gFunc(q, imageSize, w, k);
		key = g % (hashtableGetNumOfBuckets(hts[0]));

		hashNodePtr node = hashtableGetBucket(hts[i], key); // Get first node of bucket if it exists
		while (node != NULL) { // While there are more elements in the bucket
			numCheckedItems++;

			imagePtr im = hashNodeGetImage(node); // Get image
			int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
			if (currDist < worstDist) { // If we found better candidate than the worst candidate
				(*candImages)[worstCand] = im;
				(*candDist)[worstCand] = currDist;

				worstDist = -1;
				for (int z = 0; z < knearest; z++) { // Update the current worst candidate
					if ((*candDist)[z] > worstDist) {
						worstDist = (*candDist)[z];
						worstCand = z;
					}
				}
			}

			if (numCheckedItems > 1000 * numOfHashtables) { // If large number of items are checked

				sortNeighbors(candImages, candDist, knearest);

				return 0;
			}

			node = hashNodeGetNext(node);
		}
	}

	sortNeighbors(candImages, candDist, knearest);

	return 0;
}

// int exactKNN(htArrayPtr h, imagePtr q, int imageSize, int knearest, imagePtr **candImages, int **candDist) { // Exact k-nearest neihbors with linear search ; Return array of images
// 	*candImages = malloc(knearest * sizeof(imagePtr));
// 	if (*candImages == NULL) { // Check malloc failure

// 		return INT_MIN;
// 	}

// 	*candDist = malloc(knearest * sizeof(int)); // Keep k-best candidate distances
// 	if (*candDist == NULL) { // Check malloc failure

// 		return INT_MIN;
// 	}

// 	for (int i = 0; i < knearest; i++) { // Initialize max distances

// 		*candDist[i] = INT_MAX;
// 	}

// 	int worstDist = INT_MAX; // Keep the distance of the worst candidate 
// 	int worstCand = 0; // Keep the index of the worst candidate

// 	int numOfBuckets = hashtableGetNumOfBuckets(h->hts[0]);
// 	for (int i = 0; i < numOfBuckets; i++) {
// 		hashNodePtr node = hashtableGetBucket(h->hts[0], i); // Get first node of bucket if it exists
// 		while (node != NULL) { // While there are more elements in the bucket
// 			imagePtr im = hashNodeGetImage(node); // Get image
// 			int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
// 			if (currDist < worstDist) { // If we found better candidate than the worst candidate
// 				//numRetrItems++;
// 				*candImages[worstCand] = im;
// 				*candDist[worstCand] = currDist;

// 				worstDist = -1;
// 				for (int z = 0; z < knearest; z++) { // Update the current worst candidate
// 					if (*candDist[z] > worstDist) {
// 						worstDist = *candDist[z];
// 						worstCand = z;
// 					}
// 				}
// 			}



// 			node = hashNodeGetNext(node);
// 		}
// 	}

// 	return 0;
// }

int exactKNN(imageArrayPtr imArr, imagePtr q, int imageSize, int knearest, imagePtr **candImages, int **candDist) { // Exact k-nearest neihbors with linear search ; Return array of images
	*candImages = malloc(knearest * sizeof(imagePtr));
	if (*candImages == NULL) { // Check malloc failure

		return INT_MIN;
	}

	*candDist = malloc(knearest * sizeof(int)); // Keep k-best candidate distances
	if (*candDist == NULL) { // Check malloc failure

		return INT_MIN;
	}

	for (int i = 0; i < knearest; i++) { // Initialize max distances
		(*candImages)[i] = NULL;
		(*candDist)[i] = INT_MAX;
	}

	int worstDist = INT_MAX; // Keep the distance of the worst candidate 
	int worstCand = 0; // Keep the index of the worst candidate

	int numOfImages = imageArrayGetNumOfImages(imArr);
	for (int i = 0; i < numOfImages; i++) {
		imagePtr im = imageArrayGetImage(imArr, i);
		int currDist = imageManhatDist(im, q, imageSize); // Calculate distance

		if (currDist < worstDist) { // If we found better candidate than the worst candidate
			(*candImages)[worstCand] = im;
			(*candDist)[worstCand] = currDist;

			worstDist = -1;
			for (int z = 0; z < knearest; z++) { // Update the current worst candidate
				if ((*candDist)[z] > worstDist) {
					worstDist = (*candDist)[z];
					worstCand = z;
				}
			}
		}
	}

	sortNeighbors(candImages, candDist, knearest);

	return 0;
}

int apprRSearch(htArrayPtr h, imagePtr q, int imageSize, double R, int w, int k, imageNodePtr *candImages) { // Approximate Range Search with LSH ; Return list of images
	hashtablePtr *hts = htArrayGetHts(h);
	int numOfHashtables = htArrayGetNumOfHts(h);

	long int g;
	int key;

	*candImages = NULL;
	for (int i = 0; i < numOfHashtables; i++) {
		g = gFunc(q, imageSize, w, k);
		key = g % (hashtableGetNumOfBuckets(hts[0]));

		hashNodePtr node = hashtableGetBucket(hts[i], key); // Get first node of bucket if it exists
		while (node != NULL) { // While there are more elements in the bucket
			imagePtr im = hashNodeGetImage(node); // Get image
			int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
			if (currDist < R) { // If we found distance smaller than R
				if (imageNodeListExists(*candImages, im) == 0) { // If image does not exist in list
					imageNodePtr temp = *candImages;

					*candImages = imageNodeInit(im);
					imageNodeSetNext(*candImages, temp);
				}
			}

			node = hashNodeGetNext(node);
		}
	}

	return 0;
}