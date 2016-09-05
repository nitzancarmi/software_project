#include "SPMainAux.h"

int argParse(int argc, char* argv[], SPConfig* _config, SP_CONFIG_MSG* conf_msg) {
	SPConfig config;
	switch (argc) {
	case 1:
        //default config file
		config = spConfigCreate(DEFAULT_CONFIG, conf_msg);
		if (!config)
			return 1;
		break;

	case 3:
        //user defined config file 
		if (strcmp(argv[1], C_ARG)) {
			printf(INVALID_COMLINE);
			return 1;
		}
		config = spConfigCreate(argv[2], conf_msg);
		if (!config)
			return 1;
		break;

	default:
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

	SPPoint* all_points = extractImagesFeatures(&all_points_size, config,
			log_msg, conf_msg);
	if (!all_points)
		return 1;

	SPKDArray kdarray = spKDArrayCreate(config, all_points, all_points_size,
			log_msg, conf_msg);
	if (!kdarray) {
		spPointArrayDestroy(all_points, all_points_size);
		return 1;
	}

	*kdtree = spKDTreeCreate(kdarray, config, conf_msg, log_msg);
	spPointArrayDestroy(all_points, all_points_size);
	SPKDArrayDestroy(kdarray);

	return *kdtree ? 0 : 1;
}

void cleanGlobalResources(SPConfig config, SPKDTreeNode kdtree,
		int* similar_images) {
	spLoggerDestroy();
	if (config)
		spConfigDestroy(config);

	if (kdtree)
		spKDTreeDestroy(kdtree);
	free(similar_images);
}

void cleanTempResources(SPPoint** q_features, int q_numOfFeats, char* q_path) {
	spPointArrayDestroy(*q_features, q_numOfFeats);
	memset(&q_path[0], '\0', strlen(q_path));
	*q_features = NULL;
}

bool initializeSPLogger(SPConfig config, SP_LOGGER_MSG* log_msg) {
	char loggerPath[1024] = { '\0' };
	spConfigGetSPLoggerFilename(loggerPath, config);
	*log_msg = spLoggerCreate(!strcmp(loggerPath, STD) ? NULL : loggerPath,
			SP_LOGGER_WARNING_ERROR_LEVEL);
	if (*log_msg != SP_LOGGER_SUCCESS) {
		return true;
	}
	*log_msg = spLoggerPrintDebug(FIRST_MSG, __FILE__, __func__, __LINE__);
	if (*log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, *log_msg);
		return true;
	}
	return false;
}
