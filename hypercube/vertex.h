#ifndef VERTEX_H
#define VERTEX_H

#include <stdio.h>
#include <stdint.h>

#include "../images/image.h"

typedef struct vertex *vertexPtr;

vertexPtr vertexInit(imagePtr, int); // Initialize vertex 
								    // Return pointer to vertex 
								    // Image as argument

void vertexFree(vertexPtr); // Free vertex

void vertexSetImage(vertexPtr, imagePtr); // Set image
void vertexSetKey(vertexPtr, int); // Set key
void vertexSetId(vertexPtr, char*); // Set id
void vertexSetNext(vertexPtr, vertexPtr); // Set next

imagePtr vertexGetImage(vertexPtr); // Get image
int vertexGetKey(vertexPtr); // Get key
char* vertexGetId(vertexPtr); // Get id
vertexPtr vertexGetNext(vertexPtr); // Get next

void vertexPrint(vertexPtr); // Print a bucket

#endif