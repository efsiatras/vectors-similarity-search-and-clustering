#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "vertex.h"
#include "../images/image.h"

struct vertex {
	imagePtr im;
	int key;
	char* id;
	struct vertex *next;
};

vertexPtr vertexInit(imagePtr i, int k) { // Initialize vertex ; Return pointer to vertex ; Image as argument; Bucket length as argument
	vertexPtr v;
	v = malloc(sizeof(struct vertex)); // Malloc vertex struct
	if (v == NULL) { // Check malloc failure

		return NULL;
	}

	v->im = i;
	v->key = -1;

	v->id = (char*) malloc((k+1) * sizeof(char)); // Initialize empty id
	memset(v->id, ' ', k);
	v->id[k] = '\0';

	v->next = NULL;

	return v;
}

void vertexFree(vertexPtr v) { // Free vertex
	if (v == NULL) {

		return;
	}
	
	free(v->id);
	free(v);
}

void vertexSetImage(vertexPtr v, imagePtr i) { // Set image
	if (v == NULL) {

		return;
	}

	v->im = i;
}

void vertexSetKey(vertexPtr v, int k) { // Set key
	if (v == NULL) {

		return;
	}

	v->key = k;
}

void vertexSetId(vertexPtr v, char* id) { // Set id
	if (v == NULL) {

		return;
	}

	strcpy(v->id, id);
}

void vertexSetNext(vertexPtr v, vertexPtr nex) { // Set next
	if (v == NULL) {

		return;
	}

	v->next = nex;
}

imagePtr vertexGetImage(vertexPtr v) { // Get image
	if (v == NULL) {

		return NULL;
	}

	return v->im;
}

int vertexGetKey(vertexPtr v) { // Get key
	if (v == NULL) {

		return INT_MIN;
	}

	return v->key;
}

char* vertexGetId(vertexPtr v) { // Get id
	if (v == NULL) {

		return NULL;
	}

	return v->id;
}

vertexPtr vertexGetNext(vertexPtr v) { // Get next
	if (v == NULL) {

		return NULL;
	}

	return v->next;
}

void vertexPrint(vertexPtr v) { // Print a bucket

	vertexPtr curV = v; // Get first object in bucket 
	while (curV != NULL) {

		printf("v->key %d\n", v->key);
		printf("v->id %s\n\n", v->id);
		
		curV = curV->next;
	}
}