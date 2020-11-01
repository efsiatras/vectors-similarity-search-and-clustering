#ifndef KMEDIANS_H
#define KMEDIANS_H

#include <stdio.h>
#include <stdint.h>

#include "../images/image.h"
#include "../images/imageArray.h"
#include "../images/imageNode.h"
#include "../hypercube/vertex.h"
#include "../hypercube/hypercube.h"

typedef struct kmedians *kmediansPtr;

kmediansPtr kmediansInit(int, int, imageArrayPtr); // Initialize kmedians clusters; Return kmedians clusters; 
																		// Method id as argument; Number of clusters as argument; Array of images as argument

int countCentroids(void *, int); // Count number of centroids in a bucket

int *getCentroidsIndicies(void *, int, int); // Get the indicies of all centroids in a bucket

void combineIndicies(int **, int *, int **, int *); // Combine two array of indicies into one, free second array and return to first array

void kmediansFree(kmediansPtr); // Free kmedians clusters

int kmediansGetNumOfClusters(kmediansPtr); // Get number of clusters

imagePtr clusterGetCentroid(kmediansPtr, int); // Get centroid of i-th cluster

imageNodePtr clusterGetElement(kmediansPtr, int); // Get head element of i-th cluster

int clusterCountElements(kmediansPtr, int); // Get number of elements of i-th cluster

int isCentroid(kmediansPtr, int); // Check if image is centroid or not based on id

imagePtr lloydNearest(kmediansPtr, imagePtr); // Get the centroid that image im belongs to
imagePtr lloydNearestIndices(kmediansPtr, int, int *, imagePtr, int); // // Get the closest centroid from a given amount of centroids

int lloydSecondNearestIndex(kmediansPtr, imagePtr); // Get the index of the second closest centroid of an image

int classicLloyd(kmediansPtr, imageArrayPtr); // Classic LLoyd's Algorithm
int LSHReverseAssign(kmediansPtr, imageArrayPtr, int, int, int, int);
int hypercubeReverseAssign(kmediansPtr, imageArrayPtr, int, int); // Reverse Assignment using Hypercube

double* silhouette(kmediansPtr); // Silhouette metric

#endif