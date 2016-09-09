#include "SPConfig.h"
#include "macros.h"
#include "string.h"

/*************************/
/*** Struct Definition ***/
/*************************/
struct sp_config_t {
	/*must be set*/
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix;
	int spNumOfImages;

	/*default values*/
	int spPCADimension;
	char* spPCAFilename;
	int spNumOfFeatures;

	bool spExtractionMode;
	int spNumOfSimilarImages;
	splitMethod spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char* spLoggerFilename;
};
/*************************/

/**
 * checks a single line in the configuration file 
 * and extract its variable and value
 * 
 * @param line - line to be checked upon
 * @param msg - config message to be updated in case of
 *              an error
 * @param isCommentBlank - flag to signal calling function
 *                         whether the line should be ignored.
 * @param varReturn - a string that should be updated with the
 *                    name of the variable
 * @param valueReturn - a string that should be updated with the
 *                      value assigned in the line
 *
 * @return
 *  - msg updated with the line reading status
 *    SP_CONFIG_SUCCESS - for success
 *    SP_CONFIG_INVALID_STRING - for failure
 *  - on success - the varReturn and valueReturn are updated correctly 
 */
void checkLine(char* line, SP_CONFIG_MSG* msg, int* isCommentBlank,
		char** varReturn, char** valueReturn) {
	if (!line || !msg || !isCommentBlank || !varReturn || !valueReturn)
		return;
	char *var, *value, *tmp, *save;
	/* pass over spaces to next character*/
	ignoreSpaces(line)

	/* if end of line (only whitespaces or blank) or comment, ignore line */
	if (*line == '\0' || *line == '#') {
		*msg = SP_CONFIG_SUCCESS;
		*isCommentBlank = 1;
		return;
	}

	/* save current position on line */
	var = line;
	value = line;

	/* go to '=' position of value */
	while (*value != '=' && *value != '\0') {
		if (isalpha(*value) || isspace(*value))
			value++;
		else {
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}

	}
	/* if value (line) ended without equal sign then line is invalid */
	if (*value == '\0') {
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}

	/* so the var would end where is the equal sign */
	*value = '\0';

	value++;
	ignoreSpaces(value)

	tmp = var;
	while (!isspace(*tmp) && *tmp!='\0')
		tmp++;
	*tmp = '\0';

	tmp = value;
	while (!isspace(*tmp) && *tmp!='\0')
		tmp++;
	if (*tmp != '\0') {
		save = tmp;
		ignoreSpaces(tmp)
		if (*tmp != '\0') { //space chars found inside string and not at the end
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}
		*save = '\0'; //if spaces are at the end of the value, end value on the first space
	}
	*varReturn = var;
	*valueReturn = value;

	/* check that value is not only spaces */
	for (unsigned int i = 0; i < strlen(value); i++) {
		if (!isspace(*(value + i))) {
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
	}
	*msg = SP_CONFIG_INVALID_STRING;

}

/**
 * updates an integer value for config parameter
 *
 * @param value - the value to be updated
 * @param min - minimum border value
 * @param max - maximum border value
 *
 * @return 0 for success, 1 otherwise
 */
int updateValueInRange(char* value, int min, int max) {
	int num;
	char* tmp = value;
	while (isdigit(*tmp)) {
		tmp++;
	}
	if (*tmp != '\0')
		num = 0;
	else
		num = strtol(value, &tmp, 10);
	return (num < min || num > max) ? 0 : num;
}

/**
 * finds a string value for config parameter in a set
 * possible values
 *
 * @param set - set of possible values
 * @param value - value to ce checked whether in the set or not
 * @param size - size of the set
 *
 * @return index of value in set for success, -1 otherwise
 */
int findValueInSet(const char *set[], char* value, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (!strcmp(set[i], value))
			return i;
	}
	return -1;
}

/**
 * assigns a value into a variable in configuration struct 
 *
 * @param attr - configuration struce to be updated
 * @param var - name of field in the struct to be updated
 * @param value - value to be assigned into struct
 * @param line - line number in config file
 * @param filename - filename of the config file
 *
 * @return SP_CONFIG_SUCCESS for SUCCESS
           SP_CONFIG_INVALID_STRING/INTEGER for failure
 */
SP_CONFIG_MSG assignVarValue(SPConfig attr, char *var, char *value, int line,
		const char* filename) {
	/*spImagesDirectory*/
	/*Constraint: None*/
	if (!strcmp(var, "spImagesDirectory")) {
		assignString(attr->spImagesDirectory, value);
		return SP_CONFIG_SUCCESS;
	}

	/*spImagesPrefix*/
	/*Constraint: None*/
	if (!strcmp(var, "spImagesPrefix")) {
		assignString(attr->spImagesPrefix, value);
		return SP_CONFIG_SUCCESS;
	}

	/*spImagesSuffix*/
	/*Constraint: one of the following: .jpg, .png, .bmp, .gif */
	if (!strcmp(var, "spImagesSuffix")) {
		const char *suffixes[] = { ".jpg", ".png", ".bmp", ".gif" };
		int val = findValueInSet(suffixes, value, 4);

		if (val != -1) {
			assignString(attr->spImagesSuffix, suffixes[val]);
			return SP_CONFIG_SUCCESS;

		} else {
			printf(CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_STRING;
		}
	}

	/*spNumOfImages*/
	/*Constraint: positive integers only*/
	if (!strcmp(var, "spNumOfImages")) {
		int val = updateValueInRange(value, 1, INT_MAX);
		if (val) {
			attr->spNumOfImages = val;
			return SP_CONFIG_SUCCESS;
		} else {
			printf(
			CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_INTEGER;
		}
	}

	/*spPCADimension*/
	/*Constraint: integers in range [10,28]*/
	if (!strcmp(var, "spPCADimension")) {
		int val = updateValueInRange(value, 10, 28);
		if (val) {
			attr->spPCADimension = val;
			return SP_CONFIG_SUCCESS;
		} else {
			printf(
			CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_INTEGER;
		}
	}

	/*spPCAFilename*/
	/*Constraint: None*/
	if (!strcmp(var, "spPCAFilename")) {
		assignString(attr->spPCAFilename, value);
		return SP_CONFIG_SUCCESS;
	}

	/*spNumOfFeatures*/
	/*Constraint: positive integers only*/
	if (!strcmp(var, "spNumOfFeatures")) {
		int val = updateValueInRange(value, 1, INT_MAX);
		if (val) {
			attr->spNumOfFeatures = val;
			return SP_CONFIG_SUCCESS;
		} else {
			printf(
			CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_INTEGER;
		}
	}

	/*spExtractionMode*/
	/*Constraint: one of the following: {true, false}*/
	if (!strcmp(var, "spExtractionMode")) {
		const char *boolean[] = { "true", "false" };
		int val = findValueInSet(boolean, value, 2);
		if (val == 0) {
			attr->spExtractionMode = true;
			return SP_CONFIG_SUCCESS;
		}
		if (val == 1) {
			attr->spExtractionMode = false;
			return SP_CONFIG_SUCCESS;
		}
		printf(CONSTRAINT, filename, line);
		return SP_CONFIG_INVALID_STRING;
	}

	/*spNumOfSimilarImages*/
	/*Constraint: positive integers only*/
	if (!strcmp(var, "spNumOfSimilarImages")) {
		int val = updateValueInRange(value, 1, INT_MAX);
		if (val) {
			attr->spNumOfSimilarImages = val;
			return SP_CONFIG_SUCCESS;
		} else {
			printf(
			CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_INTEGER;
		}
	}

	/*spKDTreeSplitMethod*/
	/*Constraint: one of the following :RANDOM, MAX_SPREAD,INCREMENTAL*/
	if (!strcmp(var, "spKDTreeSplitMethod")) {
		const char *options[] = { "RANDOM", "MAX_SPREAD", "INCREMENTAL" };
		int val = findValueInSet(options, value, 3);
		switch (val) {
		case 0:
			attr->spKDTreeSplitMethod = RANDOM;
			break;
		case 1:
			attr->spKDTreeSplitMethod = MAX_SPREAD;
			break;
		case 2:
			attr->spKDTreeSplitMethod = INCREMENTAL;
			break;
		default:
			printf(CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_STRING;
		}

		return SP_CONFIG_SUCCESS;
	}

	/*spKNN*/
	/*Constraint: positive integers only*/
	if (!strcmp(var, "spKNN")) {
		int val = updateValueInRange(value, 1, INT_MAX);
		if (val) {
			attr->spKNN = val;
			return SP_CONFIG_SUCCESS;
		} else {
			printf(
			CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_INTEGER;
		}
	}

	/*spMinimalGUI*/
	/*Constraint: one of the following: true, false*/
	if (!strcmp(var, "spMinimalGUI")) {
		const char *boolean[] = { "true", "false" };
		int val = findValueInSet(boolean, value, 2);
		if (val == 0) {
			attr->spMinimalGUI = true;
			return SP_CONFIG_SUCCESS;
		}
		if (val == 1) {
			attr->spMinimalGUI = false;
			return SP_CONFIG_SUCCESS;
		}

		printf(CONSTRAINT, filename, line);
		return SP_CONFIG_INVALID_STRING;

	}

	/*spLoggerLevel*/
	/*Constaint: integers in range [1,4]*/
	if (!strcmp(var, "spLoggerLevel")) {
		int val = updateValueInRange(value, 1, 4);
		if (val) {
			attr->spLoggerLevel = val;
			return SP_CONFIG_SUCCESS;
		} else {
			printf(CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_INTEGER;
		}

	}

	/*spLoggerFilename*/
	/*Constraint: None*/
	if (!strcmp(var, "spLoggerFilename")) {
		assignString(attr->spLoggerFilename, value);
		return SP_CONFIG_SUCCESS;
	}

	printf(INVALID_CONFLINE, filename, line);
	return SP_CONFIG_INVALID_STRING;
}

/**
 * This function prints the error message to stdout when a certain must-have
 * parameter is not set in the config file.
 *
 * The message is as follows:
 * 	File: <filename>
 *  Line: <No. of lines in <filename>>
 *  Message: Parameter <parameter> is not set
 *
 *  The function rewinds the file to its start to count the lines in the file -
 *  reminder, the function is called on an error, after which the program is
 *  finished so the file is not supposed to be read continuously after the
 *  call to this function.
 *  The file is not closed in the function.
 *
 *  If config == NULL || filename == NULL || parameter == NULL, than nothing is done.
 *
 */
void printMissing(FILE* config, const char* filename, const char* parameter) {
	if (!config || !filename || !parameter)
		return;
	/** return to the start of the file **/
	fseek(config, SEEK_SET, 0);
	char ch;
	int lines = 1;	//doesnt count the last line

	while (!feof(config)) {

		ch = fgetc(config);
		/** count lines **/
		if (ch == '\n') {
			lines++;
		}
	}
	printf(PARAMETER_NOTSET, filename, lines, parameter);

}

/**
 * This function checks if the non-essential parameters in the configuration file are set,
 * and if not, assigns them the default values as follows:
 *
 * spPCADimension = 20
 * spPCAFilename = "pca.yml"
 * spNumOfFeatures = 100
 * spExtractionMode = true
 * spNumOfSimilarImages = 1
 * spKDTreeSplitMethod = MAX_SPREAD
 * spKNN = 1
 * spMinimalGUI = false
 * spLoggerLevel = 3
 * spLoggerFilename = "stdout"
 *
 *  The function rewinds the file to its start to count the lines in the file -
 *  reminder, the function is called on an error, after which the program is
 *  finished so the file is not supposed to be read continuously after the
 *  call to this function.
 *  The file is not closed in the function.
 *
 */
SP_CONFIG_MSG checkForDefaults(SPConfig attr) {

	/** Integer variables, using macro **/
	checkAndAssign(spPCADimension, 20);
	checkAndAssign(spNumOfFeatures, 100);
	checkAndAssign(spNumOfSimilarImages, 1);
	checkAndAssign(spKDTreeSplitMethod, MAX_SPREAD);
	checkAndAssign(spKNN, 1);
	checkAndAssign(spLoggerLevel, 3);

	/** Special variables **/
	if (!attr->spPCAFilename) {
		attr->spPCAFilename = (char*) malloc(8);
		if (!attr->spPCAFilename) {
			return SP_CONFIG_ALLOC_FAIL;
		}
		strcpy(attr->spPCAFilename, PCAYML);
	}
	if (!attr->spLoggerFilename) {
		attr->spLoggerFilename = (char*) malloc(7);
		if (!attr->spLoggerFilename) {
			return SP_CONFIG_ALLOC_FAIL;
		}
		strcpy(attr->spLoggerFilename, STD);
	}
	if (attr->spExtractionMode < 0)
		attr->spExtractionMode = true;
	return SP_CONFIG_SUCCESS;

}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {

	*msg = SP_CONFIG_SUCCESS;

	/* Variables Initializatgion */
	char line[MAX_LENGTH], *var, *value;
	int lineNum = 0;
	int isCommentBlank = 0;
	SPConfig attr = NULL;
	bool gotError = false;

	FILE* config = fopen(filename, "r");

	/* if file cannot be opened
	 * nothing got allocated yet, can finish without freeing */
	if (!config) {

		/* if default file */
		if (!strcmp(filename, DEFAULT_CONFIG)) {
			printf(DEFAULT_CANT_OPEN);
			*msg = SP_CONFIG_CANNOT_OPEN_FILE;
			return NULL;

		} else { /* if not default file */
			printf(CANT_OPEN, filename);
			*msg = SP_CONFIG_CANNOT_OPEN_FILE;
			return NULL;
		}
	}

	/* memory allocation for config */
	attr = (SPConfig) malloc(sizeof(*attr));
	if (!attr) {
		printf(MEMORY_FAIL1);
		*msg = SP_CONFIG_ALLOC_FAIL;

		gotError = true;
		fseek(config, SEEK_END, 0); 	//so would not get into while at all
	}

	/* dummy initialization of attributes - so can be checked later whether variables are initialized or not*/
	memset(attr, 0, sizeof(*attr));
	attr->spExtractionMode = -1;

	while (fgets(line, MAX_LENGTH, config) != NULL) {
		lineNum++;
		isCommentBlank = 0;

		/* check for line errors */
		checkLine(line, msg, &isCommentBlank, &var, &value);

		/* line is invalid */
		if (*msg != SP_CONFIG_SUCCESS) {

			errorInvalidLine();
			*msg = SP_CONFIG_INVALID_STRING;

			gotError = true;
			break;

		} else if (isCommentBlank) {
			continue;

		} else {
			*msg = assignVarValue(attr, var, value, lineNum, filename);
			if (*msg != SP_CONFIG_SUCCESS) {
				gotError = true;
				break;
			}

		}
	}

	/* check if doesnt have another error already */
	if (!gotError) {
		if (!attr->spImagesDirectory) {
			*msg = SP_CONFIG_MISSING_DIR;
			printMissing(config, filename, IMGDIR);
			gotError = true;

		} else if (!attr->spImagesPrefix) {
			*msg = SP_CONFIG_MISSING_PREFIX;
			printMissing(config, filename, IMGPRE);
			gotError = true;

		} else if (!attr->spImagesSuffix) {
			*msg = SP_CONFIG_MISSING_SUFFIX;
			printMissing(config, filename, IMGSUF);
			gotError = true;

		} else if (!attr->spNumOfImages) {
			*msg = SP_CONFIG_MISSING_NUM_IMAGES;
			printMissing(config, filename, IMGNUM);
			gotError = true;
		}
	}

	fclose(config);
	/* Assign default values if not set */
	if (checkForDefaults(attr) != SP_CONFIG_SUCCESS) {
		printf(MEMORY_FAIL1);
		gotError = true;
	}

	if (gotError) {
		spConfigDestroy(attr);
		return NULL;
	}

	return attr;

}

void spConfigDestroy(SPConfig config) {
	if (!config)
		return;
	free(config->spImagesDirectory);
	free(config->spImagesPrefix);
	free(config->spImagesSuffix);
	free(config->spLoggerFilename);
	free(config->spPCAFilename);
	free(config);
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {
	getterBoolean(spExtractionMode);
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	getterBoolean(spMinimalGUI);
}

int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spKNN);
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spNumOfImages);
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spNumOfSimilarImages);
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spNumOfFeatures);
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spPCADimension);
}

splitMethod spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spKDTreeSplitMethod);
}
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index) {

	if (!imagePath || !config)
		return SP_CONFIG_INVALID_ARGUMENT;
	if (index >= config->spNumOfImages || index < 0)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;

	sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
			config->spImagesPrefix, index, config->spImagesSuffix);

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	if (!pcaPath || !config)
		return SP_CONFIG_INVALID_ARGUMENT;

	sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPCAFilename);

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetSPLoggerFilename(char* loggerPath,
		const SPConfig config) {
	if (!loggerPath || !config)
		return SP_CONFIG_INVALID_ARGUMENT;

	sprintf(loggerPath, "%s", config->spLoggerFilename);

	return SP_CONFIG_SUCCESS;
}
