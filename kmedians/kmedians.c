#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <float.h>

#include "kmedians.h"
#include "../utils/functions.h"
#include "../hashtable/hashnode.h"
#include "../hypercube/vertex.h"
#include "../hypercube/hypercube.h"
#include "../hashtable/htArray.h"

struct kmedians {
	int method; // 0: Exact LLoyd's; 1: Reverse Range Search with LSH; 2: Reverse Range Search with Hypercube
	int k; // Number of clusters

	int imageSize;
	imagePtr *centroids; // Centroids' array
	imageNodePtr *elements; // Elements' buckets with index the number of the centroid
};

kmediansPtr kmediansInit(int me, int numclusters, imageArrayPtr imArr) { // Initialize kmedians clusters; Return kmedians clusters; 
																		 // Method id as argument; Number of clusters as argument; Array of images as argument
	if ((numclusters < 1) || (me > 2) || (me < 0) || (imArr == NULL)) {

		return NULL;
	}

	kmediansPtr km;
	km = malloc(sizeof(struct kmedians)); // Malloc kmedians struct
	if (km == NULL) { // Check malloc failure

		return NULL;
	}

	km->method = me;
	km->k = numclusters;
	km->imageSize = imageArrayGetImageSize(imArr);

	km->centroids = malloc(km->k * sizeof(imagePtr)); // Malloc centroids' array
	if (km->centroids == NULL) { // Check malloc failure

		return NULL;
	}

	km->elements = malloc(km->k * sizeof(imageNodePtr)); // Malloc elements' buckets with index the number of the centroid
	if (km->elements == NULL) { // Check malloc failure

		return NULL;
	}

	for(int i = 0; i < km->k; i++) {
		km->centroids[i] = NULL;
		km->elements[i] = NULL;
	}

	int n = imageArrayGetNumOfImages(imArr);
	int imageSize = imageArrayGetImageSize(imArr);
	imagePtr *ims = imageArrayGetImages(imArr);

	// t = 0 
	int imInd = pseudoUniDistr(0, n);
	km->centroids[0] = imageNewCopy(imageGetNumber(ims[imInd]), ims[imInd], imageSize);

	int t = 1;
	while (t < km->k) {
		float *D = malloc(n * sizeof(float));
		float Dmax = INT_MIN;
		for (int i = 0; i < n; i++) {
			if (isCentroid(km, imageGetNumber(ims[i]))) {

				continue;
			}

			D[i] = FLT_MAX;
			for (int j = 0; j < t; j++) {
				float currDist = (float) imageManhatDist(ims[i], km->centroids[j], imageSize);
				if (currDist < D[i]) {
					D[i] = currDist;
					if (D[i] > Dmax) {

						Dmax = D[i];
					}
				}
			}
		}

		for (int i = 0; i < n; i++) {
			if (isCentroid(km, imageGetNumber(ims[i]))) {

				continue;
			}

			D[i] = D[i] / Dmax;			
		}

		float *P = malloc((n - t) * sizeof(float));
		int centrsMet = 0;
		for (int i = 0; i < n; i++) {
			if (isCentroid(km, imageGetNumber(ims[i]))) {
				centrsMet++;

				continue;
			}

			if ((i - centrsMet) > (n - t - 1)) {

				break;
			}

			if (i - centrsMet > 0) { // Rest partial sums

				P[i - centrsMet] = P[i - centrsMet - 1] + (D[i] * D[i]);
			}

			else { // First partial sum

				P[0] = (D[i] * D[i]);
			}
		}

		float x = pseudoUniDistrFloat(P[n - t - 1]);

		int r = INT_MAX;

		int l = 0;
		int h = (n - t - 1);
		int m = (h + l) / 2;
		while (l <= h) {
			m = (h + l) / 2;

			if (x < P[m]) {

				h = m - 1;
			} 

			else if (x > P[m]) {

				l = m + 1;
			} 
			else {
				r = m;

				break;
			}
		}

		if (r != m) {

			r = l;
		}
		
		int rconfirm = 0;
		int rr = 0;
		while (rconfirm < r) {
			if (!isCentroid(km, imageGetNumber(ims[rr]))) { // If its not centroid
			
				rconfirm++;
			}

			rr++;
		}
		km->centroids[t] = imageNewCopy(imageGetNumber(ims[rr]), ims[rr], imageSize);

		free(P);
		free(D);
		t++;
	}

	for (int i = 0; i < km->k; i++) {

		imageSetNumber(km->centroids[i], -i);
	}

	return km;
}

void kmediansFree(kmediansPtr km) { // Free kmedians clusters
	if (km == NULL) {

		return;
	}

	for (int i = 0; i < km->k; i++) {

		imageFree(km->centroids[i]);
	}

	for (int i = 0; i < km->k; i++) {

		while (km->elements[i] != NULL) { // While there are more elements in the bucket
			
			imageNodePtr temp = km->elements[i];
			km->elements[i] = imageNodeGetNext(km->elements[i]);	
		
			imageNodeFree(temp);
		}

		km->elements[i] = NULL;
	}

	free(km->elements);
	free(km->centroids);
	free(km);
}

int kmediansGetNumOfClusters(kmediansPtr km) { // Get number of clusters
	if (km == NULL) {

		return INT_MIN;
	}

	return km->k;	
}

imagePtr clusterGetCentroid(kmediansPtr km, int index) { // Get centroid of i-th cluster
	if (km == NULL) {

		return NULL;
	}

	return km->centroids[index];
}

imageNodePtr clusterGetElement(kmediansPtr km, int index) { // Get head element of i-th cluster
	if (km == NULL) {

		return NULL;
	}

	return km->elements[index];
}

int clusterCountElements(kmediansPtr km, int index) { // Get number of elements of i-th cluster
	if (km == NULL) {

		return INT_MIN;
	}

	int count = 0;
	imageNodePtr elem = km->elements[index];
	while (elem != NULL) {
		count++;

		elem = imageNodeGetNext(elem);
	}

	return count;
}

int countCentroids(void *v, int methodId) { // Count number of centroids in a bucket
	if ((v == NULL) || (methodId > 2) || (methodId < 1)) {

		return INT_MIN;
	}

	int count = 0;
	if (methodId == 1) {
		while (v != NULL) { // While there are more elements in the bucket
			imagePtr im = hashNodeGetImage(v); // Get image

			if (imageGetNumber(im) <= 0) {

				count++;
			}

			v = hashNodeGetNext(v);
		}
	}

	else { // methodId == 2
		while (v != NULL) { // While there are more elements in the bucket
			imagePtr im = vertexGetImage(v); // Get image

			if (imageGetNumber(im) <= 0) {

				count++;
			}

			v = vertexGetNext(v);
		}
	}

	return count;
}

int* getCentroidsIndicies(void *v, int methodId, int centrPerBucket) { // Get the indicies of all centroids in a bucket
	if ((v == NULL) || (methodId > 2) || (methodId < 1)) {

		return NULL;
	}

	int *centrIndices = malloc(centrPerBucket * sizeof(int));

	int i = 0;
	if (methodId == 1) {
		while (v != NULL) { // While there are more elements in the bucket
			imagePtr im = hashNodeGetImage(v); // Get image

			if (imageGetNumber(im) <= 0) {

				centrIndices[i] = -imageGetNumber(im);
				i++;
			}

			v = hashNodeGetNext(v);
		}
	}

	else { // methodId == 2
		while (v != NULL) { // While there are more elements in the bucket
			imagePtr im = vertexGetImage(v); // Get image

			if (imageGetNumber(im) <= 0) {

				centrIndices[i] = -imageGetNumber(im);
				i++;
			}

			v = vertexGetNext(v);
		}
	}

	return centrIndices;
}

void combineIndicies(int **inds1, int *num1, int **inds2, int *num2) { // Combine two array of indicies into one, free second array and return to first array
	if (*inds1 == NULL) {
		*inds1 = *inds2;
		*num1 = *num2;

		return;
	}

	if (*inds2 == NULL) {

		return;
	}

	if ((*inds1 != NULL) && (*inds2 != NULL)) {
		int newUnique = 0; // New unique numbers to add on first array from second array
		for (int i = 0; i < *num2; i++) { // For every number in second array
			int found = 0;
			for (int j = 0; j < *num1; j++) { // Check if number exists in first array
				if ((*inds1)[j] == (*inds2)[i]) {
					found = 1;

					break;
				}
			}
			
			if (found == 0) {

			    newUnique += 1;
			}
		}
		
		if (newUnique == 0) {
			free(*inds2);
			*inds2 = NULL;

			*num2 = 0;
		}

		else { // newUnique > 0
			int num3 = (*num1) + newUnique;
			int *inds3 = malloc(num3 * sizeof(int));
			
			for (int j = 0; j < *num1; j++) {

				inds3[j] = (*inds1)[j];
			}

			int i = 0; // For ints in second array
			int j = 0; // For ints in first array
			int uniqueAdded = 0; // Number of new unique numbers added

			for (int i = 0; i < *num2; i++) { // For every number in second array
				if (newUnique == uniqueAdded) { // If we added all unique numbers

					break;
				}

				int found = 0;
				for (int j = 0; j < *num1; j++) { // Check if number exists in first array
					if ((*inds1)[j] == (*inds2)[i]) {
						found = 1;

						break;
					}
				}
				
				if (found == 0) {
					inds3[*num1 + uniqueAdded] = (*inds2)[i];
					
					uniqueAdded += 1;
				}
			}

			free(*inds1); // Free old first array
			free(*inds2); // Free old second array
			*inds2 = NULL;
			*num2 = 0;
			
			// Set third array as first array
			*inds1 = inds3;
			*num1 = num3;
		}
	}
}

int isCentroid(kmediansPtr km, int num) { // Check if image is centroid or not based on id
	for (int i = 0; i < km->k; i++) {
		if (imageGetNumber(km->centroids[i]) == num) {

			return 1;
		}
	}

	return 0;
}

imagePtr lloydNearest(kmediansPtr km, imagePtr im) { // Get the centroid that image im belongs to
	int smallest = INT_MAX;
	int smallestInd = -1;

	for (int i = 0; i < km->k; i++) { // For all centroids
		int currDist = imageManhatDist(km->centroids[i], im, km->imageSize);

		if (currDist < smallest) {
			smallest = currDist;
			smallestInd = i;
		}
	}

	if (smallestInd == -1) {

		return NULL;
	}

	// else
	return km->centroids[smallestInd];
}

imagePtr lloydNearestIndices(kmediansPtr km, int centrPerBucket, int *centrIndices, imagePtr im, int imageSize) { // Get the closest centroid from a given amount of centroids
	if (imageGetNumber(im) <= 0) { // If image im is a centroid

		return NULL;
	}

	int smallest = INT_MAX;
	int smallestInd = -1;

	for (int i = 0; i < centrPerBucket; i++) { // For centroids that have a collision
		int currDist = imageManhatDist(km->centroids[centrIndices[i]], im, km->imageSize);

		if (currDist < smallest) {
			smallest = currDist;
			smallestInd = centrIndices[i];
		}
	}

	if (smallestInd == -1) {

		return NULL;
	}

	return km->centroids[smallestInd];
}

int lloydSecondNearestIndex(kmediansPtr km, imagePtr im) { // Get the index of the second closest centroid of an image 
	int smallest = INT_MAX;
	int secSmallest = INT_MAX;
	int smallestInd = -1;
	int secSmallestInd = -1;

	for (int i = 0; i < km->k; i++) { // For all centroids
		int currDist = imageManhatDist(km->centroids[i], im, km->imageSize);

		if (currDist < smallest) {
			secSmallest = smallest;
			smallest = currDist;

			secSmallestInd = smallestInd;
			smallestInd = i;
		}

		else if (currDist < secSmallest && currDist != smallest) {

			secSmallest = currDist;
			secSmallestInd = i;
		}
	}

	if (secSmallestInd == -1) {

		return INT_MIN;
	}

	if (smallestInd == -1) {

		return INT_MIN;
	}

	// else
	return secSmallestInd;
}

int classicLloyd(kmediansPtr km, imageArrayPtr imArr) { // Classic LLoyd's Algorithm
	float avgDisplacement = 0.0;
	while (1) {	
		for (int i = 0; i < imageArrayGetNumOfImages(imArr); i++) { // Assign every image to a cluster
			imagePtr im = imageArrayGetImage(imArr, i); // Get image

			imagePtr centr = lloydNearest(km, im); // Find nearest centroid

			imageNodePush(&(km->elements[-imageGetNumber(centr)]), im); // Assign image to cluster of found centroid
		}

		for (int j = 0; j < km->k; j++) {

			avgDisplacement += imageUpdateMedian(km->elements[j], &(km->centroids[j]), imageArrayGetImageSize(imArr));
		}

		avgDisplacement = avgDisplacement / km->k;

		if (avgDisplacement < 1) {

			break;
		}

		for (int i = 0; i < km->k; i++) {

			while (km->elements[i] != NULL) { // While there are more elements in the bucket
				
				imageNodePtr temp = km->elements[i];
				km->elements[i] = imageNodeGetNext(km->elements[i]);	
			
				imageNodeFree(temp);
			}

			km->elements[i] = NULL;
		}
	} 

	return 0;
}

int LSHReverseAssign(kmediansPtr km, imageArrayPtr imArr, int L, int d, int w, int kLSH) { // Reverse Assignment using LSH
	int numOfImages = imageArrayGetNumOfImages(imArr);
	htArrayPtr h = htArrayInit(L, numOfImages / 8);

	int numOfHts = htArrayGetNumOfHts(h);
	if ((numOfHts < 1) || (numOfImages < 1)) {

		return INT_MIN;
	}

	hashtablePtr *hashtables = htArrayGetHts(h);
	int numOfBuckets = hashtableGetNumOfBuckets(hashtables[0]);

	for (int i = 0; i < numOfImages; i++) {
		if (htArrayPush(h, imageArrayGetImage(imArr, i), d, w, kLSH) < 0) {

			return INT_MIN;
		}
	}

	float avgDisplacement = 0.0;
	int numOfIter = 0;
	while (1) {
		numOfIter++;

		for (int i = 0; i < km->k; i++) {
			if (htArrayPush(h, km->centroids[i], d, w, kLSH) < 0) {

				return INT_MIN;
			}
		}

		for (int i = 0; i < numOfBuckets; i++) {
			int totalCentrPerBucket = 0;
			int *totalCentrIndices = NULL;

			for (int j = 0; j < numOfHts; j++) { // For every hashtable, check the same bucket
				int centrPerBucket = countCentroids(hashtableGetBucket(hashtables[j], i), 1);
				if (centrPerBucket < 0) { // If bucket empty
				
					continue;
				}

				if (centrPerBucket > 0) {
					int *centrIndices = getCentroidsIndicies(hashtableGetBucket(hashtables[j], i), 1, centrPerBucket);

					combineIndicies(&totalCentrIndices, &totalCentrPerBucket, &centrIndices, &centrPerBucket);
				}
			}

			if (totalCentrPerBucket == 0) { // If there aren't any centroids in the same buckets of the hashtables
				for (int j = 0; j < numOfHts; j++) {  // For every hashtable, check the same bucket
					hashNodePtr curN = hashtableGetBucket(hashtables[j], i);
					while (curN != NULL) { // While there are more elements in the bucket
						
						imagePtr im = hashNodeGetImage(curN); // Get image

						imagePtr centr = lloydNearest(km, im); // Find nearest centroid

						imageNodePush(&(km->elements[-imageGetNumber(centr)]), im); // Assign image to cluster of found centroid
					
						curN = hashNodeGetNext(curN);	
					}
				}
			}

			else if (totalCentrPerBucket == 1) { // If there is only one centroid in the same buckets of the hashtables
				htArrayDeleteImage(h, -(totalCentrIndices[0])); // Delete centroid from LSH
				for (int j = 0; j < numOfHts; j++) { // For every hashtable, check the same bucket
					hashNodePtr curN = hashtableGetBucket(hashtables[j], i);

					while (curN != NULL) { // While there are more elements in the bucket
						imagePtr im = hashNodeGetImage(curN); // Get image

						imageNodePush(&(km->elements[totalCentrIndices[0]]), im); // // Assign image to cluster of found centroid
						
						curN = hashNodeGetNext(curN);
					}
				}

				free(totalCentrIndices);
			}

			else { // If there are more than two centroids in the same buckets of the hashtables
				for (int t = 0; t < totalCentrPerBucket; t++) {

					htArrayDeleteImage(h, -(totalCentrIndices[t])); // Delete centroid from LSH
				}

				for (int j = 0; j < numOfHts; j++) { // For every hashtable, check the same bucket
					hashNodePtr curN = hashtableGetBucket(hashtables[j], i);
					while (curN != NULL) { // While there are more elements in the bucket
						
						imagePtr im = hashNodeGetImage(curN); // Get image
						imagePtr centr = lloydNearestIndices(km, totalCentrPerBucket, totalCentrIndices, im, km->imageSize);

						imageNodePush(&(km->elements[-imageGetNumber(centr)]), im); // // Assign image to cluster of found centroid
						
						curN = hashNodeGetNext(curN);
					}
				}

				free(totalCentrIndices);
			}
		}

		avgDisplacement = avgDisplacement / km->k;

		for (int y = 0; y < km->k; y++) {

			avgDisplacement += imageUpdateMedian(km->elements[y], &(km->centroids[y]), km->imageSize);
		}

		if ((avgDisplacement < 1) || (numOfIter == 10)) {
			
			break;
		}

		for (int i = 0; i < km->k; i++) {

			while (km->elements[i] != NULL) { // While there are more elements in the bucket
				
				imageNodePtr temp = km->elements[i];
				km->elements[i] = imageNodeGetNext(km->elements[i]);	
			
				imageNodeFree(temp);
			}

			km->elements[i] = NULL;
		}	
	}

	htArrayFree(h);

	return 0;
}

int hypercubeReverseAssign(kmediansPtr km, imageArrayPtr imArr, int w, int k) { // Reverse Assignment using Hypercube
	hypercubePtr hc = hypercubeInit(k);

	for (int i = 0; i < imageArrayGetNumOfImages(imArr); i++) {

		if (hypercubePush(hc, imageArrayGetImage(imArr, i), km->imageSize, w, k) < 0) {
			
			return INT_MIN;
		}
	}

	float avgDisplacement = 0.0;
	int numOfIter = 0;
	while (1) {
		numOfIter++;

		for (int i = 0; i < km->k; i++) {

			if (hypercubePush(hc, km->centroids[i], km->imageSize, w, k) < 0) {

				return INT_MIN;
			}
		}

		for (int i = 0; i < hypercubeGetNumOfBuckets(hc); i++) {
			int centrPerBucket = countCentroids(hypercubeGetVertex(hc, i), 2);

			if (centrPerBucket < 0) { // If bucket empty
			
				continue;
			}

			else if (centrPerBucket == 0) { // If there aren't any centroids in the bucket 

				vertexPtr curV = hypercubeGetVertex(hc, i);
				while (curV != NULL) { // While there are more elements in the bucket
					
					imagePtr im = vertexGetImage(curV); // Get image

					imagePtr centr = lloydNearest(km, im); // Find nearest centroid

					imageNodePush(&(km->elements[-imageGetNumber(centr)]), im); // Assign image to cluster of found centroid
				
					curV = vertexGetNext(curV);	
				}
			}

			else if (centrPerBucket == 1) {
				int *centrIndices = getCentroidsIndicies(hypercubeGetVertex(hc, i), 2, centrPerBucket);

				hypercubeDeleteVertexElement(&hc, i, -centrIndices[0]); // Delete centroid from hypercube

				vertexPtr curV = hypercubeGetVertex(hc, i);
				while (curV != NULL) { // While there are more elements in the bucket
					imagePtr im = vertexGetImage(curV); // Get image

					imageNodePush(&(km->elements[centrIndices[0]]), im); // // Assign image to cluster of found centroid
					
					curV = vertexGetNext(curV);
				}

				free(centrIndices);
			}

			else {
				int *centrIndices = getCentroidsIndicies(hypercubeGetVertex(hc, i), 2, centrPerBucket);

				for (int j = 0; j < centrPerBucket; j++) {
					
					hypercubeDeleteVertexElement(&hc, i, -centrIndices[j]); // Delete centroids from hypercube
				}

				vertexPtr curV = hypercubeGetVertex(hc, i);
				while (curV != NULL) { // While there are more elements in the bucket
					
					imagePtr im = vertexGetImage(curV); // Get image
					imagePtr centr = lloydNearestIndices(km, centrPerBucket, centrIndices, im, km->imageSize);

					imageNodePush(&(km->elements[-imageGetNumber(centr)]), im); // // Assign image to cluster of found centroid
					
					curV = vertexGetNext(curV);
				}

				free(centrIndices);
			}
		}

		avgDisplacement = avgDisplacement / km->k;

		for (int y = 0; y < km->k; y++) {

			avgDisplacement += imageUpdateMedian(km->elements[y], &(km->centroids[y]), km->imageSize);
		}

		if ((avgDisplacement < 10) || (numOfIter == 10)) {
			
			break;
		}

		for (int i = 0; i < km->k; i++) {

			while (km->elements[i] != NULL) { // While there are more elements in the bucket
				
				imageNodePtr temp = km->elements[i];
				km->elements[i] = imageNodeGetNext(km->elements[i]);	
			
				imageNodeFree(temp);
			}

			km->elements[i] = NULL;
		}	
	}

	hypercubeFree(hc);

	return 0;
}

double* silhouette(kmediansPtr km) { // Silhouette metric

	long int sumA = 0, sumB = 0;
	long int countA = 0, countB = 0, clustCount = 0, totalCount = 0;
	double a, b, max;
	long double clustSum = 0, totalSum = 0;

	double *s = malloc((km->k + 1) * sizeof(double));

	// For each cluster
	for (int i = 0; i < km->k; i++) {
		clustSum = 0.0;
		clustCount = 0;

		imageNodePtr elem1 = km->elements[i];
		while (elem1 != NULL) {
			imagePtr im1 = imageNodeGetImage(elem1);
			clustCount++;

			sumA = 0;
			countA = 0;

			imageNodePtr elem2 = km->elements[i];
			while (elem2 != NULL) {
				imagePtr im2 = imageNodeGetImage(elem2);

				sumA += imageManhatDist(im1, im2, km->imageSize); // Calculate distance of im1 from all images in the same cluster
				countA++;

				elem2 = imageNodeGetNext(elem2);                
			}

			a = ((double) sumA / countA);

			sumB = 0;
			countB = 0;

			int secCentrInd = lloydSecondNearestIndex(km, im1); // Get index of second closest centroid of image im1

			imageNodePtr elemB = km->elements[secCentrInd];
			while (elemB != NULL) {
				imagePtr imB = imageNodeGetImage(elemB);

				sumB += imageManhatDist(im1, imB, km->imageSize); // Calculate distance of im1 from all images in its second closest centroid
				countB++;

				elemB = imageNodeGetNext(elemB);                
			}

			b = ((double) sumB / countB);

			if(a > b) {

				max = a;
			}

			else {

				max = b;
			}

			clustSum += ((b - a) / max);

			elem1 = imageNodeGetNext(elem1);
		}

		s[i] = clustSum / clustCount;

		totalSum += clustSum;
		totalCount += clustCount;
	}

	s[km->k] = totalSum / totalCount;

	return s;
}