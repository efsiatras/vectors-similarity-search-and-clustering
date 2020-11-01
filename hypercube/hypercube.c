#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "vertex.h"
#include "hypercube.h"
#include "fArray.h"
#include "../images/imageArray.h"
#include "../utils/functions.h"

struct hypercube {
	int numOfBuckets;
	fArrayPtr fa;
	vertexPtr *table; // Table as array of buckets
};

hypercubePtr hypercubeInit(int k) { // Initialize hypercube ; Return hypercube ; Number of dimensions as argument
	hypercubePtr hc;
	hc = malloc(sizeof(struct hypercube)); // Malloc hypercube struct
	if (hc == NULL) { // Check malloc failure

		return NULL;
	}

	hc->numOfBuckets = 1 << k; // 2^k buckets
	
	int M = 1 << (32 / k);
	hc->fa = fArrayInit(M, k); // Initialize table to store f functions' values

	hc->table = malloc(hc->numOfBuckets * sizeof(vertexPtr)); // Malloc table array
	if (hc->table == NULL) { // Check malloc failure

		return NULL;
	}

	for (int i = 0; i < hc->numOfBuckets; i++) {

		hc->table[i] = NULL;
	}

	return hc;
}

int hypercubePush(hypercubePtr hc, imagePtr im, int d, int w, int k) { // Add image to hypercube
	static int maxKey = 0; // Number of non-empty buckets
	int key;

	vertexPtr new = vertexInit(im, k); // Malloc new vertex
	if (new == NULL) { // Check malloc failure

		return INT_MIN;
	}

	char* id = fFuncs(hc, im, d, w, k); // Construct the target bucket id

	vertexSetId(new, id); // Set id of new object

	if (hypercubeGetVertexKeyViaId(hc, id, maxKey) == -1) { // If bucket with this id doesn't exist
		key = maxKey; // Remember the position of the previous empty bucket and return the position of the current empty bucket
		vertexSetKey(new, key); // Set object's bucket position
		hc->table[key] = new;	// Initialize bucket to store the object 
		maxKey++;
	}

	else { // If bucket with this id already exists
		key = hypercubeGetVertexKeyViaId(hc, id, maxKey); // Get the position of the bucket with this id
		vertexSetKey(new, key); // Set new object's bucket position
		
		// Push at the begining of the bucket
		vertexPtr head = hc->table[key]; 
		vertexSetNext(new, head);
		hc->table[key] = new;
	}

	free(id);

	return key;
}

int hypercubeGetNumOfBuckets(hypercubePtr hc) { // Get number of buckets
	if (hc == NULL) {

		return INT_MIN;
	}

	return hc->numOfBuckets;
}

int hypercubeGetVertexKeyViaId(hypercubePtr hc, char* id, int maxKey) { // Get bucket's key via id
	if (hc == NULL) {

		return -1;
	}

	for (int i = 0; i < maxKey; i++) {

		if(hc->table[i] == NULL) {

			continue;
		}

		if (strcmp(vertexGetId(hc->table[i]), id) == 0) {
			
			return i;
		}
	}

	return -1;
}

vertexPtr hypercubeGetVertex(hypercubePtr hc, int i) { // Get i-th vertex starting from 0
	if (hc == NULL) {

		return NULL;
	}

	if (i < 0) { // If index is invalid

		return NULL;
	}

	return hc->table[i];
}

void hypercubeDeleteVertexElement(hypercubePtr *hc, int key, int number) { // Delete an element in a vertex based on an it's image number and vertex key
	if (hc == NULL) {

		return;
	}

	if((*hc)->table[key] == NULL) {

		return; 
	}

	int imNumber = -999999;
	vertexPtr temp;
	vertexPtr prev;
	
	temp = (*hc)->table[key];
	prev = temp; 

	imagePtr im = vertexGetImage(temp);
	
	if(im != NULL) {

		imNumber = imageGetNumber(im);
	}

	if (temp != NULL && imNumber == number) { // If first element in vertex is the one to be deleted
	 
		(*hc)->table[key] = vertexGetNext(temp);  

		vertexFree(temp);               
		return; 
	} 
   
	while (temp != NULL && imNumber != number) { // If element to be deleted is not the first one, keep track of the previous element
	 
		prev = temp; 
		temp = vertexGetNext(temp);

		imagePtr im = vertexGetImage(temp);
	
		if(im != NULL) {

			imNumber = imageGetNumber(im);
		}
	} 
	
	vertexPtr prevNext = vertexGetNext(prev);
	prevNext = vertexGetNext(temp); 
	vertexFree(temp); 
}

void hypercubeFree(hypercubePtr hc) { // Free hypercube
	if (hc == NULL) {

		return;
	}

	for (int i = 0; i < hc->numOfBuckets; i++) {
		
		while (hc->table[i] != NULL) { // While there are more elements in the bucket
			
			vertexPtr curV = hc->table[i];
			hc->table[i] = vertexGetNext(hc->table[i]);	
		
			vertexFree(curV);
		}

		hc->table[i] = NULL;
	}

	fArrayFree(hc->fa);
	free(hc->table);
	free(hc);
}

int hypercubePrint(hypercubePtr hc) { // Print hypercube
	unsigned int count = 0; 

	for (int i = 0; i < hc->numOfBuckets; i++) {
		
		printf("+++ BUCKET %d+++\n", i);

		vertexPtr curV = hc->table[i];
		while (curV != NULL) {
			count++;
			vertexPrint(curV);

			curV = vertexGetNext(curV);	
		}
	}

	return count;
}

char* fFuncs(hypercubePtr hc, imagePtr im, int d, int w, int k) { // Functions f ; "Flips a coin for each h" ; Concatenation of bits of total flips
	int h, value;
	char strval[64];
	char *stres;

	stres = (char*) malloc((k+1) * sizeof(char));	//allocate memory for bucket's id

	for (int i = 0; i < k; i++) {

		h = hFunc(im, d, w, k);	// Get h function

		if (fArrayGetValue(hc->fa, h, i) == -1) {	// If function fi(h) is produced for the first time

			value = pseudoUniDistr(0, 1);	// "Flip a coin" to give fi(h) a value (0 or 1)
			fArrayPush(hc->fa, h, i, value); // Push fi(h) to fArray
		}

		else { // if function fi(h) has been produced before

			value = fArrayGetValue(hc->fa, h, i); // Get fi(h) value from fArray
		}
		
		snprintf(strval, sizeof(strval), "%d", value); // Convert integer to string

		if (i == 0) { // Store first digit of bucket's id

			strcpy(stres, strval);
		}

		else { // Construct bucket's id digit by digit

			stres = strcat(stres, strval);
		} 
	}

	return stres;
}

int hammingDist(char* id1, char* id2) { // Calculate the hamming distance between 2 strings
	int i = 0, count = 0;
	
	if (id1 == NULL || id2 == NULL) {

		return INT_MIN;
	}

	while (id1[i] != '\0') { // Traverse each character

		if (id1[i] != id2[i]) { // If characters differ at each strings, raise counter

			count++;
		}

		i++;
	}

	return count;
}

int hypercubeApprKNN(hypercubePtr hc, imagePtr q, int imageSize, int knearest, int w, int k, int M, int probes, imagePtr **candImages, int **candDist) { // Approximate k-nearest neighbors
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

	char* id = fFuncs(hc, q, imageSize, w, k); // Construct the target bucket id
	int key = hypercubeGetVertexKeyViaId(hc, id, hc->numOfBuckets); // Get the position of the bucket with this id

	int checkedM = 0; // Number of neighbors checked

	// At first we check for neighbors in our bucket
	vertexPtr v = hypercubeGetVertex(hc, key); // Get first record of vertex if it exists
	while (v != NULL) { // While there are more elements in the vertex
		imagePtr im = vertexGetImage(v); // Get image

		int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
		checkedM++; // A neighbor was checked

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

		if (checkedM == M) { // If wanted amount of neighbors is checked break
			
			sortNeighbors(candImages, candDist, knearest);
			free(id);

			return 0;
		}

		v = vertexGetNext(v);
	}

	// If there are still neighbors to be checked, check the nearest buckets based on Hamming Distance
	if (checkedM < M && probes > 0) {
		int i = 0; // Bucket at position 0
		int checkedProbes = 0; // Number of probes checked
		int hamDist = 1; // Search for buckets in hamming distance = 1

		while (i < hc->numOfBuckets) {

			if (hammingDist(id, vertexGetId(hc->table[i])) == hamDist) { // If verticies' ids differ by hamDist digits
				checkedProbes++; // A bucket was checked
				int key = i; // Current bucket position

				vertexPtr v = hypercubeGetVertex(hc, key); // Get first record of vertex if it exists
				while (v != NULL) { // While there are more elements in the vertex
					imagePtr im = vertexGetImage(v); // Get image

					int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
					checkedM++; // A neighbor was checked

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

					if (checkedM == M) { // If wanted amount of neighbors is checked break
						
						sortNeighbors(candImages, candDist, knearest);
						free(id);

						return 0;
					}

					v = vertexGetNext(v);
				}

				if (checkedProbes == probes) {

					sortNeighbors(candImages, candDist, knearest);
					free(id);

					return 0;
				}

				if (i == hc->numOfBuckets - 1) { // If all buckets in distance = hamDist were checked 

					hamDist++; // Check for buckets in distance = hamDist + 1
					i = -1;	// Reinitialize bucket counter
				}
			}

			i++;
		}
	}
	sortNeighbors(candImages, candDist, knearest);
	free(id);

	return 0;
}

int hypercubeExactKNN(imageArrayPtr imArr, imagePtr q, int imageSize, int knearest, imagePtr **candImages, int **candDist) { // Exact k-nearest neihbors with linear search
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

int hypercubeApprRSearch(hypercubePtr hc, imagePtr q, int imageSize, double R, int w, int k,  int M, int probes, imageNodePtr *candImages) { // Approximate Range Search with Hypercube ;
	*candImages = NULL;																																		// Return list of images
	
	char* id = fFuncs(hc, q, imageSize, w, k); // Construct the target bucket id
	int key = hypercubeGetVertexKeyViaId(hc, id, hc->numOfBuckets); // Get the position of the bucket with this id
	
	int checkedM = 0; // Number of neighbors checked

	// At first we check for neighbors in our bucket
	vertexPtr v = hypercubeGetVertex(hc, key); // Get first record of vertex if it exists
	while (v != NULL) { // While there are more elements in the vertex
		imagePtr im = vertexGetImage(v); // Get image

		int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
		checkedM++; // A neighbor was checked

		if (currDist < R) { // If we found distance smaller than R
			
			imageNodePtr temp = *candImages;

			*candImages = imageNodeInit(im);
			imageNodeSetNext(*candImages, temp);
		}
		
		if (checkedM == M) { // If wanted amount of neighbors is checked break
			free(id);

			return 0;
		}

		v = vertexGetNext(v);
	}

	// If there are still neighbors to be checked, check the nearest buckets based on Hamming Distance
	if (checkedM < M && probes > 0) {
		int i = 0; // Bucket at position 0
		int checkedProbes = 0; // Number of probes checked
		int hamDist = 1; // Search for buckets in hamming distance = 1

		while (i < hc->numOfBuckets) {

			if (hammingDist(id, vertexGetId(hc->table[i])) == hamDist) { // If verticies' ids differ by hamDist digits
				checkedProbes++; // A bucket was checked
				int key = i; // Current bucket position

				vertexPtr v = hypercubeGetVertex(hc, key); // Get first record of vertex if it exists
				while (v != NULL) { // While there are more elements in the vertex
					imagePtr im = vertexGetImage(v); // Get image

					int currDist = imageManhatDist(im, q, imageSize); // Calculate distance
					checkedM++; // A neighbor was checked
					
					if (currDist < R) { // If we found distance smaller than R
			
						imageNodePtr temp = *candImages;

						*candImages = imageNodeInit(im);
						imageNodeSetNext(*candImages, temp);
					}

					if (checkedM == M) { // If wanted amount of neighbors is checked break
						free(id);

						return 0;
					}

					v = vertexGetNext(v);
				}

				if (checkedProbes == probes) {
					free(id);

					return 0;
				}

				if (i == hc->numOfBuckets - 1) { // If all buckets in distance = hamDist were checked 

					hamDist++; // Check for buckets in distance = hamDist + 1
					i = -1;	// Reinitialize bucket counter
				}
			}

			i++;
		}
	}

	free(id);

	return 0;

}