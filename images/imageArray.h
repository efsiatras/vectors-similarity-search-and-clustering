#ifndef IMAGEARRAY_H
#define IMAGEARRAY_H

#include <stdio.h>
#include <stdint.h>

#include "image.h"

typedef struct imageArray *imageArrayPtr;


imageArrayPtr imageArrayInit(int32_t , int32_t); // Initialize array of images 

void imageArrayFree(imageArrayPtr);	// Free array of images

void imageArrayPrint(imageArrayPtr);	// Print array of images

int imageArrayPush(imageArrayPtr, imagePtr, int); // Push image to i-th index
int32_t imageArrayGetNumOfImages(imageArrayPtr); // Return number of images 
int32_t imageArrayGetImageSize(imageArrayPtr); // Return size of image 
imagePtr imageArrayGetImage(imageArrayPtr, int); // Return i-th image
imagePtr *imageArrayGetImages(imageArrayPtr); // Return array of images

#endif