#include "SPConfig.h"

/************************************* MACROS *******************************************************/
#define ignoreSpaces(x)						while(isspace(*x)){x++;}

#define errorInvalidLine() 					printf(INVALID_CONFLINE, filename, lineNum)

#define assignString(configAttribute,xVal)	configAttribute = (char*) malloc(strlen(xVal)); \
											if(!configAttribute){							\
												return SP_CONFIG_ALLOC_FAIL;				\
											}												\
											strcpy(configAttribute,xVal)

#define checkAndAssign(property,x)			if(!attr->property)								\
												attr->property = x;

#define getter(property)					assert(msg);									\
	*msg = config ? SP_CONFIG_SUCCESS : SP_CONFIG_INVALID_ARGUMENT;							\
	return config ? config->property : false
/****************************************************************************************************/

/*** Struct Definition ***/
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
/****************************/

void checkLine(char* line, SP_CONFIG_MSG* msg, int* isCommentBlank,
		char** varReturn, char** valueReturn) {

	/* pass over spaces to next character*/
	ignoreSpaces(line)

	/* if end of line (only whitespaces or blank) or comment, ignore line */
	if (*line == '\0' || *line == '#') {
		*msg = SP_CONFIG_SUCCESS;
		*isCommentBlank = 1;
		return;
	}

	/* save current position on line */
	char *var = line, *value = line, *tmp;

	/* go to '=' position of value */
	while (*value != '=' && *value != '\0')
		value++;

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
	while (!isspace(*tmp))
		tmp++;
	*tmp = '\0';

	tmp = value;
	while (!isspace(*tmp))
		tmp++;
	*tmp = '\0';

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

int findValueInSet(const char *set[], char* value, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (!strcmp(set[i], value))
			return i;
	}
	return -1;
}

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
		int val = updateValueInRange(value, 2, 28);				//TODO CHANGE TO 10
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
		if (val == -1) {
			printf(CONSTRAINT, filename, line);
			return SP_CONFIG_INVALID_STRING;
		}
		if (!strcmp(options[val], "RANDOM")) {
			attr->spKDTreeSplitMethod = RANDOM;
		} else if (!strcmp(options[val], "MAX_SPREAD")) {
			attr->spKDTreeSplitMethod = MAX_SPREAD;
		} else if (!strcmp(options[val], "INCREMENTAL")) {
			attr->spKDTreeSplitMethod = INCREMENTAL;
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

void printMissing(FILE* config, const char* filename, const char* parameter) {
	fseek(config, SEEK_SET, 0);
	char ch;
	int lines = 0;
	while (!feof(config)) {
		ch = fgetc(config);
		if (ch == '\n') {
			lines++;
		}
	}
	printf(PARAMETER_NOTSET, filename, lines, parameter);

}

void checkForDefaults(SPConfig attr) {
	/*
	 int spPCADimension = 20;
	 char* spPCAFilename = "pca.yml";
	 int spNumOfFeatures = 100;
	 bool spExtractionMode = true;
	 int spNumOfSimilarImages = 1;
	 splitMethod spKDTreeSplitMethod = MAX_SPREAD;
	 int spKNN = 1;
	 bool spMinimalGUI = false;
	 int spLoggerLevel = 3;
	 char* spLoggerFilename = "stdout";*/

	checkAndAssign(spPCADimension, 20);
	checkAndAssign(spNumOfFeatures, 100);
	checkAndAssign(spExtractionMode, true);
	checkAndAssign(spNumOfSimilarImages, 1);
	checkAndAssign(spKDTreeSplitMethod, MAX_SPREAD);
	checkAndAssign(spKNN, 1);
	checkAndAssign(spLoggerLevel, 3);
	if (!attr->spPCAFilename) {
		attr->spPCAFilename = (char*) malloc(8);
		strcpy(attr->spPCAFilename, "pca.yml");
	}
	if (!attr->spLoggerFilename) {
		attr->spLoggerFilename = (char*) malloc(7);
		strcpy(attr->spLoggerFilename, "stdout");
	}

}

void printAttributes(SPConfig attr) { /** DELETE **/
	fprintf(stdout, "=== Attributes: ===\n");
	fprintf(stdout, "%s\t= %s\n", "spImagesDirectory", attr->spImagesDirectory);
	fprintf(stdout, "%s\t\t= %s\n", "spImagesPrefix", attr->spImagesPrefix);
	fprintf(stdout, "%s\t\t= %s\n", "spImagesSuffix", attr->spImagesSuffix);
	fprintf(stdout, "%s\t\t= %d\n", "spNumOfImages", attr->spNumOfImages);
	fprintf(stdout, "%s\t\t= %d\n", "spPCADimension", attr->spPCADimension);
	fprintf(stdout, "%s\t\t= %s\n", "spPCAFilename", attr->spPCAFilename);
	fprintf(stdout, "%s\t\t= %d\n", "spNumOfFeatures", attr->spNumOfFeatures);
	fprintf(stdout, "%s\t= %d\n", "spExtractionMode", attr->spExtractionMode);
	fprintf(stdout, "%s\t= %d\n", "spNumOfSimilarImages",
			attr->spNumOfSimilarImages);
	fprintf(stdout, "%s\t= %d\n", "spKDTreeSplitMethod",
			attr->spKDTreeSplitMethod);
	fprintf(stdout, "%s\t\t\t= %d\n", "spKNN", attr->spKNN);
	fprintf(stdout, "%s\t\t= %d\n", "spMinimalGUI", attr->spMinimalGUI);
	fprintf(stdout, "%s\t\t= %d\n", "spLoggerLevel", attr->spLoggerLevel);
	fprintf(stdout, "%s\t= %s\n", "spLoggerFilename", attr->spLoggerFilename);
	fprintf(stdout, "===================\n");
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {

	assert(msg);

	/* vars init */
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
		if (!strcmp(filename, "spcbir.config")) {
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

	/* NULLization of atrributes */
	memset(attr, 0, sizeof(*attr));

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

			/* structure of line is OK (HAVENT checked existence of vars or constraints yet) */

			/* Line assignment - check if var exists and assign value to it.
			 * If has some kind of error - terminate.
			 */
			*msg = assignVarValue(attr, var, value, lineNum, filename);

			if (*msg != SP_CONFIG_SUCCESS) {
//				printf("line %d\n", lineNum);
//				printf("msg = %d", *msg);
				gotError = true;
				break;
			}

		}
	}

	/* check if doesnt have another error already */
	if (!gotError) {
		if (!attr->spImagesDirectory) {
			*msg = SP_CONFIG_MISSING_DIR;
			printMissing(config, filename, "spImagesDirectory");
			gotError = true;

		} else if (!attr->spImagesPrefix) {
			*msg = SP_CONFIG_MISSING_PREFIX;
			printMissing(config, filename, "spImagesPrefix");
			gotError = true;

		} else if (!attr->spImagesSuffix) {
			*msg = SP_CONFIG_MISSING_SUFFIX;
			printMissing(config, filename, "spImagesSuffix");
			gotError = true;

		} else if (!attr->spNumOfImages) {
			*msg = SP_CONFIG_MISSING_NUM_IMAGES;
			printMissing(config, filename, "SP_CONFIG_MISSING_NUM_IMAGES");
			gotError = true;
		}
	}
	if (gotError) {
		printAttributes(attr);
		spConfigDestroy(attr);
		fclose(config);
		return NULL;
	} else {
		checkForDefaults(attr);
		printAttributes(attr);
		return attr;
	}

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
	getter(spExtractionMode);
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spMinimalGUI);
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	getter(spNumOfImages);
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

