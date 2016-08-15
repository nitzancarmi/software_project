#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "KDTree.h"
#include "SPExtractor.h"
#include "macros.h"

int Setup(SPConfig config, SPKDTreeNode* kdtree, SP_LOGGER_MSG* log_msg,
		SP_CONFIG_MSG* conf_msg);

void cleanGlobalResources(SPConfig config, SPKDTreeNode kdtree,
		int* similar_images);

void cleanTempResources(SPPoint** q_features, int q_numOfFeats, char* q_path);

bool initializeSPLogger(SPConfig config, SP_LOGGER_MSG* log_msg);


#endif /* SPMAINAUX_H_ */
