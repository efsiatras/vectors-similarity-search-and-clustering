#ifndef HASHNODE_H
#define HASHNODE_H

#include <stdio.h>
#include <stdint.h>

#include "../images/image.h"

typedef struct hashNode *hashNodePtr;

hashNodePtr hashNodeInit(imagePtr); // Initialize hashNode 
								    // Return pointer to hashNode 
								    // Image as argument

void hashNodeFree(hashNodePtr); // Free hashNode

void hashNodeSetImage(hashNodePtr node, imagePtr image); // Set image
void hashNodeSetKey(hashNodePtr, int); // Set key
void hashNodeSetG(hashNodePtr, int); // Set g function
void hashNodeSetNext(hashNodePtr, hashNodePtr); // Set next

imagePtr hashNodeGetImage(hashNodePtr node); // Get image
int hashNodeGetKey(hashNodePtr); // Get key
int hashNodeGetG(hashNodePtr); // Get g function
hashNodePtr hashNodeGetNext(hashNodePtr); // Get next

#endif