#include "KDArray.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPPoint.h"
#include "math.h"

#define arrayMallocFail(array,j)	 if(!array[j]){ \
					for(j--;j>=0;j--){ \
					free(array[j]); \
					}\
					free(array); \
					return NULL
//TODO add logger message

#define EPS 0.00000000001

/*** Struct Definition ***/
struct kd_array_t {

	SPPoint* pointArray;
	int** mat;
};
/****************************/

struct pointAxis_t {
	SPPoint* pnt;
	int axis;
	int indexInPntArray;
};
typedef struct pointAxis_t* PointAxis;

SPPoint* copyPointArray(SPPoint* arr, int size) {
	SPPoint* ret = (SPPoint*) malloc(sizeof(SPPoint) * size);
	if (!ret) {
		return NULL;
	}
	for (int i = 0; i < size; i++) {
		ret[i] = spPointCopy(arr[i]);

		/* frees in case of inner malloc fail */
		arrayMallocFail(ret,i);
	}
		return ret;
	}
}

int pointComparator(const void *p, const void *q) {
	PointAxis p1 = *(const PointAxis *) p;
	PointAxis p2 = *(const PointAxis *) q;

	double p1ax = spPointGetAxisCoor(p1->pnt, p1->axis);
	double p2ax = spPointGetAxisCoor(p2->pnt, p2->axis);

	if (fabs(p1ax - p2ax) < EPS)
		return 0;
	if (p1ax - p2ax < 0)
		return -1;
	return 1;

}

/* assumes all pts same dimension */
int* sortByAxis(const SPPoint* pts, int size, int axis, const SPConfig config,
		SPLogger logger, SP_LOGGER_MSG* msg) {
	if (!pts || axis < 0 || !config || !logger) {
		spLoggerPrintError("Invalid Argument to sortByAxis", __FILE__, __func__,
		__LINE__);
		return NULL;
	}
	PointAxis* ptsAx = (PointAxis*) malloc(size * sizeof(*ptsAx));

	if (!ptsAx)
		//TODO add logger message
		return NULL;

	for (int i = 0; i < size; i++) {
		ptsAx[i] = (PointAxis) malloc(sizeof(*ptsAx[i]));

		/*Malloc fail */
		arrayMallocFail(ptsAx,i);

		ptsAx[i]->pnt = pts[i];
		ptsAx[i]->axis = axis;
		ptsAx[i]->indexInPntArray = i;

		}

	}
	qsort(ptsAx, size, sizeof(*ptsAx), pointComparator);
	int* ret = (int*) malloc(size * sizeof(int));
	if (!ret)
		//TODO add logger message
		return NULL;

	for (int i = 0; i < size; i++)
		ret[i] = ptsAx[i]->indexInPntArray;

	return ret;
}

void SPKDArrayDestroy(SPKDArray kd,
		      int arrsize,
                      int rows) {
    int i;
    /* frees kd->pointArray */
    for(i=0;i<arrsize;i++)
        spPointDestroy(kd->pointArray[i]);

    /* Frees kd->Mat */
    for(i=0; i<rows; i++)
        free(kd->mat[i]);

    free(kd);
}

SPKDArray init(SPConfig attr,
                SPPoint *arr,
                int size,
                SPLogger logger,
                SP_LOGGER_MSG *log_msg,
		SP_CONFIG_MSG *conf_msg)
{
    
    if(!arr || size<1)
	//TODO add logger message
        return NULL;

    /*create new struct SPKDArray*/
    SPKDArray kd = (SPKDArray)malloc(sizeof(*kd));
    if(!kd) {
	//TODO add logger message
        return NULL;
    } 

    int dims = spConfigGetPCADim(attr, conf_msg);
   
    /*copy array into struct*/
    kd->pointArray = copyPointArray(arr, size);
    if(!kd->pointArray){
        //TODO add logger message
	SPKDArrayDestroy(kd, size, dims);
	return NULL;
    }

    /*create initialized matrix of size dims X size */
    if(dims<0){
        //TODO add logger message
	SPKDArrayDestroy(kd, size, dims);
	return NULL;
    }

    int axis;
    int *M[dims];
    memset(M, 0, sizeof(M));
    for(axis=0; axis<dims; axis++) {
        M[axis] = sortByAxis(arr, 
                             size,
                             axis,
                             attr,
                             logger,
                             log_msg); 
        if(!M[axis]) {
            //TODO add logger message
	    SPKDArrayDestroy(kd, size, dims);
	    return NULL;
	}
    } 
    return kd;
}

int Split(SPKDArray kd, int coor) {
    
    return 0;
}

