#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"
#include "SPConfig.h"
#include "SPLogger.h"

#define EPS 0.00000000001

typedef struct kd_array_t* SPKDArray;

/* Initializes the kd-array with the data given by arr. Complexity: O(d*nlogn) */
SPKDArray spKDArrayCreate(SPConfig attr, SPPoint* arr, int size, SP_LOGGER_MSG *log_msg,
		SP_CONFIG_MSG *conf_msg);

/* Returns two kd-arrays (kdLeft,kdRight) such that the first /ceil[n/2] are in left, rest in right */
int spKDArraySplit(SPKDArray kdarr, int coor, struct kd_array_t** KDpntr1, struct kd_array_t** KDpntr2, SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg);

void printKDArrayMatrix(SPKDArray kd);

#endif /* KDARRAY_H_ */
