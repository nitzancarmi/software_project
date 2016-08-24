#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include "macros.h"
#include <stdio.h>

/** Type for defining the KDArray */
typedef struct kd_array_t* SPKDArray;

/* Initializes the kd-array with the data given by arr. Complexity: O(d*nlogn) */

/**
 * 	Creating and allocating a new SPKDArray (KD-Array) from an array of SPPoints.
 * 	The KD-Array is maintained using an indices matrix A where the i'th row is the
 * 	indices of the points in pointsArray sorted according to their i'th dimension.
 * 	The array pointsArray is copied during the operation.
 *
 * 	The complexity of the function is O[(config->spCADimension) * array_size * log(array_size)]
 *
 *  @param config		Configuration object that contains information about the function execution
 *  					(e.g. spCADimension)
 *  @param pointsArray  The SPPoints array from which the KD-Array will be constructed.
 *  @param array_size	Size of pointsArray.
 *  @param conf_msg		Pointer to config_msg that can be accessed after execution to know if an error
 *  					has occurred when config was accessed.
 *  @param log_msg		Pointer to log_msg that can be accessed after execution to know if an error
 *  					has occurred when SPLogger was used.
 *  @return
 *  A new SPKDArray in case of a success.
 *  NULL in case one of the following occurred:
 *  	- pointsArray == NULL || config == NULL || conf_msg == NULL || log_msg == NULL
 *  	- Memory allocation failure
 *  	- SPConfig access error
 */
SPKDArray spKDArrayCreate(SPConfig config, SPPoint* pointsArray, int array_size,
		SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg);

/* Returns two kd-arrays (kdLeft,kdRight) such that the first /ceil[n/2] are in left, rest in right */


/**
 * 	Splitting an existing KD-Array to two new allocated KD-Arrays - kdLeft & kdRight such that
 * 	the first ceiling[n/2] points with respect to the coordinate coor are in kdLeftg, and the rest of the
 * 	points are in kdRight.
 * 	The KDArrays are then stored in *KDpntrLeft and *KDpntrRight respectively.
 *
 * 	The complexity of the function is O[(config->spCADimension) * n], where n is the number
 * 	of points in kdarr.
 *
 *  @param kdarr		The KD-Array to be split.
 *  @param coor			The coordinate to split the KD-Array by.
 *  @param KDpntrLeft	        Address of the pointer to kdLeft.
 *  @param KDpntrRight	        Address of the pointer to kdRight.
 *  @param log_msg		Pointer to log_msg that can be accessed after execution to know if an error
 *  					has occurred when SPLogger was used.
 *  @param conf_msg		Pointer to config_msg that can be accessed after execution to know if an error
 *  					has occurred when config was accessed.
 *  @return
 *  The median value of the splitting dimension in case of a success,
 *  (-1) in case one of the following occurred:
 *  	- kd == NULL || config == NULL || KDpntr1 == NULL || KDpntr2 == NULL
 *		- conf_msg == NULL || log_msg == NULL
 *		- KDarray is invalid
 *  	- Memory allocation failure
 *  	- SPConfig access error
 */
int spKDArraySplit(SPKDArray kdarr, int coor, SPKDArray* KDpntrLeft,
		SPKDArray* KDpntrRight, SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg);

/**
 * Deallocates an existing SPKDArray.
 *
 * @param KDAr Target KD-Array to be deallocated. If KDAr is NULL nothing
 * will be done.
 */
void SPKDArrayDestroy(SPKDArray KDAr);

/*
 * Returns a copy of the SPPoint Array (with copies of the points inside) included in the KD-Array.
 * @param KD - the relevant KD-array
 * @return NULL if:
 * 		- KD == NULL
 * 		- Memory Allocation Error
 *
 * Otherwise the copy of the array.
 */
SPPoint* getKDPointArray(SPKDArray KD);

/*
 * Returns a copy of the first point of the SPPoint Array in the KD-Array.
 * Useful for single-pointed KD arrays (KDTree nodes).
 * @param KD - the relevant KD-array
 * @return NULL if:
 * 		- KD == NULL
 * 		- Memory Allocation Error
 *
 * Otherwise the copy of the point.
 */
SPPoint getKDOnlyPoint(SPKDArray kd);

/*
 * Returns a copy of the indices matrix that makes up the KDArray.
 * This function is useful for Unit Tests only.
 *
 * @param kd - the relevant KD-array
 * @return NULL if:
 * 		- KD == NULL
 * 		- Memory Allocation Error
 *
 * Otherwise the copy of the matrix.
 */
int** getKDMat(SPKDArray kd);


/*
 * Returns the number of columns in the indices matrix of the KD-Array.
 * Essentially this number represents the number of points included in the SPPoint array of the KDArray.
 *
 * @param kd - the relevant KD-array
 * @return -1 if KD == NULL, otherwise the number of columns.
 */
int getKDCols(SPKDArray kd);

/*
 * Returns the number of rows in the indices matrix of the KD-Array.
 * Essentially this number represents the dimension the KD-Array is defined with.
 *
 * @param KD - the relevant KD-array
 * @return -1 if KD == NULL, otherwise the number of rows.
 */
int getKDRows(SPKDArray kd);

#endif /* KDARRAY_H_ */
