#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include <stdio.h>

#define EPS 0.00000000001
#define ERR_ARGS	"%s - at file: %s, line: %s, func: %s"
#define INVALID 	"Invalid Argument Error"
#define CONFIG_ERR 	"SPConfig Error"
#define LOGGER_ERR 	"SPLogger Error"

#define InvalidError()	if(spLoggerPrintError(INVALID,__FILE__,__func__,__LINE__)!= SP_LOGGER_SUCCESS)	\
							fprintf(stderr,ERR_ARGS,LOGGER_ERR,__FILE__,__LINE__,__func__)


#define returnIfConfigMsg(ret) if (*conf_msg != SP_CONFIG_SUCCESS) {	\
									InvalidError();\
									return ret;	\
								}
typedef struct kd_array_t* SPKDArray;

/* Initializes the kd-array with the data given by arr. Complexity: O(d*nlogn) */
SPKDArray spKDArrayCreate(SPConfig attr, SPPoint* arr, int size,
		SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg);

/* Returns two kd-arrays (kdLeft,kdRight) such that the first /ceil[n/2] are in left, rest in right */
int spKDArraySplit(SPKDArray kdarr, int coor, SPKDArray* KDpntr1,
		SPKDArray* KDpntr2, SP_LOGGER_MSG *log_msg,
		SP_CONFIG_MSG *conf_msg);

/* TODO Documentation
*
*/
void SPKDArrayDestroy(SPKDArray kd);
>>>>>>> minor fixes after memory allocation basic checks
=======

>>>>>>> kdarray_unit_test: finished basic test
void printKDArrayMatrix(SPKDArray kd);


/*
* TODO DOCO!!!! it does malloc
*/
SPPoint* getKDPointArray(SPKDArray kd);
int** getKDMat(SPKDArray kd);
int getKDCols(SPKDArray kd);
int getKDRows(SPKDArray kd);

#endif /* KDARRAY_H_ */
