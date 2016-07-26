#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"
#include "SPConfig.h"
#include "SPLogger.h"

typedef struct kd_array_t* SPKDArray;

/* Initializes the kd-array with the data given by arr. Complexity: O(d*nlogn) */
SPKDArray init(SPConfig attr,
               SPPoint* arr,
               int size,
               SPLogger logger,
               SP_LOGGER_MSG *log_msg,
	       SP_CONFIG_MSG *conf_msg);

/* Returns two kd-arrays (kdLeft,kdRight) such that the first /ceil[n/2] are in left, rest in right */
int Split(SPKDArray kdarr,int coor);


#endif /* KDARRAY_H_ */
