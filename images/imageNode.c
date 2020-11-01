#include <stdlib.h>
#include <string.h>

#include "imageNode.h"

struct imageNode {
	imagePtr im;
	struct imageNode *next;
};

imageNodePtr imageNodeInit(imagePtr i) { // Initialize imageNode ; Return pointer to imageNode ; Image as argument
	imageNodePtr node;
	node = malloc(sizeof(struct imageNode)); // Malloc hashNode struct
	if (node == NULL) { // Check malloc failure

		return NULL;
	}
	node->im = i;
	node->next = NULL;

	return node;
}

void imageNodePush(imageNodePtr *head, imagePtr im) { // Add image to the beginning of imageNode
	imageNodePtr new = imageNodeInit(im); // Malloc new imageNode
	if (new == NULL) { // Check malloc failure

		return;
	}

	new->next = *head;
	*head = new;
}

void imageNodeFree(imageNodePtr node) { // Free imageNode
	if (node == NULL) {

		return;
	}

	free(node);
}

int imageNodeListExists(imageNodePtr head, imagePtr image) { // Check if image exists in list of imageNodes
	while (head != NULL) { // Traverse list
		if (imageGetNumber(head->im) == imageGetNumber(image)) { // If we found same image number

			return 1;
		}

		head = head->next;
	}

	return 0;
}

void imageNodeSetNext(imageNodePtr node, imageNodePtr nex) { // Set next
	if (node == NULL) {

		return;
	}

	node->next = nex;
}

imagePtr imageNodeGetImage(imageNodePtr node) { // Get image
	if (node == NULL) {

		return NULL;
	}

	return node->im;
}

imageNodePtr imageNodeGetNext(imageNodePtr node) { // Get next
	if (node == NULL) {

		return NULL;
	}

	return node->next;
}

float imageUpdateMedian(imageNodePtr elems, imagePtr *centroid, int imageSize) { // Update centroid based on median
	if ((elems == NULL) || (centroid == NULL) || (imageSize == 0)) {

		return -1.0;
	}

	float avgDisplacement = 0.0; // Average displacement of pixels to determine if centroid has changed significantlly
	int numOfElems = 0; // Number of images in cluster of centroid

	imageNodePtr temp = elems;
	while (temp != NULL) {
		numOfElems++;

		temp = temp->next;
	}

	imagePtr *elemsArray = malloc(numOfElems * sizeof(imagePtr));
	if (elemsArray == NULL) {

		return -1.0;
	}

	int ind = 0;
	while (elems != NULL) { // Transfer all images to an array to find median
		elemsArray[ind] = elems->im;
		elems = elems->next;

		ind++;
	}

	uint8_t *centrPixels = imageGetPixels(*centroid);

	// Torben Median Algorithm by Torben Mogensen
	// It does not modify the input array when looking for the median
	// Source: http://ndevilla.free.fr/median/median/src/torben.c
	for (int pixInd = 0; pixInd < imageSize; pixInd++) {
		int i, less, greater, equal;
		int min, max, guess, maxltguess, mingtguess;

		uint8_t elem0 = imageGetPixels(elemsArray[0])[pixInd]; // Pixel 0 of pixInd'th image
		uint8_t elemi; // Pixel i of pixInd'th image

		min = max = elem0;
		for (i = 1; i < numOfElems; i++) {
			elemi = imageGetPixels(elemsArray[i])[pixInd];
			if (elemi < min) {

				min = elemi;
			}

			if (elemi > max) {

				max = elemi;
			}
		}

		while (1) {
			guess = (min + max) / 2;
			less = 0;
			greater = 0;
			equal = 0;

			maxltguess = min;
			mingtguess = max;

			for (i = 0; i < numOfElems; i++) {
				elemi = imageGetPixels(elemsArray[i])[pixInd];
				if (elemi < guess) {
					less++;

					if (elemi > maxltguess) {

						maxltguess = elemi; 
					}

				}

				else if (elemi > guess) {
					greater++;

					if (elemi < mingtguess) {

						mingtguess = elemi;
					}
				} 

				else {

					equal++;
				}
			}

			if (less <= (numOfElems + 1) / 2 && greater <= (numOfElems + 1) / 2) {

				break; 
			}

			else if (less > greater) {

				max = maxltguess;
			}

			else {

				min = mingtguess;
			}
		}

		if (less >= (numOfElems + 1) / 2) {

			min = maxltguess;
		}
		else if (less+equal >= (numOfElems + 1) / 2) {

			min = guess;
		}

		else {

			min = mingtguess;
		}

		if (numOfElems & 1) { // if numOfElems is odd, we are done
			avgDisplacement += abs(min - centrPixels[pixInd]);

			centrPixels[pixInd] = min; // Update new centroid's pixel
		}

		if (greater >= (numOfElems + 1) / 2) {

			max = mingtguess;
		}

		else if (greater+equal >= (numOfElems + 1) / 2) {

			max = guess;
		}

		else {

			max = maxltguess;
		}

		avgDisplacement += abs(((min + max) / 2) - centrPixels[pixInd]);
		centrPixels[pixInd] = (min + max) / 2; // Update new centroid's pixel
	}

	free(elemsArray);

	return avgDisplacement / imageSize;
}