#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPExtractor.h"

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
	char line[MAX_FEATS_LINE];
	int size;

	/* Get the line */
	chk = fgets(line, MAX_FEATS_LINE, feats);
	if (!chk) {
		printWarning(READ_ERR)
		return ERROR;
	}
	if (*line == '\n') {
		printWarning(SPACE_ERR)
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
	declareLogMsg();
	int integer;
	char *line = NULL,
	*tmp, *ptrChk; //tail pointer for strtol

	/* get the line. if error - message prints inside */
	if (SPgetLine(&line, feats) == ERROR)
		return ERROR;

	/* attempt to parse an int from the line */
	integer = strtol(line, &ptrChk, 0);
	/* check if any characters left after the number */
	if (ptrChk != NULL && *ptrChk != '\0') {

		/* read tail */
		for (tmp = ptrChk; *tmp != '\0'; tmp++) {

			/* allow only space characters after the number */
			if (!isspace(*tmp)) {
				printWarning(CHAR_ERR)
				free(line);
				return ERROR;
			}

		}
	}
	if (integer < 0) {
		printWarning(INDX_ERR)
		free(line);
		return ERROR;
	}
	/* no need for line anymore */
	free(line);
	return integer;
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
 * @param imageIndex	index of the image for debugging purposes
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
double* getDataFromLine(char* line, int dim, int lineNumber, int imageIndex) {

	declareLogMsg();

	if (!line || dim <= 0) {
		InvalidError()
		return NULL;
	}

	/* Init */
	char *c = line;
	char *tail = NULL;
	int count = 0,
	i, lineCnt = 0;
	double singleNumber = 0, *ret;
	int lineLength = strlen(line) + 1;

	/* allocated double array exactly the size of dim (spCADimension) */
	ret = (double*) calloc(dim, sizeof(double));
	if (!ret) {
		MallocError()
		return NULL;
	}

	/* read entire line */
	while (lineCnt != lineLength && *c != '\0' && *c != EOF) { //EOF not supposed to happen, sanity check

		if (isspace(*c)) {
			c++;
			lineCnt++;
			continue;
		}
		i = 0;
		char buffer[MAX_LENGTH] = { '\0' }; //must be declared here to be nulled every time

		/* Read one double, until the next space (' ') found */
		while (lineCnt != lineLength && !isspace(*c) && *c != '\0' && *c != EOF) { //new line or EOF shouldn't happen

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
		if (count == dim) { //means we already found dim+1 doubles in line
			count++; //to signal count != dim
			break;
		}
		ret[count++] = singleNumber;
		c++;
		lineCnt++;
	}

	/* Essentially found less numbers than dim, same error message  - skipping file*/
	if (count != dim) {
		warningWithArgs(FEAT_SIZE_ERR, dim, lineNumber, imageIndex);
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
 * If any error occurred, *numOfFeatures gets the value of 0.
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
 * Otherwise, a SPPoint array that contains all the features of the image.
 */
SPPoint* extractSingleImage(int imgIndex, int* numOfFeatures, SPConfig config) {
	declareLogMsg();

	if (!numOfFeatures || !config) {
		InvalidError()
		return NULL;
	}
	/*** Initializations ***/
	char *ptr,
	*line = NULL, filepath[MAX_LENGTH] = { '\0' };
	FILE* feats;
	int imageIndex, countOfFeatures = 0, dim, lineChk;
	double* data;
	SPPoint currentPoint, *pntsArray = NULL;

	/*** Change filepath to .feats ***/
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

	/** Try to open the feats file **/
	feats = fopen(filepath, READ_MODE);
	if (!feats) {
		warningWithArgs(FILE_ERR, filepath)
		return NULL;
	}

	/** get the image index from the first line **/
	imageIndex = getIntFromLine(feats);

	/* compare with the file name - if not equal or negative skip file */
	if (imageIndex >= 0 && imageIndex != imgIndex) {
		fclose(feats);
		warningWithArgs(INDEX_ERR, imageIndex, filepath)
		return NULL;
	} else if (imageIndex < 0) {
		errorReturn()
	}

	/** get the number of features in the second line **/
	*numOfFeatures = getIntFromLine(feats);
	if (*numOfFeatures < 0) {
		errorReturn()
	}

	dim = spConfigGetPCADim(config, conf_msg);
	pntsArray = (SPPoint*) calloc(*numOfFeatures, sizeof(*pntsArray));
	if (!pntsArray) {
		MallocError()
		errorReturn()
	}

	/** Iterating through lines 3 and above of the feats file **/
	while (!feof(feats)) {

		data = NULL;
		lineChk = SPgetLine(&line, feats);

		if (lineChk == -1) {
			//Error message printed inside SPgetLine
			errorReturn()
		}

		/** Get double array from lines 3 and above **/
		data = getDataFromLine(line, dim, countOfFeatures + 3, imageIndex);
		if (data == NULL) {
			//Error message printed inside getDataFromLine
			free(line);
			line = NULL;
			errorReturn()
		}

		/** Create a point from the data extracted from the line **/
		currentPoint = spPointCreate(data, dim, imageIndex);
		free(data);
		/*** ERRORS ***/
		if (!currentPoint || countOfFeatures == *numOfFeatures) {

			if (!currentPoint) { //error creating the point
				MallocError()
			} else {
				/** Got more features than written in the file -
				 * this is a warning, skipping to next file
				 * while ignoring this one **/
				spPointDestroy(currentPoint);
				warningWithArgs(FEATS_QNTTY_MORE, *numOfFeatures);
			}
			/** freeing the resources that were ignored **/
			free(line);
			line = NULL;
			errorReturn()
		}

		/** inserting point to returned array **/
		if (countOfFeatures < *numOfFeatures)
			pntsArray[countOfFeatures++] = currentPoint;
		free(line);
		line = NULL;
	}

	/** now can be less than within in the file. again - warning and skipping **/
	if (countOfFeatures != *numOfFeatures) {
		warningWithArgs(FEATS_QNTTY_LESS, countOfFeatures, *numOfFeatures);
		free(line);
		line = NULL;
		errorReturn()
	}
	fclose(feats);
	return pntsArray;
}

SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config) {
	SPPoint* singleImageFeatures = NULL;
	int* featAddr;
	declareLogMsg();
	declareConfMsg();

	if (!config) {
		printError(NO_CNFG);
		return NULL;
	}

	int numOfImages = spConfigGetNumOfImages(config, conf_msg);
	if (*conf_msg != SP_CONFIG_SUCCESS || numOfImages < 0) {
		printError(NO_IMG_NUM);
		return NULL;
	}

	/** First maintain a SPPoint matrix -
	 * each row is for a single image (every SPPoint is a feature of the said image)
	 * Also maintain a int array for the number of features for each image,
	 * because the number of features may vary.
	 * This is needed because only at the end we can know how many features we have
	 * overall, and then we could malloc the array to be added to kdarray properly.
	 * */

	int* numOfFeatures = (int*) calloc(numOfImages, sizeof(int)); //array to hold num of features for each img
	SPPoint** imagesFeatures = (SPPoint**) calloc(numOfImages,	//matrix to hold
			sizeof(SPPoint*));
	if (!numOfFeatures || !imagesFeatures) {
		MallocError()
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}
	int imgCount = 0;
	for (int img = 0; img < numOfImages; img++) {
		featAddr = &numOfFeatures[imgCount];
		infoWithArgs(IMG_EXTR, img);
		/* Extract features of single image from the appropriate file*/
		singleImageFeatures = extractSingleImage(img, featAddr, config);
		if (singleImageFeatures == NULL) {
			free(singleImageFeatures);
			numOfFeatures[img] = 0;
			continue;
		} else {
			/** Insert the image's features to its row in the matrix **/
			imagesFeatures[imgCount++] = singleImageFeatures;
		}
	}

	/** No image features were imported **/
	if (!imgCount) {
		printError(NO_FEATS_IMPRT)
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	} else if (imgCount < numOfImages) {
		/** Some of the images features were not imported
		 * Only warning - not finishing program **/
		printWarning(SOME_FEATS);
	}

	/** Overall number of features added **/
	int totalNumOfFeatures = 0;
	for (int i = 0; i < numOfImages; i++) {
		totalNumOfFeatures += numOfFeatures[i];
	}
	*totalNumOfFeaturesPtr = totalNumOfFeatures;

	/** Copy all points from the matrix to a large single point
	 *  array, to be inserted to a kdarray **/
	SPPoint* allFeatures = (SPPoint*) calloc(totalNumOfFeatures,
			sizeof(*allFeatures));
	if (!allFeatures) {
		MallocError()
		for (int img = imgCount - 1; img >= 0; img--)
			spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}

	int currentFeature = 0;
	for (int img = 0; img < imgCount; img++) {
		int features = numOfFeatures[img];
		for (int feat = 0; feat < features; feat++) {
			allFeatures[currentFeature++] = imagesFeatures[img][feat];
		}
		free(imagesFeatures[img]); //freeing only the pointers to the points array, not the points themselves
	}
	free(numOfFeatures);
	free(imagesFeatures); //only pointer to matrix need to be freed - features themselves still being used later
	return allFeatures;
}

int exportImageToFile(SPPoint* pointArray, int size, int image_index,
		SPConfig config) {
	declareLogMsg();

	if(!pointArray || size<1 || image_index <0 || !config) {
		InvalidError();
		return ERR;
	}

	/** init **/
	SP_CONFIG_MSG _conf_msg = SP_CONFIG_SUCCESS,
	*conf_msg = &_conf_msg;
	int rc = OK, i, j, dims;
	char endChar, filepath[MAX_LENGTH] = { '\0' };
	SPPoint curr;

	/** Get file path **/
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
	sprintf(ptr, FEATS_EXT);

	/** Open feats file **/
	printDebug(DBG_CREATE);
	FILE *output = fopen(filepath, WRITE_MODE);
	if (!output) {
		printError(FEATS_FILE)
		return ERR;
	}

	/* Writes image index at 1st line,
	 * No. of points at the 2nd line*/
	rc = fprintf(output, INTS_FEATS_FILE, image_index, size);
	if (rc < ERR) {
		printError(FEATS_PRNT)
		fclose(output);
		return ERR;
	}

	/*writes double array inside points*/

	for (i = 0; i < size; i++) {
		curr = pointArray[i];
		dims = spPointGetDimension(curr);
		for (j = 0; j < dims; j++) {

			/** Last feature of last line **/
			if ((j + 1) == dims && i == size - 1) {
				rc = fprintf(output, FLOAT, spPointGetAxisCoor(curr, j));

			} else {
				/** Last feature of another line **/
				if ((j + 1) == dims && i != size - 1) {
					endChar = '\n';

				} else {
					/** Regular non-at-some-kind-of-end feature **/
					endChar = ' ';
				}
				rc = fprintf(output, FLOAT_CHAR, spPointGetAxisCoor(curr, j),
						endChar);
			}

			/** file write error **/
			if (rc < ERR) {
				printError(FEATS_PRNT)
				fclose(output);
				return ERR;
			}
		}
	}
	fclose(output);
	return OK;
}
