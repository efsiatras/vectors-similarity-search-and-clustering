#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "imageArray.h"
#include "image.h"

struct imageArray {
	int32_t numOfImages;
	int32_t imageSize;
	imagePtr *images;
};

imageArrayPtr imageArrayInit(int32_t numOfImages, int32_t imageSize) { // Initialize array of images 
	imageArrayPtr imArr;
	int i;

    imArr = (imageArrayPtr) malloc(sizeof(struct imageArray));		// Allocate memory for array of images
    
    if(imArr == NULL) { // Check malloc failure

		return NULL;
	}

	// Initialize struct's records
    imArr->numOfImages = numOfImages;
    imArr->imageSize = imageSize;
    imArr->images = (imagePtr*) malloc(numOfImages * imageSize);	

	for(i = 0; i < imArr->numOfImages; i++) {

		imArr->images[i] = NULL; // Initialize image array with NULL values
	}

	return imArr;
}

void imageArrayFree(imageArrayPtr imArr) { // Free array of images
	if (imArr == NULL) {

		return;
	}

	for (int i = 0; i < imArr->numOfImages; i++) {
		imageFree(imArr->images[i]);
		imArr->images[i] = NULL;
	}

	free(imArr->images);
	imArr->images = NULL;

	free(imArr);
}

void imageArrayPrint(imageArrayPtr imArr) {	// Print array of images
	for(int i = 0; i < imArr->numOfImages; i++) {	// Traverse through images

		imagePrint(imArr->images[i], imArr->imageSize);
	}
}

int imageArrayPush(imageArrayPtr imArr, imagePtr im, int i) { // Push image to i-th index
	if(imArr == NULL) {

		return INT_MIN;
	}

	imArr->images[i] = im;

	return 0;
}

int32_t imageArrayGetNumOfImages(imageArrayPtr imArr) { // Return number of images 
	if(imArr == NULL) {

		return INT_MIN;
	}

	return imArr->numOfImages;
}

int32_t imageArrayGetImageSize(imageArrayPtr imArr) { // Return size of image 
	if(imArr == NULL) {

		return INT_MIN;
	}

	return imArr->imageSize;
}

imagePtr imageArrayGetImage(imageArrayPtr imArr, int i) { // Return i-th image
	if(imArr == NULL) {

		return NULL;
	}

	return imArr->images[i];
}

imagePtr *imageArrayGetImages(imageArrayPtr imArr) { // Return array of images
	if(imArr == NULL) {

		return NULL;
	}

	return imArr->images;
}