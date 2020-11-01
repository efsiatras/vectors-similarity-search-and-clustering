#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <limits.h>

#include "utils/functions.h"
#include "utils/timer.h"
#include "images/image.h"
#include "images/imageArray.h"
#include "kmedians/kmedians.h"

int32_t inputInfo(FILE* fp); // Read 32-bit integer from binary file and convert it to big endian integer

int main(int argc, char* argv[]) {
   	int wHyper, wLSH, a, complete = 0, value, methodId, numclusters = -1, L = -1, kLSH = -1, M = -1, kHyper = -1, probes = -1;
	char *train = NULL, *config = NULL, *output = NULL;
    char method[32], param[64];

     // Default values
    wHyper = 40000;
    wLSH = 4000;
    numclusters = 10;
    L = 3;
	kLSH = 4;
    M = 10;
    kHyper = 14;
    probes = 2;

	if (argc < 9) {		
		
		printf("Not enough arguments given\n");
		return EXIT_FAILURE;
	}

	for (a = 1; a < argc; a++) {	// Give arguments in any priority
        if (strcmp(argv[a],"-i") == 0) {

			train = argv[a + 1];    
		}

		if (strcmp(argv[a],"-c") == 0) {

			config = argv[a + 1];    
		}

		if (strcmp(argv[a], "-o") == 0) {

			output = argv[a + 1];    
		}

		if (strcmp(argv[a], "-complete") == 0) { // Set complete variable to 1 to initialize "complete" execution

			complete = 1;    
		}

		if (strcmp(argv[a], "-m") == 0) {

			strcpy(method, argv[a + 1]);    
		}
	}

	if (strcmp(method, "Classic") == 0) {

		methodId = 0;
	}
		
	else if (strcmp(method, "LSH") == 0) {

		methodId = 1;
	}

	else if (strcmp(method, "Hypercube") == 0) {

		methodId = 2;
	}

	else {
		perror("Wrong method was given\n");

		return EXIT_FAILURE;
	}

	FILE *fcon = fopen(config, "r");
	if (fcon == NULL) {	// If file cannot be open return
		perror("Cannot open configuration file\n");

		return INT_MIN;
	}

	while (fscanf(fcon,"%s %d",param, &value) != EOF) { // Scan configuration file for a string and an integer
														// Set value if default value doesn't exist
	
		if (strcmp(param, "number_of_clusters:") == 0 && numclusters == -1) {

			numclusters = value;
		}
		
		if (strcmp(param, "number_of_vector_hash_tables:") == 0 && L == -1) {

			L = value;
		}

		if (strcmp(param, "number_of_vector_hash_functions:") == 0 && kLSH == -1) {

			kLSH = value;
		}

		if (strcmp(param, "max_number_M_hypercube:") == 0 && M == -1) {

			M = value;
		}		

		if (strcmp(param, "number_of_hypercube_dimensions:") == 0 && kHyper == -1) {

			kHyper = value;
		}

		if (strcmp(param, "number_of_probes:") == 0 && probes == -1) {

			probes = value;
		}		
	}
	
	fclose(fcon);	

	FILE *ftr = fopen(train, "rb");
	if (ftr == NULL) {	// If file cannot be open return
		perror("Cannot open binary file\n");

		return INT_MIN;
	}
	
	fseek(ftr, 4, SEEK_SET);	// Ignore magic number

	int32_t numOfImages, rows, cols;

	numOfImages = inputInfo(ftr);	// Get number of images from binary file
	rows = inputInfo(ftr);			// Get number of rows from binary file
	cols = inputInfo(ftr);			// Get number of columns from binary file

	if (numOfImages == -1 || rows == -1 || cols == -1) {
		printf("A wrong part of the file was read\n");

		return INT_MIN;
	}

	int32_t imageSize = rows * cols;
	imageArrayPtr imArrTrain = NULL;

	imArrTrain = imageArrayInit(numOfImages, imageSize);	// Initialize an array of images

	for (int i = 0; i < numOfImages; i++) {
		uint8_t *pix = malloc(imageSize * sizeof(uint8_t)); 

		if (fread(pix, sizeof(uint8_t), imageSize, ftr) != (size_t)imageSize) {
			fputs("error: file exceeds buffer size, or fread failed.\n", stderr);
			
			return INT_MIN;
		}

		imagePtr image = imageInit(i+1, pix); // Initialize image
		imageArrayPush(imArrTrain, image, i); // Push image inside the image array
	}

	fclose(ftr);

	struct timespec startCluster;
	long double totalTimeCluster = 0.0;

	startCluster = startTimer();

	kmediansPtr km = NULL;
	km = kmediansInit(methodId, numclusters, imArrTrain);

	FILE *fout = fopen(output, "w");
	if (fout == NULL) {	// If file cannot be open return
		perror("Cannot open binary file\n");

		return INT_MIN;
	}

	fprintf(fout, "Algorithm: ");	

	if (methodId == 0) {
		fprintf(fout, "Lloyds\n");

		classicLloyd(km, imArrTrain);
	}

	else if (methodId == 1) {
		fprintf(fout, "Range Search LSH\n");

		LSHReverseAssign(km, imArrTrain, L, imageSize, wLSH, kLSH);
	}
	
	else if (methodId == 2) {
		fprintf(fout, "Range Search Hypercube\n");

		hypercubeReverseAssign(km, imArrTrain, wHyper, kHyper);
	}

	totalTimeCluster = endTimer(startCluster) / 1000000000.0; // Nanoseconds to seconds

	for (int i = 0; i < kmediansGetNumOfClusters(km); i++) {

		fprintf(fout, "CLUSTER-%d ", i+1);

		fprintf(fout, "{size: %d,", clusterCountElements(km, i));
		
		fprintf(fout, "centroid: [");
		imagePtr im = clusterGetCentroid(km, i); 
		uint8_t *pix = imageGetPixels(im);

		for (int j = 0; j < imageSize; j++) {		

			if (j == imageSize - 1) {

				fprintf(fout, "%d]}\n", pix[j]);
			}

			else {

				fprintf(fout, "%d, ", pix[j]);
			}
		}
	}

	fprintf(fout, "clustering_time: %Lf s\n", totalTimeCluster);
	fprintf(fout, "Silhouette: [");

	double *s = silhouette(km);

	for (int i = 0; i < (kmediansGetNumOfClusters(km) + 1); i++) {

		if (i == kmediansGetNumOfClusters(km)) {

			fprintf(fout, "%f]\n", s[i]);
		}

		else {
			
			fprintf(fout, "%f, ", s[i]);
		}
	}

	free(s);

	if (complete == 1) {

		for (int i = 0; i < kmediansGetNumOfClusters(km); i++) {
		
			fprintf(fout, "{centroid: [");
			imagePtr im = clusterGetCentroid(km, i); 
			uint8_t *pix = imageGetPixels(im);

			for (int j = 0; j < imageSize; j++) {		

				if (j == imageSize - 1) {

					fprintf(fout, "%d], ", pix[j]);
				}

				else {

					fprintf(fout, "%d, ", pix[j]);
				}
			}

			imageNodePtr elem = clusterGetElement(km, i);

			while (elem != NULL) {

				imagePtr im = imageNodeGetImage(elem);
				int imNum = imageGetNumber(im);

				elem = imageNodeGetNext(elem);

				if (elem == NULL) {

					fprintf(fout, "%d}\n", imNum);
				}

				else {

					fprintf(fout, "%d, ", imNum);
				}
			}
		}
	}

	fflush(fout);
	fclose(fout);


	imageArrayFree(imArrTrain);
	kmediansFree(km);

	return EXIT_SUCCESS;
}

int32_t inputInfo(FILE* fp) { // Read 32-bit integer from binary file and convert it to big endian integer

	int32_t leNum, b0, b1, b2, b3, beNum;

	if (ftell(fp) > 12) {

		return INT_MIN;
	}

	if (fread(&leNum, sizeof(int32_t), 1, fp) != 1) {

		return INT_MIN;
	}
	
	// Do the proper shifts
	b0 = (leNum & 0x000000ff) << 24;
	b1 = (leNum & 0x0000ff00) << 8;
	b2 = (leNum & 0x00ff0000) >> 8;
	b3 = (leNum & 0xff000000) >> 24;

	beNum = b0 | b1 | b2 | b3; // Reconstruct integer in big endian format

	return beNum;
}
