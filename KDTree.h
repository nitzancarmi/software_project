#ifndef KDTREE_H_
#define KDTREE_H_

#include "KDArray.h"

/** Type for defining the KDTree */
typedef struct kd_tree_node_t* SPKDTreeNode;

/**
 * 	Creating and allocating a new SPKDTreeNode (KD-Tree) recursively from an existing SPKDArray (KD-Array).
 * 	The splitting of the KD-Array are done using properties defined in the configuration object.
 * 	The leaves of the tree will contains the points.
 *
 *  @param KDArray   	The KD-Array from which the KD-Tree will be constructed.
 *  @param config		Configuration object that contains information about the function execution
 *  					(e.g. splitMethod)
 *  @param conf_msg		Pointer to config_msg that can be accessed after execution to know if an error
 *  					has occurred when config was accessed.
 *  @param log_msg		Pointer to log_msg that can be accessed after execution to know if an error
 *  					has occurred when SPLogger was used.
 *  @return
 *  A new SPKDTreeNode in case of a success.
 *  NULL in case one of the following occurred:
 *  	- kdarray == NULL || config == NULL || conf_msg == NULL || log_msg == NULL
 *  	- Memory allocation failure
 *  	- SPConfig access error
 */
SPKDTreeNode spKDTreeCreate(SPKDArray KDArray,const SPConfig config,SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg);

/**
 * spKDTreeDestroy: Deallocates an existing KDTreeNode and all its descendants.
 *
 * @param KDTree Target KDTree to be deallocated. If KDTree is NULL nothing
 * will be done.
 */
void spKDTreeDestroy(SPKDTreeNode KDTree);

#endif /* KDTREE_H_ */
