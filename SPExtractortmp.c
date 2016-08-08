#include <stdlib.h>
#include "SPConfig.h"
#include "ctype.h"

int SPgetLine(char** linePtr, FILE* feats) {

	char* line = NULL;
	int* sizeBuffer = NULL;
	int chk = getline(&line, sizeBuffer, feats);
	if (chk == -1 || !line || !sizeBuffer) {	//Error checking for getline
		free(line);
		//TODO logger - do a warning, Error while reading the file
		return -1;
	}
	line[sizeBuffer - 1] = '\0';	//instead of '/n' as assigned by getline
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
	char* c = *line;
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
SPPoint* extractSingleImage(char* path, int imgIndex, int* numOfFeatures,
		SPConfig config) {
	if (!path || !numOfFeatures)
		//TODO logger
		return NULL;

	FILE* feats = fopen(path, "r");
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
	SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
	int dim = spConfigGetPCADim(config, &conf_msg);
	SPPoint* ret = (SPPoint*) calloc(*numOfFeatures, sizeof(*ret));

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
				spPointArrayDestroy(currentPoint);
				//TODO more features than written warning
			}
			for (; countOfFeatures >= 0; countOfFeatures--) {
				spPointArrayDestroy(ret[countOfFeatures]);
			}
			free(ret);
			fclose(feats);
			return NULL;
		}
		/************/

		ret[countOfFeatures++] = currentPoint;

	}
	countOfFeatures--;

	if (countOfFeatures != *numOfFeatures) {
		//TODO less features than written warning
		for (; countOfFeatures >= 0; countOfFeatures--) {
			spPointArrayDestroy(ret[countOfFeatures]);
		}
		free(ret);
		fclose(feats);
		return NULL;
	}

	return ret;

}

SPPoint* extractImagesFeatures(SPConfig config, SP_LOGGER_MSG* log_msg,
		SP_CONFIG_MSG* conf_msg) {
	if (!config || !log_msg || !conf_msg)
		return NULL;

	int numOfImages = spConfigGetNumOfImages(config);

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
		char path[1024] = { '\0' };
		spConfigGetImagePath(path, config, img);//no need to check configMsg - we know what the args

		/* Extract feautres of single image from the appropriate file*/
		SPPoint* singleImageFeatures = extractSingleImage(path, img,
				&numOfFeatures[img], config);
		if (!singleImageFeatures) {
			//Error inside
			free(numOfFeatures);
			for (; img >= 0; img--) {
				spPointArrayDestroy(imagesFeatures[img], numOfFeatures[img]);
			}
			free(imagesFeatures);
			return NULL;
		}
		imagesFeatures[img] = singleImageFeatures;

	}

	int totalNumOfFeatures = 0;
	for(int i=0;i<numOfImages;i++){
		totalNumOfFeatures += numOfFeatures[i];
	}

	SPPoint** imagesFeatures = (SPPoint**) calloc(numOfImages,
				sizeof(SPPoint*));

	return NULL;
}

