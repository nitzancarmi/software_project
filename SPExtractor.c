#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPExtractor.h"

/******************************************************************************************************/
/*************************************** Import from Files ********************************************/
/******************************************************************************************************/

int getline(char **lineptr, int* n, FILE *stream) {
	char *bufptr = NULL;
	char *p = bufptr;
	int size;
	int c;

	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (!bufptr || (bufptr && !size)) {
		if (!bufptr)
			free(bufptr);
		bufptr = malloc(128);
		if (bufptr == NULL) {
			return -1;
		}
		size = 128;
	}
	p = bufptr;
	while (c != EOF) {
		if ((p - bufptr) > (size - 1)) {
			size = size + 128;
			bufptr = realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		*p++ = c;
		if (c == '\n') {
			break;
		}
		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
}

int SPgetLine(char** linePtr, FILE* feats) {
	declareLogMsg();
	char line[50000];

	//int chk = getline(&line, sizeBuffer, feats);
	//int chk = 0;
	char* chk = fgets(line, 50000, feats);
	if (!chk) {	//Error checking for getline
		//free(line);
		//printf("\nchkline = %p",chk,line,0);
		printWarning("Error while reading the file. SKIPPING FILE")
		return -1;
	}
	int size = strlen(line);

	line[size] = '\0';
	*linePtr = (char*) malloc(size + 1);
	if (*linePtr == NULL) {
		MallocError()
		return -1;
	}

	strcpy(*linePtr, line);                        // Copy the characters
	//*linePtr[size - 1] = '\0';
	return 0;
}

int getIntFromLine(FILE* feats) {
	declareLogMsg();
	long int imageIndex = -1;
	char* line = NULL;
	if (SPgetLine(&line, feats) == -1) {
		return -1;
	}

	char* ptrChk;

	//printf("line = %s\n", line);
	imageIndex = strtol(line, &ptrChk, 0);
	if (ptrChk != NULL) {
		char* tmp = ptrChk;
		while (*tmp != '\0') {
			if (!isspace(*tmp)) {
				printWarning("Letters found on file. SKIPPING FILE")
				free(line);
				return -1;
			}
			tmp++;
		}
	}
	free(line);
	return imageIndex;
}

double* getDataFromLine(char* line, int dim, int lineNumber) {
	declareLogMsg();

	if (!line || dim <= 0) {
		InvalidError()
		return NULL;
	}

	double* ret = (double*) malloc(dim * sizeof(double));
	if (!ret) {
		MallocError()
		return NULL;
	}
	char* c = line;
	int count = 0,
	i;

	while (*c != '\0' && *c != EOF) {
		i = 0;
		char buffer[1024] = { '\0' };

		/** Read one double **/
		while (*c != ' ' && *c != '\0' && *c != '\n' && *c != EOF) {
			if (!isdigit(*c) && *c != '.' && *c != '-') {
				warningWithArgs(
						"%s\n feature = %d, line = %d, char = <%d>\nline is:\n%s",
						SYNTAX, count, lineNumber, *c, line)
				free(ret);
				return NULL;
			}
			buffer[i++] = *c;
			c++;
		}
		char* tail = NULL;
		buffer[i] = '\0';
		double singleNumber = strtod(buffer, &tail);

		/** Error reading the double **/
		if (tail != NULL) {
			if (*tail != '\0') {
				printWarning(SYNTAX)
				free(ret);
				return NULL;
			}
		}
		if (count > dim) {
			warningWithArgs(
					"Features size is %d features, spCADimension = %d. SKIPPING FILE",
					count, dim);
			free(ret);
			return NULL;
		}

		ret[count++] = singleNumber;
		c++;
	}
	if (count != dim) {
		printWarning(
				"Feature size is smaller than spCADimension. SKIPPING FILE")
		free(ret);
		return NULL;
	}

	return ret;

}

//Not using the index in the path
SPPoint* extractSingleImage(int imgIndex, int* numOfFeatures, SPConfig config) {
	declareLogMsg();

	if (!numOfFeatures || !config) {
		InvalidError()
		return NULL;
	}

	/*** Change filepath to .feats ***/
	char filepath[1024] = {'\0'};
	SP_CONFIG_MSG _conf_msg = spConfigGetImagePath(filepath, config, imgIndex),
	*conf_msg = &_conf_msg;
	returnIfConfigMsg(NULL);

	char* ptr = filepath + strlen(filepath);
	while (*ptr != '.') {
		*ptr = '\0';
		ptr--;
	}
	sprintf(ptr, ".feats");
	/*********************************/

	FILE* feats = fopen(filepath, "r");
	if (!feats) {
		warningWithArgs("File \"%s\" cannot be opened. SKIPPING FILE", filepath)
		return NULL;
	}

	int imageIndex = getIntFromLine(feats);
	if (imageIndex == -1) {
		fclose(feats);
		return NULL;
	}

	*numOfFeatures = getIntFromLine(feats);
	if (*numOfFeatures == -1) {
		fclose(feats);
		return NULL;
	}

	int countOfFeatures = 0;
	int dim = spConfigGetPCADim(config, conf_msg);
	SPPoint* ret = (SPPoint*) calloc(*numOfFeatures, sizeof(*ret));	//Returned array

	while (!feof(feats)) {

		char* line = NULL;
		int lineChk = SPgetLine(&line, feats);

		if (lineChk == -1) {
			//Error message printed inside
			fclose(feats);
			return NULL;
		}

		double* data = getDataFromLine(line, dim, countOfFeatures + 3);
		if (!data) {
			//Error message printed inside
			fclose(feats);
			return NULL;
		}

		SPPoint currentPoint = spPointCreate(data, dim, imageIndex);

		/*** ERRORS ***/
		if (!currentPoint || countOfFeatures > *numOfFeatures) {
			if (!currentPoint) {
				MallocError()
				free(data);
			} else {
				spPointDestroy(currentPoint);
				warningWithArgs(
						"Features file %d features, while %d was the quantity defined. SKIPPING FILE",
						countOfFeatures, *numOfFeatures);
			}
			spPointArrayDestroy(ret, *numOfFeatures);

			fclose(feats);
			return NULL;
		}
		/************/

		ret[countOfFeatures++] = currentPoint;

	}

	if (countOfFeatures != *numOfFeatures) {
		printWarning(
				"Features file contains less features than defined in it. SKIPPING FILE")
		spPointArrayDestroy(ret, *numOfFeatures);
		free(ret);
		fclose(feats);
		return NULL;
	}

	//return NULL;
	return ret;
}

SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config,
		SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg) {
	if (!config || !log_msg || !conf_msg)
		return NULL;

	int numOfImages = spConfigGetNumOfImages(config, conf_msg);

	/** First maintain a SPPoint matrix -
	 * each line is for a single image (every SPPoint is a feature of the said image)
	 * Also maintain a int array for the number of features for each image,
	 * because the number of features may vary.
	 */
	int* numOfFeatures = (int*) calloc(numOfImages, sizeof(int));

	SPPoint** imagesFeatures = (SPPoint**) calloc(numOfImages,
			sizeof(SPPoint*));
	if (!numOfFeatures || !imagesFeatures) {
		MallocError()
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}
	int imgCount = 0;
	for (int img = 0; img < numOfImages; img++) {

		/* Extract feautres of single image from the appropriate file*/
		SPPoint* singleImageFeatures = extractSingleImage(img,
				&numOfFeatures[img], config);
		if (!singleImageFeatures) {
			continue;
		} else
			imagesFeatures[imgCount++] = singleImageFeatures;

	}
	if (!imgCount) {
		printError("No image features were imported")
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	} else if (imgCount < numOfImages)
		printWarning("Some of the images features were not imported");

	int totalNumOfFeatures = 0;
	for (int i = 0; i < numOfImages; i++) {
		totalNumOfFeatures += numOfFeatures[i];
	}
	*totalNumOfFeaturesPtr = totalNumOfFeatures;

	/* Copy all points from the matrix to a single big point array, to be inserted to a kdarray */

	SPPoint* allFeatures = (SPPoint*) calloc(totalNumOfFeatures,
			sizeof(*allFeatures));
	/** allocation error **/
	if (!allFeatures) {
		MallocError()

		for (int img = imgCount; img >= 0; img--) {
			spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
		}
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}
	/***********************/

	int currentFeature = -1;
	for (int img = 0; img < imgCount; img++) {

		int features = numOfFeatures[img];
		for (int feat = 0; feat < features; feat++) {
			allFeatures[++currentFeature] = imagesFeatures[img][feat];
		}
	}

	for (int img = imgCount; img >= 0; img--) {
		spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
	}
	free(numOfFeatures);
	free(imagesFeatures);

	return allFeatures;
}

/******************************************************************************************************/
/**************************************** Export to Files *********************************************/
/******************************************************************************************************/

int exportImageToFile(SPPoint* pa, int size, int image_index, SPConfig config) {
	declareLogMsg();
	SP_CONFIG_MSG _conf_msg = SP_CONFIG_SUCCESS,
	*conf_msg = &_conf_msg;

	int rc = 0;
	char filepath[1024] = { '\0' };
	*conf_msg = spConfigGetImagePath(filepath, config, image_index);
	returnIfConfigMsg(1)

	/*** Change filepath to .feats ***/
	char* ptr = filepath + strlen(filepath);
	while (*ptr != '.') {
		*ptr = '\0';
		ptr--;
	}
	sprintf(ptr, ".feats");
	//printf("%s\n", filepath);
	/*********************************/

	FILE *output = fopen(filepath, "w");
	if (!output) {
		printError(FEATS_FILE)
		return 1;
	}

	/*writes image index and num of points at the beginning of file*/
	rc = fprintf(output, "%d\n%d\n", image_index, size);
	if (rc < 1) {
		printError(FEATS_PRNT)
		fclose(output);
		return 1;
	}

	/*writes double array inside points*/
	int i, j, dims;
	SPPoint curr;
	for (i = 0; i < size; i++) {
		curr = pa[i];
		dims = spPointGetDimension(curr);
		for (j = 0; j < dims; j++) {

			char endChar;
			if ((j + 1) == dims && i == size - 1) {	//Last feature of last line
				rc = fprintf(output, "%f", spPointGetAxisCoor(curr, j));
			} else {
				if ((j + 1) == dims && i != size - 1) {	//Last feature of another line
					endChar = '\n';
				} else {
					//Regular feature
					endChar = ' ';
				}
				rc = fprintf(output, "%f%c", spPointGetAxisCoor(curr, j),
						endChar);
			}

			if (rc < 1) {
				printError(FEATS_PRNT)
				fclose(output);
				return 1;
			}
		}
	}
	fclose(output);
	return 0;
}
