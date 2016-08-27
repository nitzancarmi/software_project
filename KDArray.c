#include "KDArray.h"
#include "macros.h"
#include <math.h>

/*** Struct defintion for spKDArray ***/
struct kd_array_t {

	SPPoint* pointArray;
	int** mat;
	int cols;
	int rows;
};
/****************************/

/** Private struct for comfrotably sorting the kd-array's matrix during creation **/
struct pointAxis_t {
	SPPoint pnt;
	int axis;
	int indexInPntArray;
};

typedef struct pointAxis_t* PointAxis;


SPPoint* getKDPointArray(SPKDArray kd) {
	int i;
	declareLogMsg();
	if (!kd) {
		InvalidError()
		return NULL;
	}

	/** Allocation of the copy **/
	SPPoint* cpy = (SPPoint*) calloc(kd->cols, sizeof(SPPoint));
	if (!cpy) {
		MallocError()
		return NULL;
	}

	/** Copying each point **/
	for (i = 0; i < kd->cols; i++) {
		cpy[i] = spPointCopy(kd->pointArray[i]);
		if (!cpy[i]) {
			MallocError()
			spPointArrayDestroy(cpy, kd->cols);
			return NULL;
		}

	}
	return cpy;
}

SPPoint getKDOnlyPoint(SPKDArray kd) {
	declareLogMsg();
	if (!kd) {
		InvalidError()
		return NULL;
	}

	/** Copying the first point only **/
	SPPoint ret = spPointCopy(kd->pointArray[0]);
	if (!ret) {
		MallocError()
		return NULL;
	}
	return ret;
}

/*
 * Allocation of a 2d integer array while allowing only one call to free() function when
 * deallocating the said array. No need to free each individual row.
 * The value of each cell in the returned array is zero.
 *
 * @param rows - rows of the allocated matrix
 * @param cols - columns of the allocated matrix
 *
 * @return NULL if rows < 0 || cols < 0 or a Memory allocation failure occurred.
 * Otherwise, the allocated array.
 */
int** calloc2dInt(int rows, int cols) {
	declareLogMsg();
	if (rows < 0 || cols < 0) {
		InvalidError()
		return NULL;
	}
	/* size of the main array, pointers to the rows */
	int header = rows * sizeof(int*);

	/* Overall size of all cells */
	int data = rows * cols * sizeof(int);

	/** Allocate enough space to include row pointers & cells **/
	int** rowptr = (int**) calloc(header + data, sizeof(int));
	if (!rowptr) {
		MallocError()
		return NULL;
	}

	/** Pointer to the first cell, after the allocated space for the row pointers **/
	int* buf = (int*) (rowptr + rows);

	for (int k = 0; k < rows; ++k) {
		/** Iterate through all rows, each row pointer of the main array
		 *  will now point to the right row, allowing enough space to remain to accomulate
		 *  all the rows beforehand.
		 */
		rowptr[k] = buf + k * cols;
	}
	return rowptr;
}

int** getKDMat(SPKDArray kd) {
	declareLogMsg();
	if (!kd) {
		InvalidError()
		return NULL;
	}
	int i,
	j, **cpy = calloc2dInt(kd->rows, kd->cols);

	if (!cpy) {
		MallocError()
		return NULL;
	}

	for (i = 0; i < kd->rows; i++)
		for (j = 0; j < kd->cols; j++)
			cpy[i][j] = (kd->mat)[i][j];

	return cpy;
}

int getKDCols(SPKDArray kd) {
	if (!kd) {
		declareLogMsg();
		InvalidError()
		return -1;
	}

	return kd->cols;
}

int getKDRows(SPKDArray kd) {
	if (!kd) {
		declareLogMsg();
		InvalidError()
		return -1;
	}

	return kd->rows;
}

/*
 * Returns a copy of a point array (with copies of each point)
 *
 * @param arr 		the relevant point array
 * @param size		size of the array arr
 * @return NULL if:
 * 		- arr == NULL
 * 		- size < 1
 * 		- Memory Allocation Error
 *
 * Otherwise the copy of the point array.
 */
SPPoint* copyPointArray(SPPoint* arr, int size) {
	int i;
	declareLogMsg();
	if (!arr || size < 1) {
		InvalidError()
		return NULL;
	}
	SPPoint* ret = (SPPoint*) malloc(sizeof(SPPoint) * size);
	if (!ret) {
		MallocError()
		return NULL;
	}
	for (i = 0; i < size; i++) {
		ret[i] = spPointCopy(arr[i]);


		/* frees in case of inner malloc fail */
		if (!ret[i]) {
			MallocError()
			for (i--; i >= 0; i--) {
				free(ret[i]);
			}

			free(ret);
			return NULL;
		}

	}

	return ret;
}

/*
 * Compares between two PointAxis objects based on their axis coordinate value.
 *
 * @param p 		address of the first pointAxis
 * @param q			address of the second pointAxis
 *
 * @return 0 if values are equal (difference is less than Epsilon)
 * 		   1 if value of p is bigger than q
 * 		  -1 if value of q is bigger than p
 */
int pointComparator(const void *p, const void *q) {
	PointAxis p1 = *(const PointAxis *) p;
	PointAxis p2 = *(const PointAxis *) q;

	/* use of register because this function is called many many times */
	register double p1ax = spPointGetAxisCoor(p1->pnt, p1->axis);
	register double p2ax = spPointGetAxisCoor(p2->pnt, p2->axis);

	if (fabs(p1ax - p2ax) < EPS)
		return 0;
	if (p1ax - p2ax < 0)
		return -1;
	return 1;

}
/**
 * sort array of points with respect to given axis
 * 
 * @param ret - int array to be updated with the sorted indices
 * @param pts - opint array to be sorted
 * @param axis - axis to sort by
 * @param config - configuration struct
 * @param log_msg - logger message to be updated
 * @post ret contains the indices of the points sorted with respect to the given axis coordinate
 *  @return 0 for success, 1 otherwise
 */
int sortByAxis(int *ret, const SPPoint* pts, int size, int axis,
		const SPConfig config, SP_LOGGER_MSG* log_msg) {
	if (!pts || axis < 0 || !config) {
		InvalidError()
		return 1;
	}
	PointAxis* ptsAx = (PointAxis*) malloc(size * sizeof(*ptsAx));

	if (!ptsAx) {
		MallocError()
		return 1;
	}

	for (int i = 0; i < size; i++) {
		ptsAx[i] = (PointAxis) malloc(sizeof(*ptsAx[i]));

		/*Malloc fail */

		if (!ptsAx[i]) {
			MallocError()
			for (i--; i >= 0; i--) {
				free(ptsAx[i]);
			}

			free(ptsAx);
			return 1;
		}
		ptsAx[i]->pnt = pts[i];
		ptsAx[i]->axis = axis;
		ptsAx[i]->indexInPntArray = i;

	}

	qsort(ptsAx, size, sizeof(*ptsAx), pointComparator);
	for (int i = 0; i < size; i++)
		ret[i] = ptsAx[i]->indexInPntArray;

	for (int i = size - 1; i >= 0; i--) {
		free(ptsAx[i]);
	}
	free(ptsAx);
	return 0;
}

void SPKDArrayDestroy(SPKDArray kd) {
	int i;
	/* frees kd->pointArray */
	if (kd->pointArray) {
		for (i = kd->cols - 1; i >= 0; i--)
			spPointDestroy(kd->pointArray[i]);
	}
	free(kd->pointArray);

	free(kd->mat);
	free(kd);
}

SPKDArray spKDArrayCreate(SPConfig attr, SPPoint *arr, int size,
		SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg) {

	if (!arr || size < 1) {
		InvalidError()
		return NULL;
	}
	/*create new struct SPKDArray*/
	SPKDArray kd = (SPKDArray) malloc(sizeof(*kd));

	if (!kd) {
		MallocError()
		return NULL;
	}

	int dims = spConfigGetPCADim(attr, conf_msg);

	if (dims < 1) {
		SPKDArrayDestroy(kd);
		returnIfConfigMsg(NULL);
	}
	kd->cols = size;
	kd->rows = dims;

	/*copy array into struct*/
	kd->pointArray = copyPointArray(arr, size);

	if (!kd->pointArray) {
		MallocError()
		SPKDArrayDestroy(kd);
		return NULL;
	}
	/*create initialized matrix of size dims X size */

	int axis;
	int **M = calloc2dInt(dims, size);
	if (!M) {
		MallocError();
		free(kd);
		return NULL;
	}

	for (axis = 0; axis < dims; axis++) {
		if (sortByAxis(M[axis], arr, size, axis, attr, log_msg)) {
			SPKDArrayDestroy(kd);
			return NULL;
		}

	}

	kd->mat = M;
	return kd;
}

int spKDArraySplit(SPKDArray kd, int coor, SPKDArray* KDpntr1,
		SPKDArray* KDpntr2, SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg) {

	if (!kd || coor < 0 || !KDpntr1 || !KDpntr2 || !log_msg || !conf_msg) {
		InvalidError()
		return -1;
	}
	if (!kd->cols || !kd->rows || !kd->mat || !kd->pointArray) {
		InvalidError()
		return -1;
	}
	int n = kd->cols, i, indexP1 = -1, indexP2 = -1;
	int *map1 = NULL, *map2 = NULL, **A1 = NULL, **A2 = NULL;
	*KDpntr1 = NULL;
	*KDpntr2 = NULL;
	SPPoint *P1 = NULL, *P2 = NULL;
	double half = kd->cols;
	half /= 2;
	int splitSize = (int) (ceil(half) + 0.5);
	if (!splitSize)
		return 0;
	SPKDArray KD1 = NULL, KD2 = NULL;
	KD1 = (SPKDArray) malloc(sizeof(*KD1));
	KD2 = (SPKDArray) malloc(sizeof(*KD2));
	if (!KD1 || !KD2) {
		MallocError()
		free(KD1);
		free(KD2);
		return -1;
	}
	memset(KD1, 0, sizeof(*KD1));
	memset(KD2, 0, sizeof(*KD2));

	/** Creating boolean array X to help decide how to split KDArray with respect to given coor **/
	bool* X = (bool*) malloc(n * sizeof(bool));
	if (!X) {
		MallocError()
		return -1;
	}
	for (i = 0; i < splitSize; i++) {
		X[kd->mat[coor][i]] = false;
	}
	for (; i < n; i++) {
		X[kd->mat[coor][i]] = true;
	}

	/** Creating two point arrays to contain sorted points according to needed partition **/
	P1 = (SPPoint*) malloc(splitSize * sizeof(SPPoint));
	KD1->pointArray = P1;
	P2 = (SPPoint*) malloc((n - splitSize) * sizeof(*P2));
	KD2->pointArray = P2;
	map1 = (int*) malloc(n * sizeof(int));
	map2 = (int*) malloc(n * sizeof(int));
	if (!P1 || !P2 || !map1 || !map2) {
		MallocError()
		frees()
		return -1;
	}
	for (i = 0; i < n; i++) {
		if (!X[i]) {
			P1[++indexP1] = spPointCopy(kd->pointArray[i]);
			map1[i] = indexP1;
			map2[i] = -1;
		} else {
			P2[++indexP2] = spPointCopy(kd->pointArray[i]);
			map2[i] = indexP2;
			map1[i] = -1;
		}
	}

	/***** ALLOCATION *****/
	A1 = calloc2dInt(kd->rows, splitSize);
	KD1->mat = A1;
	KD1->rows = kd->rows;
	KD1->cols = splitSize;
	A2 = calloc2dInt(kd->rows, n - splitSize);
	KD2->mat = A2;
	KD2->rows = kd->rows;
	KD2->cols = n - splitSize;
	if (!A1 || !A2) {
		MallocError()
		frees()
		return -1;
	}

	/* Going over original kd->mat, and mapping values to A1, A2, depending on X, map1, map2 values */
	int cell, k1, k2;
	for (int i = 0; i < kd->rows; i++) {		//for on columns of X
		k1 = 0;
		k2 = 0;
		for (int j = 0; j < kd->cols; j++) {
			cell = kd->mat[i][j];
			if (!X[cell])
				A1[i][k1++] = map1[cell];
			else
				A2[i][k2++] = map2[cell];
		}
	}
	*KDpntr1 = KD1;
	*KDpntr2 = KD2;

	free(X);
	free(map1);
	free(map2);
	return kd->mat[coor][splitSize];
}
