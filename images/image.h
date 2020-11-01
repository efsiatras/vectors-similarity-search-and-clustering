#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>
#include <stdint.h>

typedef struct image *imagePtr;

imagePtr imageInit(int, uint8_t *); // Initialize image 
								    // Return pointer to image 
								    // Pixels as arguments

void imageFree(imagePtr); // Free image

imagePtr imageNewCopy(int, imagePtr, int); // Create new copy of image

int imageGetNumber(imagePtr); // Get image number

void imageSetNumber(imagePtr, int); // Set image number

uint8_t *imageGetPixels(imagePtr); // Get pixels

void imagePrint(imagePtr, int32_t);	// Print image

int imageManhatDist(imagePtr, imagePtr, int); // Return Manhattan Distance of two images

#endif