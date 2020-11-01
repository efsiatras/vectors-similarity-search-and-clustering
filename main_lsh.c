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
#include "images/imageArray.h"
#include "hashtable/htArray.h"
#include "hashtable/hashtable.h"


int32_t inputInfo(FILE*); // Read 32-bit integer from binary file and convert it to big endian integer

int insTrainFile(char *, imageArrayPtr *, htArrayPtr *, int, int, int); // Read and insert train file
int insQueryFile(char *, imageArrayPtr *); // Read and insert query file

int queries(char *, imageArrayPtr, imageArrayPtr, htArrayPtr, int, int, int, int, double); // Execute approximate and exact k-nearest neighbors queries, r-near neighbors queries and print output

int main(int argc, char* argv[]) {
   	int w, a, k, L, N, flagTr = 0, flagQ = 0, flagOut = 0;
	double R;
	char rep[12];
	char *tr = NULL, *q = NULL, *out = NULL, *train = NULL, *query = NULL, *output = NULL, *eptr = NULL;
    srand(time(NULL));
	
    // Default values
	w = 4000;
	k = 4;
	L = 5;
	N = 1;
	R = 10000.0;

	if (argc < 1) {		
		
		printf("Not enough arguments given\n");
		return EXIT_FAILURE;
	}

	for (a = 1; a < argc; a++) {	// Give arguments in any priority
        if (strcmp(argv[a],"-d") == 0) {

			train = argv[a + 1];    
		}

        if (strcmp(argv[a], "-q") == 0) {

			query = argv[a + 1];    
		}

		if (strcmp(argv[a], "-k") == 0) {

			k = atoi(argv[a + 1]);    
		}

		if (strcmp(argv[a], "-L") == 0) {

			L = atoi(argv[a + 1]);    
		}

        if (strcmp(argv[a], "-o") == 0) {

			output = argv[a + 1];    
		}

		if (strcmp(argv[a], "-N") == 0) {

			N = atoi(argv[a + 1]);    
		}

		if (strcmp(argv[a], "-R") == 0) {

			R = strtod(argv[a + 1], &eptr);    
		}
	}

	htArrayPtr ht = NULL;
	imageArrayPtr imArrTrain = NULL;
	while(1) {

		if (train == NULL) {

			printf("Please give Input file.\n\n");

			tr = malloc(512);
			scanf("%s", tr); 

			train = tr;
			flagTr = 1;

			ht = NULL;
			imArrTrain = NULL;
		}
		
		if (ht == NULL && imArrTrain == NULL) {
			
			if (insTrainFile(train, &imArrTrain, &ht, w, k, L) == INT_MIN) { 

				return EXIT_FAILURE;
			}
		}

		if (query == NULL) {

			printf("Please give Query file.\n\n");

			q = malloc(512);
			scanf("%s", q); 

			query = q;
			flagQ = 1;
		}

		if (output == NULL) {

			printf("Please give Output file.\n\n");

			out = malloc(512);
			scanf("%s", out); 

			output = out;
			flagOut = 1;
		}

		imageArrayPtr imArrQuery = NULL;
		if (insQueryFile(query, &imArrQuery) == INT_MIN) {

			return EXIT_FAILURE;
		}

		if (queries(output, imArrTrain, imArrQuery, ht, w, k, L, N, R) == INT_MIN) {

		return EXIT_FAILURE;
		}

		imageArrayFree(imArrQuery);

		if (flagQ == 1) {

			free(q);
		}

		if (flagOut == 1) {

			free(out);
		}

		printf("Repeat the process? (Y) or (N) \n\n");
		scanf("%s", rep); 

		if ((strcmp(rep, "N") == 0) || (strcmp(rep, "n") == 0)) {

			imageArrayFree(imArrTrain);
			htArrayFree(ht);

			if (flagTr == 1) {

				free(tr);
			}
			
			break;
		}

		printf("Use different train data? (Y) or (N) \n\n");
		scanf("%s", rep);

		if ((strcmp(rep, "Y") == 0) || (strcmp(rep, "y") == 0)) {

			imageArrayFree(imArrTrain);
			htArrayFree(ht);
			
			if (flagTr == 1) {

				free(tr);
			}

			train = NULL;
		}

		query = NULL;
		output = NULL;
	}
	
	return EXIT_SUCCESS;
}

// Read 32-bit integer from binary file and convert it to big endian integer
int32_t inputInfo(FILE* fp) { 

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

// Read and insert train file
int insTrainFile(char *train, imageArrayPtr *imArrTrain, htArrayPtr *ht, int w, int k, int L) { 
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
	*imArrTrain = imageArrayInit(numOfImages, imageSize);	// Initialize an array of images
	for (int i = 0; i < numOfImages; i++) {
		uint8_t *pix = malloc(imageSize * sizeof(uint8_t)); 

		if (fread(pix, sizeof(uint8_t), imageSize, ftr) != (size_t)imageSize) {
			fputs("error: file exceeds buffer size, or fread failed.\n", stderr);
			
			return INT_MIN;
		}

		imagePtr image = imageInit(i+1, pix); // Initialize image
		imageArrayPush(*imArrTrain, image, i); // Push image inside the image array
	}

	*ht = htArrayInit(L, numOfImages / 16);
	hashtablePtr *hashtables = htArrayGetHts(*ht); // Get array of hashtables

	for (int j = 0; j < numOfImages; j++) {
		if (htArrayPush(*ht, imageArrayGetImage(*imArrTrain, j), imageSize, w, k) < 0) {

			return INT_MIN;
		}
	}
	fclose(ftr);

	return 0;
}

// Read and insert query file
int insQueryFile(char *query, imageArrayPtr *imArrQuery) { 
	FILE *fqu = fopen(query, "rb");
	if (fqu == NULL) {	// If file cannot be open return
		perror("Cannot open binary file\n");

		return INT_MIN;
	}
	
	fseek(fqu, 4, SEEK_SET);	// Ignore magic number

	int32_t numOfImages, rows, cols;

	numOfImages = inputInfo(fqu);	// Get number of images from binary file
	rows = inputInfo(fqu);			// Get number of rows from binary file
	cols = inputInfo(fqu);			// Get number of columns from binary file

	if (numOfImages == -1 || rows == -1 || cols == -1) {
		printf("A wrong part of the file was read\n");

		return INT_MIN;
	}

	int32_t imageSize = rows * cols;
	*imArrQuery = imageArrayInit(numOfImages, imageSize);	// Initialize an array of images

	for (int i = 0; i < numOfImages; i++) {
		uint8_t *pix = malloc(imageSize * sizeof(uint8_t)); 

		if (fread(pix, sizeof(uint8_t), imageSize, fqu) != (size_t)imageSize) {
			fputs("error: file exceeds buffer size, or fread failed.\n", stderr);
			
			return INT_MIN;
		}

		imagePtr image = imageInit(i+1, pix); // Initialize image
		imageArrayPush(*imArrQuery, image, i); // Push image inside the image array
	}
	fclose(fqu);

	return 0;
}

// Execute approximate and exact k-nearest neighbors queries, r-near neighbors queries and print output
int queries(char *output, imageArrayPtr imArrTrain, imageArrayPtr imArrQuery, htArrayPtr ht, int w, int k, int L, int N, double R) {
	long double dtLSH; // Times in milliseconds
	long double dtTrue; // Times in milliseconds

	struct timespec startLSH, startTrue;

	FILE *fout = fopen(output, "w");
	if (fout == NULL) {	// If file cannot be open return
		perror("Cannot open binary file\n");

		return INT_MIN;
	}

	int imageSize = imageArrayGetImageSize(imArrQuery);
	int numOfImages = imageArrayGetNumOfImages(imArrQuery);
	for (int i = 0; i < numOfImages; i++) { // For every query image
		dtLSH = 0.0;
		dtTrue = 0.0;
		imagePtr qIm = imageArrayGetImage(imArrQuery, i);

		// Approximate N-nearest neighbors
		imagePtr *apprCandImages;
		int *apprCandDist;

		startLSH = startTimer();  // Start LSH timer
		apprKNN(ht, qIm, imageSize, N, w, k, &apprCandImages, &apprCandDist); // Find approximate N-nearest neighbors
		dtLSH = endTimer(startLSH) / 1000000.0;

		// Exact N-nearest neighbors
		imagePtr *exactCandImages;
		int *exactCandDist;

		startTrue = startTimer(); // Start True timer
		exactKNN(imArrTrain, qIm, imageSize, N, &exactCandImages, &exactCandDist); // Find exact N-nearest neighbors
		//exactKNN(ht, qIm, imageSize, N, &exactCandImages, &exactCandDist); // Find exact N-nearest neighbors
		dtTrue = endTimer(startTrue) / 1000000.0;

		fprintf(fout, "Query %d\n", imageGetNumber(qIm));
		for (int i = 0; i < N; i++) {

			if (apprCandImages[i] == NULL) {

				fprintf(fout, "Nearest neighbor-%d: NOT FOUND\n", i+1);	
				fprintf(fout, "distanceHypercube: NOT FOUND\n");			
			}

			else {
				fprintf(fout, "Nearest neighbor-%d: %d\n", i+1, imageGetNumber(apprCandImages[i]));
				fprintf(fout, "distanceLSH: %d\n", apprCandDist[i]);
				fflush(fout);
			}

			if(exactCandImages[i] == NULL) {

				fprintf(fout, "distanceTrue: NOT FOUND\n");
			}

			else {

				fprintf(fout, "distanceTrue: %d\n", exactCandDist[i]);
			}
		}

		fprintf(fout, "tLSH: %Lf ms\n", dtLSH);
		fprintf(fout, "tTrue: %Lf ms\n", dtTrue);

		free(apprCandImages);
		free(apprCandDist);
		free(exactCandImages);
		free(exactCandDist);

		imageNodePtr apprRCandImages = NULL;
		apprRSearch(ht, qIm, imageSize, R, w, k, &apprRCandImages);
		fprintf(fout, "R-near neighbors:\n");
		while (apprRCandImages != NULL) { // While there are more R near neighbors
			fprintf(fout, "%d\n", imageGetNumber(imageNodeGetImage(apprRCandImages)));

			imageNodePtr temp = apprRCandImages;
			apprRCandImages = imageNodeGetNext(apprRCandImages); // Get next

			imageNodeFree(temp);
		}

		apprRCandImages = NULL;
	}
	fclose(fout);

	return 0;
}
