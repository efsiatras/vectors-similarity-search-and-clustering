#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <stdio.h>
#include <stdint.h>

#include "vertex.h"
#include "../images/image.h"
#include "../images/imageArray.h"
#include "../images/imageNode.h"

typedef struct hypercube *hypercubePtr;

hypercubePtr hypercubeInit(int); // Initialize hypercube 
									// Return hypercube 
									// Number of dimensions as argument
	
int hypercubePush(hypercubePtr, imagePtr, int, int, int); // Add image to hypercube

int hypercubeGetVertexKeyViaId(hypercubePtr, char*, int); // Get bucket's key via id

int hypercubeGetNumOfBuckets(hypercubePtr); // Get number of buckets
vertexPtr hypercubeGetVertex(hypercubePtr, int); // Get i-th vertex starting from 0

void hypercubeDeleteVertexElement(hypercubePtr *, int, int); // Delete an element in a vertex based on an it's image number and vertex key

void hypercubeFree(hypercubePtr); // Free hypercube

int hypercubePrint(hypercubePtr); // Print hypercube

char* fFuncs(hypercubePtr, imagePtr, int, int, int); // Functions f 

int hammingDist(char*, char*); // Calculate the hamming distance between 2 strings

int hypercubeApprKNN(hypercubePtr, imagePtr, int, int, int, int, int, int, imagePtr **, int **); // Approximate k-nearest neighbors

int hypercubeExactKNN(imageArrayPtr, imagePtr, int, int, imagePtr **, int **); // Exact k-nearest neihbors with linear search

int hypercubeApprRSearch(hypercubePtr, imagePtr, int, double, int, int,  int, int, imageNodePtr *); // Approximate Range Search with Hypercube ; Return list of images

#endif