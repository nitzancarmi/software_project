#include "KDArray.h"
#include <math.h>

#define arrayMallocFail(array,j)	 if(!array[j]){ \
										for(j--;j>=0;j--){ \
											free(array[j]); \
										}\
										free(array); \
										return NULL; \
										}
#define frees()							free(map1); free(map2); free(X);
//TODO add logger message

#define EPS 0.00000000001

/*** Struct DefspKDArrayCreateion ***/
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

SPPoint* getKDPointArray(SPKDArray kd) {
	SPPoint* cpy = (SPPoint*) malloc(kd->cols * sizeof(SPPoint));
	int i;
	for (i = 0; i < kd->cols; i++)
		cpy[i] = spPointCopy(kd->pointArray[i]);
	return cpy;
}

int** calloc2dInt(int rows, int cols) {
	int header = rows * sizeof(int*);
	int data = rows * cols * sizeof(int);
	int** rowptr = (int**) calloc(header + data, sizeof(int));
	if (!rowptr)
		return NULL;
	int* buf = (int*) (rowptr + rows);
	for (int k = 0; k < rows; ++k) {
		rowptr[k] = buf + k * cols;
	}
	return rowptr;
}

int** getKDMat(SPKDArray kd) {
	int i, j;

	int** cpy = (int**) malloc(
			kd->rows * sizeof(int*) + kd->rows * kd->cols * sizeof(int));
	if (!cpy)
		return NULL;
	int* offset = cpy[kd->rows];
	for (i = 0; i < kd->rows; i++, offset += kd->cols)
		cpy[i] = offset;

	for (i = 0; i < kd->rows; i++)
		for (j = 0; j < kd->cols; j++)
			cpy[i][j] = (kd->mat)[i][j];

	return cpy;
}

int getKDCols(SPKDArray kd) {
	return (!kd) ? -1 : kd->cols;
}

int getKDRows(SPKDArray kd) {
	return (!kd) ? -1 : kd->rows;
}
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
int sortByAxis(int *ret, const SPPoint* pts, int size, int axis,
		const SPConfig config, SP_LOGGER_MSG* msg) {
	*msg = 0; //TODO
	if (!pts || axis < 0 || !config) {
		spLoggerPrintError("Invalid Argument to sortByAxis", __FILE__, __func__,
		__LINE__);
		return 1;
	}
	PointAxis* ptsAx = (PointAxis*) malloc(size * sizeof(*ptsAx));

	if (!ptsAx)
		//TODO add logger message
		return 1;

	for (int i = 0; i < size; i++) {
		ptsAx[i] = (PointAxis) malloc(sizeof(*ptsAx[i]));

		/*Malloc fail */

		if (!ptsAx[i]) {
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

void printKDArrayMatrix(SPKDArray kd) {
	if (!kd) {
		printf("kd matrix = NULL");
		return;
	}
	printf("\n");
	printf("KDArray Matrix is:\n");
	for (int i = 0; i < kd->rows; i++) {
		printf("| ");
		for (int j = 0; j < kd->cols; j++) {
			printf((j == kd->cols - 1 ? "%d" : "%d\t"), kd->mat[i][j]);
		}
		printf(" |\n");
	}

}

void print2DIntArray(int** a, int rows, int cols) {
	printf("\n");
	for (int i = 0; i < rows; i++) {
		printf("| ");
		for (int j = 0; j < cols; j++) {
			printf((j == cols - 1 ? "%d" : "%d\t"), a[i][j]);
		}
		printf(" |\n");
	}

}

void printKDPointArray(SPKDArray kd) {
	if (!kd) {
		printf("error");
		return;
	}
	printf("\n=====Point Array:=====\n");
	for (int j = 0; j < kd->cols; j++) {
		printf("%d. (", j);
		for (int i = 0; i < kd->rows; i++) {
			printf((i == kd->rows - 1 ? "%d" : "%d, "),
					(int) spPointGetAxisCoor(kd->pointArray[j], i));
		}
		printf(")\n");
	}
	printf("======================\n");

}
void printIntArray(int* a, int size, const char* name) {
	if (!a) {
		printf("error");
		return;
	}
	printf("%s: (", name);
	for (int i = 0; i < size; i++) {
		printf((i == size - 1 ? "%d" : "%d, "), a[i]);
	}
	printf(")\n");
}

void printBoolArray(bool* a, int size, const char* name) {
	if (!a) {
		printf("error");
		return;
	}
	printf("%s: (", name);
	int i;
	for (i = 0; i < size; i++) {
		if (i == size - 1)
			printf("%d", a[i]);
		else
			printf("%d, ", a[i]);
	}
	printf(")\n");
}

SPKDArray spKDArrayCreate(SPConfig attr, SPPoint *arr, int size,
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
		SPKDArrayDestroy(kd);
		return NULL;
	}
	kd->cols = size;
	kd->rows = dims;

	/*copy array into struct*/
	kd->pointArray = copyPointArray(arr, size);
	if (!kd->pointArray) {
//TODO add logger message
		SPKDArrayDestroy(kd);
		return NULL;
	}
	/*create initialized matrix of size dims X size */

	int axis;
	int **M = calloc2dInt(dims, size);
//	int **M = (int**) malloc(dims * sizeof(int*) + dims * size * sizeof(int));
//	int *offset = M[dims];
//	for (axis = 0; axis < dims; axis++, offset += size)
//		M[axis] = offset;

	for (axis = 0; axis < dims; axis++) {
		if (sortByAxis(M[axis], arr, size, axis, attr, log_msg)) {
			//TODO add logger message
			SPKDArrayDestroy(kd);
			return NULL;
		}

	}
	kd->mat = M;
	return kd;
}

int spKDArraySplit(SPKDArray kd, int coor, SPKDArray* KDpntr1,
		SPKDArray* KDpntr2, SPConfig config, SP_LOGGER_MSG *log_msg,
		SP_CONFIG_MSG *conf_msg) {

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
//	int splitSize = (int)half + 1;			  //TODO we must have ceil, even numbers will be incremented without it
	//with ceil it always works (almogz)
	int splitSize = (int) (ceil(half) + 0.5); //TODO why do we need ceil? (nitzanc)
	if (!splitSize)
		return 0;

	printf("splitSize = %d\n", splitSize);
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
	printIntArray(map1, n, "map1");
	printIntArray(map2, n, "map2");

	/***** ALLOCATION *****/
	A1 = calloc2dInt(kd->rows, splitSize);
//	A1 = (int**) malloc(kd->rows * sizeof(int*) + kd->rows * splitSize * sizeof(int));
//	int *offset = A1[kd->rows];
//	for (i = 0; i < kd->rows; i++, offset += splitSize)
//		A1[i] = offset;
	KD1->mat = A1;
	KD1->rows = kd->rows;
	KD1->cols = splitSize;
	A2 =  calloc2dInt(kd->rows, n-splitSize);
//	A2 = (int**) malloc(kd->rows * sizeof(int*) + kd->rows * (n - splitSize) * sizeof(int));
//	offset = A2[kd->rows];
//	for (i = 0; i < kd->rows; i++, offset += (n - splitSize))
//		A2[i] = offset;
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
	//printf("\n");
	*KDpntr1 = KD1;
	*KDpntr2 = KD2;

	free(X);
	free(map1);
	free(map2);
	//return the median
	return kd->mat[coor][splitSize];
}
