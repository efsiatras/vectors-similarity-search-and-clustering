#ifndef IMAGENODE_H
#define IMAGENODE_H

#include <stdio.h>
#include <stdint.h>

#include "../images/image.h"

typedef struct imageNode *imageNodePtr;

imageNodePtr imageNodeInit(imagePtr); // Initialize imageNode 
								      // Return pointer to imageNode 
								      // Image as argument

void imageNodePush(imageNodePtr *, imagePtr); // Add image to the beginning of imageNode

void imageNodeFree(imageNodePtr); // Free imageNode

int imageNodeListExists(imageNodePtr, imagePtr); // Check if image exists in list of imageNodes

void imageNodeSetNext(imageNodePtr, imageNodePtr); // Set next

imagePtr imageNodeGetImage(imageNodePtr); // Get image
imageNodePtr imageNodeGetNext(imageNodePtr); // Get next

float imageUpdateMedian(imageNodePtr, imagePtr *, int); // Update centroid based on median

#endif