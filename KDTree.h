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
 * checks whether a node in KD-Tree is a leaf or not
 *
 * @param node - the checked node
 * @return true iff node is a leaf
 */
bool isLeaf(SPKDTreeNode node);


/**
 * returns the index of the node
 *
 * @param node - the checked node
 * @return node's index, -1 on failure
 */
int getIndex(SPKDTreeNode node);

/**
 * returns the distance of a node from a given point
 *
 * @param node - the checked node
 * @param point - the point for the distance to be calculated by
 * @return distance from the point, -1 for failure
 */
double getDistFromPoint(SPKDTreeNode node, SPPoint point);

/**
 * returns the dimension of the node
 *
 * @param node - the checked node
 * @return node's dimension, -1 for failure
 */
int getDim(SPKDTreeNode node);

/**
 * returns the value of the node
 *
 * @param node - the checked node
 * @return node's value
 */
int getVal(SPKDTreeNode node);

/**
 * get the root node of the left child (subtree)
 *
 * @param node - the root node
 * @return node's left child, NULL if !node
 */
SPKDTreeNode getLeftSubtree(SPKDTreeNode node);

/**
 * get the root node of the right child (subtree)
 *
 * @param node - the root node
 * @return node's right child, NULL if !node
 */
SPKDTreeNode getRightSubtree(SPKDTreeNode node);

/**
 * calculates whether two double variables are equal.
 * to avoid floating point fail, the function uses EPS
 * constant.
 *
 * @param a - first number
 * @param b - second number
 * @return true iff the numbers are equal.
 */
bool doubleEquals(double a, double b);

#endif /* KDTREE_H_ */
