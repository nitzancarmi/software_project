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
										return NULL; \
										}
//TODO add logger message

#define EPS 0.00000000001

/*** Struct Definition ***/
struct kd_array_t {

	SPPoint* pointArray;
	int** mat;
	int cols;
	int rows;
};
/****************************/

struct pointAxis_t {
	SPPoint pnt;
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
		if (!ret[i]) {
			for (i--; i >= 0; i--) {
				free(ret[i]);
			}

			free(ret);
			return NULL;
		}

	}
	return ret;
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
	*msg = 0; //TODO
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

		if (!ptsAx[i]) {
			for (i--; i >= 0; i--) {
				free(ptsAx[i]);
			}

			free(ptsAx);
			return NULL;
		}
		ptsAx[i]->pnt = pts[i];
		ptsAx[i]->axis = axis;
		ptsAx[i]->indexInPntArray = i;

	}

	qsort(ptsAx, size, sizeof(*ptsAx), pointComparator);
	int* ret = (int*) malloc(size * sizeof(int));
	if (!ret) {
		//TODO add logger message
		return NULL;
	}
	for (int i = 0; i < size; i++)
		ret[i] = ptsAx[i]->indexInPntArray;

	for (int i = size - 1; i >= 0; i--) {
		free(ptsAx[i]);
	}
	free(ptsAx);

	return ret;
}

void SPKDArrayDestroy(SPKDArray kd, int arrsize, int rows) {
	int i;
	/* frees kd->pointArray */
	for (i = 0; i < arrsize; i++)
		spPointDestroy(kd->pointArray[i]);

	/* Frees kd->Mat */
	for (i = 0; i < rows; i++)
		free(kd->mat[i]);

	free(kd);
}

SPKDArray init(SPConfig attr, SPPoint *arr, int size, SPLogger logger,
		SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg) {

	if (!arr || size < 1)
//TODO add logger message
		return NULL;

	/*create new struct SPKDArray*/
	SPKDArray kd = (SPKDArray) malloc(sizeof(*kd));

	if (!kd) {
//TODO add logger message
		return NULL;
	}

	int dims = spConfigGetPCADim(attr, conf_msg);
	if (dims < 0) {
//TODO add logger message
		SPKDArrayDestroy(kd, size, dims);
		return NULL;
	}
	kd->cols = size;
	kd->rows = dims;
	/*copy array into struct*/
	kd->pointArray = copyPointArray(arr, size);
	if (!kd->pointArray) {
//TODO add logger message
		SPKDArrayDestroy(kd, size, dims);
		return NULL;
	}

	/*create initialized matrix of size dims X size */

	int axis;
	int *M[dims];
	memset(M, 0, sizeof(M));
	for (axis = 0; axis < dims; axis++) {
		M[axis] = sortByAxis(arr, size, axis, attr, logger, log_msg);
		if (!M[axis]) {
			//TODO add logger message
			SPKDArrayDestroy(kd, size, dims);
			return NULL;
		}
	}
	return kd;
}

int split(SPKDArray kd, int coor, SPLogger logger, SP_LOGGER_MSG *log_msg,
		SP_CONFIG_MSG *conf_msg) {
	int n = kd->cols, splitSize = (int) (ceil(kd->cols) + 0.5), i, indexP1 = -1,
			indexP2 = -1, *map1 = NULL, *map2 = NULL;
	SPPoint *P1 = NULL, *P2 = NULL;
	*log_msg = 0;												///TODO DELETE
	*conf_msg = 0;												///TODO DELETE
	printf("%p", &logger);								///TODO DELETE
	/** boolean array for belonging to splits **/
	bool* halfs = (bool*) malloc(n * sizeof(bool));
	if (!halfs) {
		//TODO add logger message
		return -1;
	}
	for (i = 0; i < splitSize; i++) {
		halfs[kd->mat[coor][i]] = false;
	}
	for (; i < n; i++) {
		halfs[kd->mat[coor][i]] = true;
	}

	/** two point arrays to contain sorted points according to belonging **/
	P1 = (SPPoint*) malloc(splitSize);
	P2 = (SPPoint*) malloc(n - splitSize);
	map1 = (int*) malloc(n);
	map2 = (int*) malloc(n);
	if (!P1 || !P2 || !map1 || !map2) {
		//TODO add logger message
		free(map1);
		free(map2);
		free(P1);
		free(P2);
		free(halfs);
		return -1;
	}
	for (i = 0; i < n; i++) {
		if (!halfs[i]) {
			P1[++indexP1] = kd->pointArray[i];
			map1[i] = indexP1;
			map2[i] = -1;
		} else {
			P2[++indexP2] = kd->pointArray[i];
			map2[i] = indexP2;
			map1[i] = -1;
		}
	}

	return 0;
}

