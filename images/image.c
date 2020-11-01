#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "image.h"

struct image {
	int number;
	uint8_t *pixels;
};

imagePtr imageInit(int num, uint8_t *pi) { // Initialize image ; Return pointer to image ; Pixels as arguments
	imagePtr im;
	im = malloc(sizeof(struct image)); // Malloc image struct
	if (im == NULL) { // Check malloc failure

		return NULL;
	}

	im->number = num;

	im->pixels = pi;
	if (im->pixels == NULL) { // If malloc and copy failed
		imageFree(im); // Free what we have already malloc'ed

		return NULL;
	}

	return im;
}

void imageFree(imagePtr im) { // Free image
	if (im == NULL) {

		return;
	}

	if (im->pixels != NULL) {
		free(im->pixels);

		im->pixels = NULL;
	}

	free(im);
}

imagePtr imageNewCopy(int num, imagePtr im, int imageSize) { // Create new copy of image
	if (im == NULL) {

		return NULL;
	}
	
	uint8_t *newPixels = malloc(imageSize * sizeof(uint8_t));

	for (int i = 0; i < imageSize; i++) {

		newPixels[i] = im->pixels[i];
	}

	imagePtr newIm = imageInit(num, newPixels);
	if (newIm == NULL) {

		return NULL;
	}

	return newIm;
}

int imageGetNumber(imagePtr im) { // Get image number
	if (im == NULL) {

		return INT_MIN;
	}

	return im->number;
}

void imageSetNumber(imagePtr im, int num) { // Set image number
	if (im == NULL) {

		return;
	}

	im->number = num;
}

uint8_t *imageGetPixels(imagePtr im) { // Get pixels 
	if (im == NULL) {

		return NULL;
	}

	return im->pixels;
}

void imagePrint(imagePtr im, int32_t imageSize) {	// Print image
	int i;	

	printf("\n*** image %u ***\n\n", imageGetNumber(im));

	for (i = 0; i < imageSize; i++) {		// Loop over bytes

	    printf("%d ", im->pixels[i]);
	}
	
	printf("\n\n\n");
}

int imageManhatDist(imagePtr im1, imagePtr im2, int d) { // Return Manhattan Distance of two images
	if ((im1 == NULL) || (im2 == NULL)) {

		return INT_MIN;
	}

	unsigned int result = 0;
	for (int i = 0; i < d; i++) {
		int dist = abs(im1->pixels[i] - im2->pixels[i]);

		result += dist;
	}

	return result;
}