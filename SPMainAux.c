#include "SPMainAux.h"
#include <sys/stat.h>
#include <unistd.h>

int argParse(int argc, char* argv[], SPConfig* _config, SP_CONFIG_MSG* conf_msg) {
	SPConfig config;
	/* Based on the number of arguments, deciding which state  */
	switch (argc) {
	case 1:
		//default config file
		config = spConfigCreate(DEFAULT_CONFIG, conf_msg);
		if (!config)
			return 1;
		break;

	case 3:
		//user defined config file
		if (strcmp(argv[1], C_ARG)) {	//if second arguemnt is not "-c"
			printf(INVALID_COMLINE);
			return 1;
		}
		config = spConfigCreate(argv[2], conf_msg);	//create config file from the third
		if (!config)
			return 1;
		break;

	default:	//any other state is not permitted
		config = NULL;
		printf(INVALID_COMLINE);
		return 1;
	}
	*_config = config;
	return OK;
}

int Setup(SPConfig config, SPKDTreeNode* kdtree) {

	int all_points_size = -1;
        declareLogMsg();

	/* Create a large SPPoint array from all the extractable features from
	 * all the .feat files in the defined directory */
        printInfo("Start Building a KD Tree");
        printInfo("Importing Image features from files");
	SPPoint* all_points = extractImagesFeatures(&all_points_size, config);
	if (!all_points) {
                printError("Failed Extracting Features from image files");
		return 1;
        }

        printInfo("Constructing KD Array out of the features");
	SPKDArray kdarray = spKDArrayCreate(config, all_points, all_points_size);
	if (!kdarray) {
                printError("Failed Constructing KD Array");
		spPointArrayDestroy(all_points, all_points_size);
		return 1;
	}

	/* Create an SPKDTree from the SPKDArray */
        printInfo("Constructing KD Tree using the KD Array");
	*kdtree = spKDTreeCreate(kdarray, config);
	spPointArrayDestroy(all_points, all_points_size);
	SPKDArrayDestroy(kdarray);

	return *kdtree ? 0 : 1;
}

void cleanGlobalResources(SPConfig config, SPKDTreeNode kdtree) {
        declareLogMsg();
	if (config) {
                printInfo("Destroying Configuration parameters struct");
		spConfigDestroy(config);
        }
	if (kdtree) {
                printInfo("Destroying KD-Tree");
		spKDTreeDestroy(kdtree);
        }
        printInfo("Destroying Logger");
	spLoggerDestroy();
}

void cleanTempResources(SPPoint** q_features, int q_numOfFeats, char* q_path, int** similar_images) {
        declareLogMsg();
        printInfo("Cleaning query-oriented resources");
	spPointArrayDestroy(*q_features, q_numOfFeats);
	memset(&q_path[0], '\0', strlen(q_path)); //Prepare q_path for the next query
	*q_features = NULL;
	free(*similar_images);
	*similar_images = NULL;
}

int initializeSPLogger(SPConfig config, SP_LOGGER_MSG* log_msg) {
        declareConfMsg();
	char loggerPath[1024] = { '\0' };
	if(spConfigGetSPLoggerFilename(loggerPath, config) != SP_CONFIG_SUCCESS)
            return 1;
	/* Create the logger based on filename and level got in configuration file */
	*log_msg = spLoggerCreate(!strcmp(loggerPath, STD) ? NULL : loggerPath,
			spConfigGetSPLoggerLevel(config,conf_msg));
	if (*log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, *log_msg);
		return 1;
	}
        char msg[1024] = {'\0'};
        sprintf(msg, "Logger opened successfully in file: %s", loggerPath);
        printInfo(msg); 
	return 0;
}

bool isValidFile(char* path) {
	struct stat s;
	stat(path, &s);
	//		 path exists			path describes a file
	if (access(path, F_OK) == -1 || !S_ISREG(s.st_mode)) {
		return false;
	}
	return true;
}
