#ifndef HTARRAY_H
#define HTARRAY_H

#include <stdio.h>
#include <stdint.h>

#include "hashtable.h"
#include "../images/imageArray.h"
#include "../images/imageNode.h"

typedef struct htArray *htArrayPtr;

htArrayPtr htArrayInit(int, int); // Initialize array of hashtables

void htArrayFree(htArrayPtr); // Free array of hashtables

int htArrayPush(htArrayPtr, imagePtr, int, int, int); // Add image to array of hashtables

void htArrayDeleteImage(htArrayPtr, int); // Delete image from array of hashtables based on its image number

hashtablePtr *htArrayGetHts(htArrayPtr); // Return array of hashtables
int htArrayGetNumOfHts(htArrayPtr); // Return number of hashtables

int apprKNN(htArrayPtr, imagePtr, int, int, int, int, imagePtr **, int **); // Approximate k-nearest neighbors 
																			// Return array of images

// int exactKNN(htArrayPtr, imagePtr, int, int, imagePtr **, int **); // Exact k-nearest neihbors with linear search
// 																	  // Return array of images

int exactKNN(imageArrayPtr, imagePtr, int, int, imagePtr **, int **); // Exact k-nearest neihbors with linear search
																	  // Return array of images

int apprRSearch(htArrayPtr, imagePtr, int, double, int, int, imageNodePtr *); // Approximate Range Search with LSH
																		   // Return list of images

#endif