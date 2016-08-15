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


//TODO DOCO
int* getClosestImages(SPKDTreeNode kdtree, SPConfig config, SPPoint* q_features,
		int q_numOfFeats, SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg);

#endif /* SPKNNSEARCH_H_ */
