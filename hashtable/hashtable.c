#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hashtable.h"
#include "../images/image.h"
#include "../images/imageArray.h"
#include "../hashtable/htArray.h"
#include "../utils/functions.h"

struct hashtable {
	int numOfBuckets;
	hashNodePtr *table; // Table as array of buckets
};

hashtablePtr hashtableInit(int n) { // Initialize hashtable ; Return hashtable ; Number of buckets as argument
	hashtablePtr h;
	h = malloc(sizeof(struct hashtable)); // Malloc hashtable struct
	if (h == NULL) { // Check malloc failure

		return NULL;
	}

	h->numOfBuckets = n;

	h->table = malloc(h->numOfBuckets * sizeof(hashNodePtr)); // Malloc table array
	if (h->table == NULL) { // Check malloc failure

		return NULL;
	}

	for (int i = 0; i < h->numOfBuckets; i++) {

		h->table[i] = NULL;
	}

	return h;
}

void hashtableFree(hashtablePtr h) { // Free hashtable
	if (h == NULL) {

		return;
	}

	for (int i = 0; i < h->numOfBuckets; i++) {
		while (h->table[i] != NULL) { // While there are more elements in the bucket
			hashNodePtr temp = h->table[i];
			h->table[i] = hashNodeGetNext(h->table[i]);

			hashNodeFree(temp);
		}

		h->table[i] = NULL;
	}
	free(h->table);

	free(h);
}

void hashtableDeleteImage(hashtablePtr h, int number) { // Delete an image from hashtable based on its image number
	if ((h == NULL) || (h->numOfBuckets < 1)) {

		return;
	}

	for (int i = 0; i < h->numOfBuckets; i++) {
		hashNodePtr curr = h->table[i]; // Current hashnode
		if (imageGetNumber(hashNodeGetImage(curr)) == number) { // If we found the image on the first element of bucket
			hashNodePtr next = hashNodeGetNext(curr); // Next element
			h->table[i] = next;
			hashNodeFree(curr);
			return;
		}

		// If there are more elements in the bucket
		hashNodePtr prev = h->table[i]; // Previous hashnode
		curr = hashNodeGetNext(prev); // Current hashnode
		while (curr != NULL) { // While there are more elements in the bucket
			if (imageGetNumber(hashNodeGetImage(curr)) == number) { // If we found the image
				hashNodePtr next = hashNodeGetNext(curr); // Next element
				hashNodeSetNext(prev, next); // Skip current element
				hashNodeFree(curr); // Free current element
				return;
			}

			curr = hashNodeGetNext(curr);
		}
	}
}

int hashtablePush(hashtablePtr h, imagePtr im, int d, int w, int k) { // Add image to hashtable
	hashNodePtr new = hashNodeInit(im); // Malloc new hashNode
	if (new == NULL) { // Check malloc failure

		return INT_MIN;
	}

	long int g = gFunc(im, d, w, k);
	hashNodeSetG(new, g);

	int key = g % (h->numOfBuckets);
	hashNodeSetKey(new, key);

	if (h->table[key] == NULL) { // If List is empty

		h->table[key] = new;
	}

	else { // If list is not empty, insert at first
		hashNodePtr head = h->table[key];
		hashNodeSetNext(new, head);
		h->table[key] = new;
	}

	return key;
}

int hashtableGetNumOfBuckets(hashtablePtr h) { // Get number of buckets
	if (h == NULL) {

		return INT_MIN;
	}

	return h->numOfBuckets;
}

hashNodePtr hashtableGetBucket(hashtablePtr h, int i) { // Get i-th bucket starting from 0
	if (h == NULL) {

		return NULL;
	}

	return h->table[i];
}

long int gFunc(imagePtr im, int d, int w, int k) { // Hash function g ; Concatenation of results of h functions
	if (k <= 0) {

		return 0;
	}

	long int result = hFunc(im, d, w, k); // Calculate h_1(p)

	for (int i = 1; i < k; i++) { // Concatenate with the rest of h_k(p)
		int h = hFunc(im, d, w, k);

		result = concatenate(result, h);
	}

	return result;
}

