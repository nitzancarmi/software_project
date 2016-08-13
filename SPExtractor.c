#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPExtractor.h"

/******************************************************************************************************/
/*************************************** Import from Files ********************************************/
/******************************************************************************************************/

/**
 * This function reads the next entire line from file stream, allocates enough space to store the text
 * (including the null character, not including the new line) and storing it in *lineptr.
 * When the function returns, *lineptr is a char* which points to the text of the line.
 *
 * @param linePtr	Address of the line to be written to
 * @param feats		FILE stream to be read.
 *
 * @return
 * -1 for failure if one of the following occurred:
 * 		- Memory allocation Error
 * 		- linePtr == NULL || feats == NULL
 * 		- Reading Error
 * 		- Got EOF character (not supposed to happen)
 * 0 for success
 */
int SPgetLine(char** linePtr, FILE* feats) {
	declareLogMsg();
	if (!linePtr || !feats) {
		InvalidError()
		return ERROR;
	}

	/* Initializations */
	char *chk;
	char line[MAX_FEATS_LINE]; //More than enough to hold the line
	int size;

	/* Get the line */
	chk = fgets(line, MAX_FEATS_LINE, feats);
	if (!chk) {
		/* Getting here only if there is a read error
		 * or end of file reached - not supposed to get here because
		 * EOF is checked as while condition in @extractSingleImage */
		printWarning(READ_ERR)
		return ERROR;
	}
	/* size of actual line copied */
	size = strlen(line);

	/* sanity check */
	line[size] = '\0';

	/* allocation for line to be returned */
	*linePtr = (char*) malloc(size + 1);
	if (*linePtr == NULL) {
		MallocError()
		return ERROR;
	}

	/* Copy the characters */
	strcpy(*linePtr, line);
	return OK;
}

/**
 * This function reads the next entire line from file stream, and returns the integer found in it,
 * using the strtol function (technicaly accetps hex numbers also using 0x or 0X before the number).
 * The function can accept space characters ONLY (no '\n', obviously, because it reads only one line)
 * before and after the integer, but no other characters are permitted.
 *
 * @see   strtol
 *
 * @param feats		FILE stream to be read.
 *
 * @return
 * -1 for failure if one of the following occurred:
 * 		- Memory allocation Error
 * 		- feats == NULL
 * 		- Reading Error
 * 		- Got EOF character (not supposed to happen)
 * 		- Other characters that are not number found on line (and the number is not correct hexadecimal)
 * otherwise the converted integer
 */
int getIntFromLine(FILE* feats) {

	int imageIndex;
	char *line = NULL, *tmp, *ptrChk; //tail pointer for strtol

	/* get the line. if error - message prints inside */
	if (SPgetLine(&line, feats) == ERROR)
		return ERROR;

	/* attempt to parse an int from the line */
	imageIndex = strtol(line, &ptrChk, 0);

	/* check if any characters left after the number */
	if (ptrChk != NULL && *ptrChk != '\0') {

		/* read tail */
		for (tmp = ptrChk; *tmp != '\0'; tmp++) {

			/* allow only space characters after the number */
			if (!isspace(*tmp)) {
				declareLogMsg();
				printWarning(CHAR_ERR)
				free(line);
				return ERROR;
			}

		}
	}
	/* no need for line anymore */
	free(line);
	return imageIndex;
}

//TODO
void printDoubleArray(double* a, int size, const char* name) {
	if (!a) {
		printf("error");
		return;
	}
	printf("%s: (", name);
	for (int i = 0; i < size; i++) {
		printf((i == size - 1 ? "%.6f" : "%.6f, "), a[i]);
	}
	printf(")\n");
}

/**
 * This function reads an array of doubles from a string, each one can be separated by space characters
 * beforehand and afterhand. The doubles are then parsed by the @strtod function. No other characters
 * are permitted.
 *
 * @see   strtod
 *
 * @param line			string to be read
 * @param dim			size of the expected array
 * @param lineNumber	number of the line in the file for debugging purposes
 *
 * @return
 * NULL for failure if one of the following occurred:
 * 		- Memory allocation Error
 * 		- line == NULL || dim <= 0
 * 		- Reading Error
 * 		- Got EOF character (not supposed to happen)
 * 		- Other characters that are not number found on line (and the number is not correct hexadecimal)
 * 		- number of doubles converted is different than dim argument
 * a new allocated double array the size of dim, with the converted doubles.
 */
double* getDataFromLine(char* line, int dim, int lineNumber) {

	/* Init */
	char *c = line;
	char *tail = NULL;
	int count = 0, i, lineCnt = 0;
	double singleNumber = 0;
	int lineLength = strlen(line) + 1;

	declareLogMsg();
	if (!line || dim <= 0) {
		InvalidError()
		return NULL;
	}

	/* allocated double array exactly the size of dim (spCADimension) */
	double* ret = (double*) calloc(dim, sizeof(double));
	if (!ret) {
		MallocError()
		return NULL;
	}

	/* read entire line */
	while (lineCnt != lineLength && *c != '\0' && *c != EOF) { //EOF not supposed to happen, sanity check

		if (isspace(*c)) {
			lineCnt++;
			continue;
		}
		i = 0;
		char buffer[1024] = { '\0' }; //must be declared here to be nulled every time

		/* Read one double, until the next space (' ') found */
		while (!isspace(*c) && *c != '\0' && *c != EOF && lineCnt != lineLength) { //new line or EOF shouldn't happen

			/* Contains other characters than the valid double ones */
			if (!isdigit(*c) && *c != '.' && *c != '-') {
				warningWithArgs(FEAT_ERR2, count, lineNumber, *c, (int ) *c,
						line)
				free(ret);
				return NULL;
			}

			/* store character in buffer */
			buffer[i++] = *c;
			c++;
			lineCnt++;
		}
		tail = NULL;

		/* i - basically strlen */
		buffer[i] = '\0';

		/* extract double */
		singleNumber = strtod(buffer, &tail);

		/* Error reading the double */
		if (tail != NULL) {
			if (*tail != '\0') {
				printWarning(SYNTAX)
				free(ret);
				return NULL;
			}
		}
		/* found more numbers than dim - skipping file*/
		if (count > dim) {
			warningWithArgs(FEAT_SIZE_ERR, count, dim);
			free(ret);
			return NULL;
		}

		ret[count++] = singleNumber;
		c++;
		lineCnt++;
	}

	/* Essentially found less numbers than dim, same error message  - skipping file*/
	if (count != dim) {
		warningWithArgs(FEAT_SIZE_ERR, count, dim);
		free(ret);
		return NULL;
	}

	return ret;
}

/*
 * Returns an SPPoint array that contains all features of a single image with the index imgIndex.
 * Expected name of the .feats file are as the image filename, only with the extension ".feats"
 * instead of the image extension (as the pattern defined in the configuration object).
 * The size of the array is stored in *numOfFeatures.
 *
 * Format of the file is expected to be as created by the function exportImageToFile.
 * @see exportImageToFile
 * @see extractImagesFeatures for list of constraints of the file.
 *
 * If any error occurred, *numOfFeatures gets the value of -1.
 *
 * @param imgIndex 	 		 Index of the relevant image
 * @param numOfFeatures	 	 Address of the array size
 * @param config			 Configuration attributes
 *
 * @return
 * NULL if failure for one of the following reasons:
 * 		- .feats File format invalid
 * 		- config == NULL || numOfFeatures == NULL
 * 		- SPConfig access error
 * 		- Memory Allocation Failure
 * 		- Reading Error
 * 		- image index written in the file (first line) doesn't match argument imgIndex
 * 		- The file doesn't contain the same amount of features as written on the second line of the file exactly
 * 0 for successful operation
 */
SPPoint* extractSingleImage(int imgIndex, int* numOfFeatures, SPConfig config) {
	declareLogMsg();

	if (!numOfFeatures || !config) {
		InvalidError()
		return NULL;
	}
	/*** Initializations ***/
	char *ptr,
	*line = NULL;
	FILE* feats;
	int imageIndex, countOfFeatures, dim, lineChk;
	double* data;
	SPPoint currentPoint, *pntsArray;

	/*** Change filepath to .feats ***/
	char filepath[1024] = { '\0' };
	SP_CONFIG_MSG _conf_msg = spConfigGetImagePath(filepath, config, imgIndex),
			*conf_msg = &_conf_msg;
	returnIfConfigMsg(NULL);

	ptr = filepath + strlen(filepath);

	while (*ptr != '.' && ptr != filepath) {
		*ptr = '\0';
		ptr--;
	}
	if (ptr == filepath) {
		printError(FEATS_SUFFIX);
	}
	sprintf(ptr, FEATS_EXT);
	/*********************************/

	/** Try to open the feats file **/
	feats = fopen(filepath, "r");
	if (!feats) {
		warningWithArgs(FILE_ERR, filepath)
		return NULL;
	}

	/** get the image index from the first line **/
	imageIndex = getIntFromLine(feats);

	/* compare with the file name - if not equal or negative skip file */
	if (imageIndex != imgIndex) {
		warningWithArgs(INDEX_ERR, imageIndex, filepath)
		return NULL;
	} else if (imageIndex < 0) {
		extractError()
	}

	/** get the number of features in the second line **/
	*numOfFeatures = getIntFromLine(feats);
	if (*numOfFeatures < 0) {
		extractError()
	}

	countOfFeatures = 0;
	dim = spConfigGetPCADim(config, conf_msg);
	pntsArray = (SPPoint*) calloc(*numOfFeatures, sizeof(*pntsArray)); //Returned array

	while (!feof(feats)) {

		data = NULL;
		line = NULL;
		lineChk = SPgetLine(&line, feats);

		if (lineChk == -1) {
			//Error message printed inside
			extractError()
		}

		data = getDataFromLine(line, dim, countOfFeatures + 3);
		if (data == NULL) {
			//Error message printed inside
			extractError()
		}

		currentPoint = spPointCreate(data, dim, imageIndex);

		/*** ERRORS ***/
		if (!currentPoint || countOfFeatures > *numOfFeatures) {
			if (!currentPoint) {
				MallocError()
				free(data);
			} else {
				spPointDestroy(currentPoint);
				warningWithArgs(FEATS_QNTTY, countOfFeatures, *numOfFeatures);
			}
			spPointArrayDestroy(pntsArray, *numOfFeatures);

			extractError()
		}
		/************/

		pntsArray[countOfFeatures++] = currentPoint;
	}

	if (countOfFeatures != *numOfFeatures) {
		warningWithArgs(FEATS_QNTTY, countOfFeatures, *numOfFeatures);
		spPointArrayDestroy(pntsArray, *numOfFeatures);
		free(pntsArray);
		extractError()
	}

	return pntsArray;
}

SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config,
		SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg) {
	SPPoint* singleImageFeatures = NULL;
	int* featAddr;

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
		featAddr = &numOfFeatures[img];

		/* Extract feautres of single image from the appropriate file*/
		singleImageFeatures = extractSingleImage(img, featAddr, config);

		if (singleImageFeatures == NULL) {
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

		for (int img = imgCount - 1; img >= 0; img--) {
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

//	for (int img = imgCount - 1; img >= 0; img--) {
//		spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
//	}
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
	while (*ptr != '.' && ptr != filepath) {
		*ptr = '\0';
		ptr--;

	}
	if (ptr == filepath) {
		printError(FEATS_SUFFIX);
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
