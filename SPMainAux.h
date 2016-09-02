#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "KDTree.h"
#include "SPExtractor.h"
#include "macros.h"


/**
 * setup the needed resources for the program to run, using 
 * given config parameters.
 *
 * @param config - configuration file
 * @param kdtree - uninitialized pointer for a kdtree (to be initialized inside)
 * @param log_msg - logger message to be updated (if needed)
 * @param conf_msg - config message to be updated (if needed)
 */
int Setup(SPConfig config, SPKDTreeNode* kdtree, SP_LOGGER_MSG* log_msg,
		SP_CONFIG_MSG* conf_msg);
/**
 * frees the resources created at the setup stage
 * 
 * @param config - configuration struct
 * @param kdtree - built kdtree based on given path to images folder
 * @param similar_images - array of indices for the closest images
 *
 * @return 0 for success, 1 for Error
 */
void cleanGlobalResources(SPConfig config, SPKDTreeNode kdtree,
		int* similar_images);

/**
 * frees all query related resources
 * 
 * @param q_features - point array, used as representation of the query image
 * @param q_numOfFeats - point array size
 * @param q_path - path to the given query image
 *
 * @return None
 */
void cleanTempResources(SPPoint** q_features, int q_numOfFeats, char* q_path);

/**
 * starts a static logger to be later used in the program
 *
 * @param config = configuration file, containing the image path
 * @param log_msg = logger message to be updated during runtime
 *
 * @return true iff success
*/
bool initializeSPLogger(SPConfig config, SP_LOGGER_MSG* log_msg);

/**
 * parse the command line arguments, or return an error in case of
 * invalid args insertion
 *
 * @param argc - number of given arguments
 * @param argv - array of strings given in command line
 * @param conf_msg = config error message to be updated during runtime
 *
 * @return 0 in case of success, 1 otherwise
*/
int argParse(int argc, char* argv[], SPConfig* _config, SP_CONFIG_MSG* conf_msg);

#endif /* SPMAINAUX_H_ */
