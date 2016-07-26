#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"


typedef struct kd_array_t* SPKDArray;

/* Initializes the kd-array with the data given by arr. Complexity: O(d*nlogn) */
SPKDArray init(SPPoint* arr, int size);

/* Returns two kd-arrays (kdLeft,kdRight) such that the first /ceil[n/2] are in left, rest in right */
KDArray* Split(SPKDArray kdarr,init coor);


#endif /* KDARRAY_H_ */
