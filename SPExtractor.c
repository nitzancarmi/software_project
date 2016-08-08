#include <stdio.h>
#include <stdlib.h>
#include "SPExtractor.h"

/******************************************************************************************************/
/*************************************** Import from Files ********************************************/
/******************************************************************************************************/

int getline(char **lineptr, int *n, FILE *stream) {
	char *bufptr = NULL;
	char *p = bufptr;
	int size;
	int c;

	if (!lineptr || !stream || !n) {
		return -1;
	}
	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (bufptr == NULL) {
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

	char* line = NULL;
	int* sizeBuffer = NULL;
	int chk = getline(&line, sizeBuffer, feats);
	//int chk = 0;
	if (chk == -1 || !line || !sizeBuffer) {	//Error checking for getline
		free(line);
		//TODO logger - do a warning, Error while reading the file
		return -1;
	}
	line[*sizeBuffer - 1] = '\0';	//instead of '/n' as assigned by getline
	*linePtr = line;

	return 0;
}

int getIntFromLine(FILE* feats) {
	char* line = NULL;
	if (SPgetLine(&line, feats) == -1) {
		return -1;
	}

	char* ptrChk = NULL;
	int imageIndex = strtol(line, &ptrChk, 10);
	if (ptrChk != NULL) {
		char* tmp = ptrChk;
		while (*tmp != '\0') {
			if (!isspace(*tmp)) {
				//TODO logger - warning - invalid file - letters found
				free(line);
				return -1;
			}
			tmp++;
		}
	}
	free(line);
	return imageIndex;
}

double* getDataFromLine(char* line, int dim) {
	if (!line || dim <= 0) {
		//TODO invalid args logger
		return NULL;
	}

	double* ret = (double*) malloc(dim * sizeof(double));
	if (!ret) {
		//TODO malloc error
		return NULL;
	}
	char* c = line;
	int count = 0, i = 0;
	while (*c != '\0') {
		char buffer[1024] = { '\0' };

		/** Read one double **/
		while (*c != ' ' && *c != '\0') {
			if (!isdigit(*c) && *c != '.' && *c != '-') {
				//TODO file syntax error
				free(ret);
				return NULL;
			}
			buffer[i++] = *c;
		}
		char* tail = NULL;

		double singleNumber = strtod(buffer, &tail);

		/** Error reading the double **/
		if (tail != NULL) {
			if (*tail != '\0') {
				//TODO file syntax error
				free(ret);
				return NULL;
			}
		}
		if (count > dim) {
			//TODO feature size bigger than dims warning
			free(ret);
			return NULL;
		}

		ret[count++] = singleNumber;

	}
	count--;
	if (count != dim) {
		//TODO feature size smaller than dims warning
		free(ret);
		return NULL;
	}

	return ret;

}

//Not using the index in the path
SPPoint* extractSingleImage(int imgIndex, int* numOfFeatures, SPConfig config) {
	if (!numOfFeatures || !config) {
		//TODO logger
		return NULL;
	}

	/*** Change filepath to .feats ***/
	char filepath[1024] = { '\0' };
	SP_CONFIG_MSG conf_msg = spConfigGetImagePath(filepath, config, imgIndex);
	if (conf_msg != SP_CONFIG_SUCCESS) {
		//TODO logger
		return NULL;
	}

	char* ptr = filepath + strlen(filepath);
	while (*ptr != '.') {
		*ptr = '\0';
		ptr--;
	}
	sprintf(ptr, ".feats");
	/*********************************/

	FILE* feats = fopen(filepath, "r");
	if (!feats)
		//TODO logger - warning - file cannot be opened
		return NULL;

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
	int dim = spConfigGetPCADim(config, &conf_msg);
	SPPoint* ret = (SPPoint*) calloc(*numOfFeatures, sizeof(*ret));	//Returned array

	while (!feof(feats)) {

		char* line = NULL;
		int lineChk = SPgetLine(&line, feats);

		if (lineChk == -1) {
			//Error message printed inside
			fclose(feats);
			return NULL;
		}
		double* data = getDataFromLine(line, dim);
		if (!data) {
			//Error message printed inside
			fclose(feats);
			return NULL;
		}

		SPPoint currentPoint = spPointCreate(data, dim, imageIndex);

		/*** ERRORS ***/
		if (!currentPoint || countOfFeatures > *numOfFeatures) {
			if (!currentPoint) {
				//TODO malloc error
				free(data);
			} else {
				spPointDestroy(currentPoint);
				//TODO more features than written warning
			}
			spPointArrayDestroy(ret, *numOfFeatures);

			fclose(feats);
			return NULL;
		}
		/************/

		ret[countOfFeatures++] = currentPoint;

	}
	countOfFeatures--;

	if (countOfFeatures != *numOfFeatures) {
		//TODO less features than written warning
		spPointArrayDestroy(ret, *numOfFeatures);
		free(ret);
		fclose(feats);
		return NULL;
	}

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
		//TODO malloc error
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}

	for (int img = 0; img < numOfImages; img++) {

		/* Extract feautres of single image from the appropriate file*/
		SPPoint* singleImageFeatures = extractSingleImage(img,
				&numOfFeatures[img], config);
		if (!singleImageFeatures) {
			//Error inside

			for (; img >= 0; img--) {
				spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
			}
			free(numOfFeatures);
			free(imagesFeatures);
			return NULL;
		}
		imagesFeatures[img] = singleImageFeatures;

	}

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
		//TODO malloc error

		for (int img = numOfImages; img >= 0; img--) {
			spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
		}
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}
	/***********************/

	int currentFeature = -1;
	for (int img = 0; img < numOfImages; img++) {

		int features = numOfFeatures[img];
		for (int feat = 0; feat < features; feat++) {
			allFeatures[++currentFeature] = imagesFeatures[img][feat];
		}
	}

	return allFeatures;
}

/******************************************************************************************************/
/**************************************** Export to Files *********************************************/
/******************************************************************************************************/

int exportImageToFile(SPPoint* pa, int size, int image_index, SPConfig config) {
	int rc = 0;
	char filepath[1024] = { '\0' };
	SP_CONFIG_MSG conf_msg = spConfigGetImagePath(filepath, config,
			image_index);
	if (conf_msg != SP_CONFIG_SUCCESS)
		return 1;

	/*** Change filepath to .feats ***/
	char* ptr = filepath + strlen(filepath);
	while (*ptr != '.') {
		*ptr = '\0';
		ptr--;
	}
	sprintf(ptr, ".feats");
	printf("%s\n", filepath); //TODO delete
	/*********************************/

	FILE *output = fopen(filepath, "w");
	if (!output) {
		//TODO logger message
		return 1;
	}

	/*writes image index and num of points at the beginning of file*/
	rc = fprintf(output, "%d\n%d\n", image_index, size);
	if (rc < 1) {
		//TODO logger message
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
			rc = fprintf(output, "%f%c", spPointGetAxisCoor(curr, j),
					((((j + 1) == dims) && (i!=size-1)) ? '\n' : ' '));
			if (rc < 1) {
				//TODO logger message
				fclose(output);
				return 1;
			}
		}
	}
	fclose(output);
	return 0;
}

