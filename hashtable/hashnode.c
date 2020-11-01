#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hashnode.h"

struct hashNode {
	imagePtr im;
	int key;
	int g;
	struct hashNode *next;
};

hashNodePtr hashNodeInit(imagePtr i) { // Initialize hashNode ; Return pointer to hashNode ; Image as argument
	hashNodePtr node;
	node = malloc(sizeof(struct hashNode)); // Malloc hashNode struct
	if (node == NULL) { // Check malloc failure

		return NULL;
	}

	node->im = i;
	node->key = -1; // Hashtable will calculate this later
	node->g = -1;	// Hashtable will calculate this later
	node->next = NULL;

	return node;
}

void hashNodeFree(hashNodePtr node) { // Free hashNode
	if (node == NULL) {

		return;
	}

	free(node);
}

void hashNodeSetImage(hashNodePtr node, imagePtr image) { // Set image
	if (node == NULL) {

		return;
	}

	node->im = image;
}

void hashNodeSetKey(hashNodePtr node, int k) { // Set key
	if (node == NULL) {

		return;
	}

	node->key = k;
}

void hashNodeSetG(hashNodePtr node, int g) { // Set g function
	if (node == NULL) {

		return;
	}

	node->g = g;
}

void hashNodeSetNext(hashNodePtr node, hashNodePtr nex) { // Set next
	if (node == NULL) {

		return;
	}

	node->next = nex;
}

imagePtr hashNodeGetImage(hashNodePtr node) { // Get image
	if (node == NULL) {

		return NULL;
	}

	return node->im;
}

int hashNodeGetKey(hashNodePtr node) { // Get key
	if (node == NULL) {

		return INT_MIN;
	}

	return node->key;
}

int hashNodeGetG(hashNodePtr node) { // Get g function
	if (node == NULL) {

		return INT_MIN;
	}

	return node->g;
}

hashNodePtr hashNodeGetNext(hashNodePtr node){ // Get next
	if (node == NULL) {

		return NULL;
	}

	return node->next;
}