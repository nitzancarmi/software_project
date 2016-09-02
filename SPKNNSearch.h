#ifndef SPKNNSEARCH_H_
#define SPKNNSEARCH_H_

/**
 * 	This function does K-nearest-neighbors search on a KD-Tree for a query of a SPPoint,
 *  using a bounded priority queue.
 *
 *  @param kdtree   	The KD-Tree to be searched on.
 *  @param point		Query for the search
 *  @param config		Configuration object that contains information about the function execution
 *  					(e.g. spKNN)
 *  @return
 *  An integer array of the k nearest neighbors' indices.
 *  NULL in case one of the following occurred:
 *  	- kdarray == NULL || config == NULL || point == NULL
 *  	- Memory allocation failure
 *  	- SPConfig access error
 */
int* findKNearestNeighbors(SPKDTreeNode kdtree, SPPoint point, SPConfig config);

/**
 * This function finds the k nearest images to a given query image.
 * The query image is represented as an array of points (features),
 * while for each point in the array, the function find its k nearest 
 * neighbors in the pre-built given kd-tree (using findKNearestNeighbors).
 *
 * @param kdtree       The KD-Tree to be searched on
 * @param config       The config file
 * @param q_features   point array, representation of the query image
 * @param log_msg      logger msg
 * @param conf_msg     config msg
 *
 * @return array of the indices of the images which are most similar ("closest")
 *         to the query image
 *         in case of an error, return NULL (with updated logger/config error message)
 */ 
int* getClosestImages(SPKDTreeNode kdtree, SPConfig config, SPPoint* q_features,
		int q_numOfFeats, SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg);

#endif /* SPKNNSEARCH_H_ */
