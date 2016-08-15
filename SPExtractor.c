#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPExtractor.h"

#define declareLogMsg() SP_LOGGER_MSG _log_msg = SP_LOGGER_SUCCESS, *log_msg = &_log_msg


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
=======
int getline(char **lineptr, int* n, FILE *stream) {
	char *bufptr = NULL;
	char *p = bufptr;
	int size;
	int c;

//	if (!lineptr || !stream || !n) {
//		return -1;
//	}
	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF) {
		printf("\n\n##1\n\n");
		return -1;
	}
	if (bufptr == NULL) {
		bufptr = malloc(128);
		if (bufptr == NULL) {
			printf("\n\n##2\n\n");
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
				printf("\n\n##3\n\n");
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
	*linePtr = (char*) malloc(size+1);
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
>>>>>>> added unnecessary files to repository
	free(line);
	return imageIndex;
}

<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
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

=======
double* getDataFromLine(char* line, int dim,int lineNumber) {
>>>>>>> added unnecessary files to repository
	declareLogMsg();

	if (!line || dim <= 0) {
		InvalidError()
		return NULL;
	}

<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
	/* Init */
	char *c = line;
	char *tail = NULL;
	int count = 0,
	i, lineCnt = 0;
	double singleNumber = 0, *ret;
	int lineLength = strlen(line) + 1;

	/* allocated double array exactly the size of dim (spCADimension) */
	ret = (double*) calloc(dim, sizeof(double));
=======
	double* ret = (double*) malloc(dim * sizeof(double));
>>>>>>> added unnecessary files to repository
	if (!ret) {
		MallocError()
		return NULL;
	}
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6

	/* read entire line */
	while (lineCnt != lineLength && *c != '\0' && *c != EOF) { //EOF not supposed to happen, sanity check

		if (isspace(*c)) {
			lineCnt++;
			continue;
		}
		i = 0;
		char buffer[1024] = { '\0' }; //must be declared here to be nulled every time

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
=======
	char* c = line;
	int count = 0,i;

	while (*c != '\0') {
		i = 0;
		char buffer[1024] = { '\0' };

		/** Read one double **/
		while (*c != ' ' && *c != '\0' && *c!= '\n') {
			if (!isdigit(*c) && *c != '.' && *c != '-') {
				char error[1024] = {'\0'};
				sprintf(error,"%s feature = %d, line = %d, *c = <%d>\nline:\n%s",SYNTAX,count,lineNumber,*c,line);
				printWarning(error)
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
>>>>>>> added unnecessary files to repository
		if (tail != NULL) {
			if (*tail != '\0') {
				printWarning(SYNTAX)
				free(ret);
				return NULL;
			}
		}
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
		/* found more numbers than dim - skipping file*/
		if (count > dim) {
			warningWithArgs(FEAT_SIZE_ERR, count, dim);
=======
		if (count > dim + 1) {
			printf("count = %d, dim = %d\n", count, dim);
			printWarning(
					"Feature size is bigger than spCADimension. SKIPPING FILE")
>>>>>>> added unnecessary files to repository
			free(ret);
			return NULL;
		}

		ret[count++] = singleNumber;
		c++;
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
		lineCnt++;
	}

	/* Essentially found less numbers than dim, same error message  - skipping file*/
	if (count != dim) {
		warningWithArgs(FEAT_SIZE_ERR, count, dim);
=======
	}
	if (count != dim) {
		printWarning(
				"Feature size is smaller than spCADimension. SKIPPING FILE")
>>>>>>> added unnecessary files to repository
		free(ret);
		return NULL;
	}

	return ret;
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
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
=======

}

//Not using the index in the path
>>>>>>> added unnecessary files to repository
SPPoint* extractSingleImage(int imgIndex, int* numOfFeatures, SPConfig config) {
	declareLogMsg();

	if (!numOfFeatures || !config) {
		InvalidError()
		return NULL;
	}
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
	/*** Initializations ***/
	char *ptr,
	*line = NULL, filepath[1024] = { '\0' };
	FILE* feats;
	int imageIndex, countOfFeatures, dim, lineChk;
	double* data;
	SPPoint currentPoint, *pntsArray;

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
		errorReturn()
	}

	/** get the number of features in the second line **/
	*numOfFeatures = getIntFromLine(feats);
	if (*numOfFeatures < 0) {
		errorReturn()
	}

	countOfFeatures = 0;
	dim = spConfigGetPCADim(config, conf_msg);
	pntsArray = (SPPoint*) calloc(*numOfFeatures, sizeof(*pntsArray)); //Returned array

	/** Iterating through lines 3 and above of the feats file **/
	while (!feof(feats)) {

		data = NULL;
		lineChk = SPgetLine(&line, feats);		//The line itself

		if (lineChk == -1) {	//some kind of error while getting line
			//Error message printed inside
			errorReturn()
		}

		/** Get double array from lines 3 and above **/
		data = getDataFromLine(line, dim, countOfFeatures + 3);
		if (data == NULL) {
			//Error message printed inside
			errorReturn()
		}

		/** Create a point from the data extracted from the line **/
		currentPoint = spPointCreate(data, dim, imageIndex);
		free(data);
		data = NULL;
		/*** ERRORS ***/
		if (!currentPoint || countOfFeatures > *numOfFeatures) {
			if (!currentPoint) { //error creating the point
				MallocError()
				free(data);
			} else {
				/** Got more features than written in the file -
				 * this is a warning, skipping to next file
				 * while ignoring this one **/
				spPointDestroy(currentPoint);
				warningWithArgs(FEATS_QNTTY, countOfFeatures, *numOfFeatures);
			}
			/** freeing the resources that were ignored **/
			spPointArrayDestroy(pntsArray, *numOfFeatures);
			errorReturn()
		}
		/************/

		/** inserting point to returned array **/
		pntsArray[countOfFeatures++] = currentPoint;
		free(line);
		line = NULL;
	}
	/** now can be less than within in the file. again - warning and skipping **/
	if (countOfFeatures != *numOfFeatures) {
		warningWithArgs(FEATS_QNTTY, countOfFeatures, *numOfFeatures);
		spPointArrayDestroy(pntsArray, *numOfFeatures);
		free(pntsArray);
		errorReturn()
	}

	return pntsArray;
=======

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
		printWarning("File cannot be opened. SKIPPING FILE")
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

		double* data = getDataFromLine(line, dim,countOfFeatures+3);
		if (!data) {
			printWarning("DATA ERROR")
			//TODO Delete
			//Error message printed inside
			fclose(feats);
			return NULL;
		}

		SPPoint currentPoint = spPointCreate(data, dim, imageIndex);

		/*** ERRORS ***/
		if (!currentPoint || countOfFeatures > *numOfFeatures ) {
			if (!currentPoint) {
				MallocError()
				free(data);
			} else {
				spPointDestroy(currentPoint);
				char error[1024] = {'\0'};
				sprintf(error,"Features file %d features, while %d was the quantity defined. SKIPPING FILE",countOfFeatures,*numOfFeatures);
				printWarning(error)
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
>>>>>>> added unnecessary files to repository
}

SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config,
		SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg) {
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
	SPPoint* singleImageFeatures = NULL;
	int* featAddr;

=======
>>>>>>> added unnecessary files to repository
	if (!config || !log_msg || !conf_msg)
		return NULL;

	int numOfImages = spConfigGetNumOfImages(config, conf_msg);

	/** First maintain a SPPoint matrix -
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
	 * each row is for a single image (every SPPoint is a feature of the said image)
	 * Also maintain a int array for the number of features for each image,
	 * because the number of features may vary.
	 * This is needed because only at the end we can know how many features we have
	 * overall, and then we could malloc the array to be added to kdarray properly.
	 * */

	int* numOfFeatures = (int*) calloc(numOfImages, sizeof(int)); //array to hold num of features for each img

	SPPoint** imagesFeatures = (SPPoint**) calloc(numOfImages,	//matrix to hold
			sizeof(SPPoint*));

=======
	 * each line is for a single image (every SPPoint is a feature of the said image)
	 * Also maintain a int array for the number of features for each image,
	 * because the number of features may vary.
	 */
	int* numOfFeatures = (int*) calloc(numOfImages, sizeof(int));

	SPPoint** imagesFeatures = (SPPoint**) calloc(numOfImages,
			sizeof(SPPoint*));
>>>>>>> added unnecessary files to repository
	if (!numOfFeatures || !imagesFeatures) {
		MallocError()
		free(numOfFeatures);
		free(imagesFeatures);
		return NULL;
	}
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6

	int imgCount = 0;
	for (int img = 0; img < numOfImages; img++) {
		featAddr = &numOfFeatures[img];

		/* Extract features of single image from the appropriate file*/
		singleImageFeatures = extractSingleImage(img, featAddr, config);

		if (singleImageFeatures == NULL) {
			/* If some kind of error occourred (all printed and explained inside -
			 * than skip the invalid file.
			 */
			*featAddr = 0; //so that it wont be summed
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
=======
	int imgCount = 0;
	for (int img = 0; img < numOfImages; img++) {

		/* Extract feautres of single image from the appropriate file*/
		SPPoint* singleImageFeatures = extractSingleImage(img,
				&numOfFeatures[img], config);
		if (!singleImageFeatures) {
			printf("error %d, ", img);
			continue;
		} else
			imagesFeatures[imgCount++] = singleImageFeatures;

	}
	printf("\n");
	if (imgCount < numOfImages)
		printWarning("Some of the images were not imported.");

>>>>>>> added unnecessary files to repository
	int totalNumOfFeatures = 0;
	for (int i = 0; i < numOfImages; i++) {
		totalNumOfFeatures += numOfFeatures[i];
	}
	*totalNumOfFeaturesPtr = totalNumOfFeatures;

<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
	/** Copy all points from the matrix to a large single point
	 *  array, to be inserted to a kdarray **/
	SPPoint* allFeatures = (SPPoint*) calloc(totalNumOfFeatures,
			sizeof(*allFeatures));

=======
	/* Copy all points from the matrix to a single big point array, to be inserted to a kdarray */

	SPPoint* allFeatures = (SPPoint*) calloc(totalNumOfFeatures,
			sizeof(*allFeatures));
>>>>>>> added unnecessary files to repository
	/** allocation error **/
	if (!allFeatures) {
		MallocError()

<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
		for (int img = imgCount - 1; img >= 0; img--) {
=======
		for (int img = imgCount; img >= 0; img--) {
>>>>>>> added unnecessary files to repository
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
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
			//insertion to returned array
			allFeatures[++currentFeature] = imagesFeatures[img][feat];
		}
		free(imagesFeatures[img]);	//freeing only the pointers to the points array, not the points themselves
	}

	free(numOfFeatures);
	free(imagesFeatures); //only pointer to matrix need to be freed - features themselves still being used later

=======
			allFeatures[++currentFeature] = imagesFeatures[img][feat];
		}
	}

>>>>>>> added unnecessary files to repository
	return allFeatures;
}

/******************************************************************************************************/
/**************************************** Export to Files *********************************************/
/******************************************************************************************************/

<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
int exportImageToFile(SPPoint* pointArray, int size, int image_index,
		SPConfig config) {
	declareLogMsg();

	if(!pointArray || size<1 || image_index <0 || !config) {
		InvalidError();
		return 1;
	}

	/** init **/
	SP_CONFIG_MSG _conf_msg = SP_CONFIG_SUCCESS,
	*conf_msg = &_conf_msg;
	int rc = 0, i, j, dims;
	char endChar, filepath[1024] = { '\0' };
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
	sprintf(ptr, ".feats");
	/*********************************/

	/** Open feats file **/
	FILE *output = fopen(filepath, "w");
	if (!output) {
		printError(FEATS_FILE)
		return 1;
	}

	/* Writes image index at 1st line,
	 * No. of points at the 2nd line*/
	rc = fprintf(output, "%d\n%d\n", image_index, size);
	if (rc < 1) {
		printError(FEATS_PRNT)
		fclose(output);
		return 1;
	}

	/*writes double array inside points*/

	for (i = 0; i < size; i++) {
		curr = pointArray[i];
		dims = spPointGetDimension(curr);
		for (j = 0; j < dims; j++) {

			/** Last feature of last line **/
			if ((j + 1) == dims && i == size - 1) {
				rc = fprintf(output, "%f", spPointGetAxisCoor(curr, j));

			} else {
				/** Last feature of another line **/
				if ((j + 1) == dims && i != size - 1) {
					endChar = '\n';

				} else {
					/** Regular non-at-some-kind-of-end feature **/
					endChar = ' ';
				}
=======
int exportImageToFile(SPPoint* pa, int size, int image_index, SPConfig config) {
	declareLogMsg();
	int rc = 0;
	char filepath[1024] = {'\0'};
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
				printError("Cannot create feats file")
				return 1;
			}

			/*writes image index and num of points at the beginning of file*/
			rc = fprintf(output, "%d\n%d\n", image_index, size);
			if (rc < 1) {
				printError("Print to feats file error")
				fclose(output);
				return 1;
			}

			/*writes double array inside points*/
			int i,
	j, dims;
	SPPoint curr;
	for (i = 0; i < size; i++) {
		curr = pa[i];
		dims = spPointGetDimension(curr);
		for (j = 0; j < dims; j++) {

			char endChar;
			if ((j + 1) == dims && i == size - 1) {
				rc = fprintf(output, "%f", spPointGetAxisCoor(curr, j));
			} else {
				if ((j + 1) == dims && i != size - 1) {
					endChar = '\n';
				} else
					endChar = ' ';
>>>>>>> added unnecessary files to repository
				rc = fprintf(output, "%f%c", spPointGetAxisCoor(curr, j),
						endChar);
			}

<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
			/** file write error **/
			if (rc < 1) {
				printError(FEATS_PRNT)
=======
			if (rc < 1) {
				//TODO logger message
>>>>>>> added unnecessary files to repository
				fclose(output);
				return 1;
			}
		}
	}
	fclose(output);
<<<<<<< c4c617ed7625e57c6764432743f8adacd84836d6
	return OK;
=======
	return 0;
>>>>>>> added unnecessary files to repository
}
