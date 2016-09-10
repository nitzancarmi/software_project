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

int Setup(SPConfig config, SPKDTreeNode* kdtree, SP_LOGGER_MSG* log_msg,
		SP_CONFIG_MSG* conf_msg) {

	int all_points_size = -1;

	/* Create a large SPPoint array from all the extractable features from
	 * all the .feat files in the defined directory */
	SPPoint* all_points = extractImagesFeatures(&all_points_size, config,
			log_msg, conf_msg);
	if (!all_points)
		return 1;

	/* Create a SPKDArray from the SPPoints extracted */
	SPKDArray kdarray = spKDArrayCreate(config, all_points, all_points_size,
			log_msg, conf_msg);
	if (!kdarray) {
		spPointArrayDestroy(all_points, all_points_size);
		return 1;
	}

	/* Create an SPKDTree from the SPKDArray */
	*kdtree = spKDTreeCreate(kdarray, config, conf_msg, log_msg);

	/* Free unnecesary objects */
	spPointArrayDestroy(all_points, all_points_size);
	SPKDArrayDestroy(kdarray);

	return *kdtree ? 0 : 1;
}

void cleanGlobalResources(SPConfig config, SPKDTreeNode kdtree) {
	spLoggerDestroy();
	if (config)
		spConfigDestroy(config);

	if (kdtree)
		spKDTreeDestroy(kdtree);
}

void cleanTempResources(SPPoint** q_features, int q_numOfFeats, char* q_path,
		int** similar_images) {

	spPointArrayDestroy(*q_features, q_numOfFeats);
	memset(&q_path[0], '\0', strlen(q_path)); //Prepare q_path for the next query
	*q_features = NULL;
	free(*similar_images);
	*similar_images = NULL;
}

bool initializeSPLogger(SPConfig config, SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg) {

	char loggerPath[1024] = { '\0' };
	spConfigGetSPLoggerFilename(loggerPath, config);

	/* Create the logger based on filename and level got in configuration file */
	*log_msg = spLoggerCreate(!strcmp(loggerPath, STD) ? NULL : loggerPath,
			spConfigGetSPLoggerLevel(config,conf_msg));

	if (*log_msg != SP_LOGGER_SUCCESS || *conf_msg != SP_CONFIG_SUCCESS) {
		return true;
	}

	*log_msg = spLoggerPrintDebug(FIRST_MSG, __FILE__, __func__, __LINE__);

	if (*log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, *log_msg);
		return true;
	}

	return false;
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
