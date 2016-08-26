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

/**
* check whether a kd tree node is a leaf or not
* 
* @param node - the checked node
* @return true iff node is a leaf in the tree
*/
bool isLeaf(SPKDTreeNode node);

/**
* getter for node's index
*
* @param node - the checked node
* @return node's index, -1 if !node
*/
int getIndex(SPKDTreeNode node);

/**
* getter for node's distance from query point
*
* @param node - the checked node
* @param point - the query point
* @note: for convenience, the distance calculated is the square of th original distance
*
* @return node's distance, -1 if !node or !point
*/
double getDistFromPoint(SPKDTreeNode node, SPPoint point);

/**
* getter for node's dinmension
*
* @param node - the checked node
* @return node's dimension, -1 if !node
*/
int getDim(SPKDTreeNode node);

/**
* getter for node's value
*
* @param node - the checked node
* @return node's value, -1 if !node
*/
int getVal(SPKDTreeNode node);

/**
* getter for node's left subtree
*
* @param node - the checked node
* @return pointer to the root of the node's left subtree. NULL if !node
*/
SPKDTreeNode getLeftSubtree(SPKDTreeNode node);

/**
* getter for node's right subtree
*
* @param node - the checked node
* @return pointer to the root of the node's right subtree. NULL if !node
*/
SPKDTreeNode getRightSubtree(SPKDTreeNode node);

/**
* check for equality between two doubles
*
* @param a,b - two doubles to be compared
* @return true iff |a-b| < EPS
*/
bool doubleEquals(double a, double b);

#endif /* KDTREE_H_ */
