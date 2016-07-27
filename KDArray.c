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
#define frees()							SPKDArrayDestroy(KD1); SPKDArrayDestroy(KD2); \
										free(map1); free(map2); \
										free(halfs);
//TODO add logger message



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
		SP_LOGGER_MSG* msg) {
	*msg = 0; //TODO
	if (!pts || axis < 0 || !config) {
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

void SPKDArrayDestroy(SPKDArray kd) {
	int i;
	/* frees kd->pointArray */
	if (kd->pointArray) {
		for (i = kd->cols - 1; i >= 0; i--)
			spPointDestroy(*(kd->pointArray + i));
	}
	if (kd->mat) {
		/* Frees kd->Mat */
		for (i = kd->rows - 1; i >= 0; i--) {
			if (*(kd->mat + i)) {
				free(*(kd->mat + i));
			}
		}
	}
	free(kd->mat);
	free(kd);
}

void printKDArrayMatrix(SPKDArray kd) {
	printf("\n");
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
void printIntArray(int* a, int size) {
	if (!a) {
		printf("error");
		return;
	}
	printf("axis: (");
	for (int i = 0; i < size; i++) {
		printf((i == size - 1 ? "%d" : "%d, "), a[i]);
	}
	printf(")\n");
}

SPKDArray spKDArrayCreate(SPConfig attr, SPPoint *arr, int size, SP_LOGGER_MSG *log_msg,
		SP_CONFIG_MSG *conf_msg) {

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

	printf("rows = %d\n", dims);
	printf("cols = %d\n", size);
	/*copy array into struct*/
	kd->pointArray = copyPointArray(arr, size);
	if (!kd->pointArray) {
//TODO add logger message
		SPKDArrayDestroy(kd);
		return NULL;
	}
	/*create initialized matrix of size dims X size */

	int axis;
	int **M = (int**) malloc(dims * sizeof(int*));
	for (axis = 0; axis < dims; axis++) {
		M[axis] = sortByAxis(arr, size, axis, attr, log_msg);
		printIntArray(M[axis], size);
		if (!M[axis]) {
			//TODO add logger message
			SPKDArrayDestroy(kd);
			return NULL;
		}

	}
	kd->mat = M;
	return kd;
}

int spKDArraySplit(SPKDArray kd, int coor, SPKDArray* KDpntr1, SPKDArray* KDpntr2,
		SP_LOGGER_MSG *log_msg, SP_CONFIG_MSG *conf_msg) {

	if (!kd || coor < 0 || !KDpntr1 || !KDpntr2 || !log_msg || !conf_msg)
		return -1;

	int n = kd->cols, i, indexP1 = -1, indexP2 = -1;
	int *map1 = NULL, *map2 = NULL, **A1 = NULL, **A2 = NULL;

	SPPoint *P1 = NULL, *P2 = NULL;
	double half = kd->cols;
	half /= 2;
	int splitSize = (int) (ceil(half) + 0.5);
	SPKDArray KD1 = NULL, KD2 = NULL;
	KD1 = (SPKDArray) malloc(sizeof(*KD1));
	KD2 = (SPKDArray) malloc(sizeof(*KD2));
	if (!KD1 || !KD2) {
		free(KD1);
		free(KD2);
	}
	memset(KD1, 0, sizeof(*KD1));
	memset(KD2, 0, sizeof(*KD2));

	*log_msg = 0;											///TODO DELETE
	*conf_msg = 0;											///TODO DELETE

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
	P1 = (SPPoint*) malloc(splitSize * sizeof(SPPoint));
	KD1->pointArray = P1;
	P2 = (SPPoint*) malloc((n - splitSize) * sizeof(SPPoint));
	KD2->pointArray = P2;
	map1 = (int*) malloc(n * sizeof(int));
	map2 = (int*) malloc(n * sizeof(int));
	if (!P1 || !P2 || !map1 || !map2) {
		//TODO add logger message
		frees()

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

	/***** ALLOCATION *****/
	A1 = (int**) calloc(splitSize * kd->rows, sizeof(int));
	KD1->mat = A1;
	KD1->rows = kd->rows;
	KD1->cols = splitSize;
	A2 = (int**) calloc((n - splitSize) * kd->rows, sizeof(int));
	KD2->mat = A2;
	KD2->rows = kd->rows;
	KD2->cols = n - splitSize;
	if (!A1 || !A2) {
		//TODO add logger message
		frees()

		return -1;
	}
	for (int i = 0; i < splitSize; i++) {
		A1[i] = (int*) malloc(splitSize * sizeof(int));

		/*Malloc fail */
		if (!A1[i]) {
			//TODO add logger message
			frees()

			return -1;
		}
	}

	for (int i = 0; i < (n - splitSize); i++) {
		A2[i] = (int*) malloc((n - splitSize) * sizeof(int));

		/*Malloc fail */
		if (!A2[i]) {
			frees()

			return -1;
		}
	}
	/**********************/

	for (int i = 0; i < n; i++) {		//for on columns of halfs
		for (int j = 0; j < kd->rows; j++) {
			if (i < splitSize) {
				int cell = kd->mat[j][i];
				A1[j][i] = map1[cell];
				assert(A1[j][i] != -1);					//TODO DELETE
			} else {
				int cell = kd->mat[j][i];
				A2[j][i- splitSize] = map2[cell];
				assert(A2[j][i] != -1);					//TODO DELETE
			}
		}
	}
	*KDpntr1 = KD1;
	*KDpntr2 = KD2;


	return 0;
}

