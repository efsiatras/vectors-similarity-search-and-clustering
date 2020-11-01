#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdint.h>

#include "./hashnode.h"
#include "../images/image.h"

typedef struct hashtable *hashtablePtr;

hashtablePtr hashtableInit(int); // Initialize hashtable 
										   // Return hashtable
										   // Number of buckets as argument

void hashtableFree(hashtablePtr); // Free hashtable

void hashtableDeleteImage(hashtablePtr, int); // Delete an image from hashtable based on its image number

int hashtablePush(hashtablePtr, imagePtr, int, int, int); // Add image to hashtable

int hashtableGetNumOfBuckets(hashtablePtr); // Get number of buckets
hashNodePtr hashtableGetBucket(hashtablePtr, int); // Get i-th bucket starting from 0

long int gFunc(imagePtr, int, int, int); // Hash function g
											 // Concatenation of results of h functions

#endif