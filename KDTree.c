#include "KDTree.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

struct kd_tree_node_t {

	int dim;
	int val;
	KDTreeNode left;
	KDTreeNode right;
	SPPoint data;
};

int getKDRows(SPKDArray arr) {	//TODO
	printf("%d", arr == NULL);
	return 0;
}

SPPoint* getKDPointArray(SPKDArray arr) { //TODO
	printf("%d", arr == NULL);
	return NULL;
}

bool doubleEquals(double a, double b) {  //TODO
	if (fabs(a - b) < EPS)
		return true;
	return false;
}

int randomInRange(int min, int max) {  //TODO
	printf("%d%d", min, max);
	return 0;
}

void getMinMaxOfCoordinate(SPPoint* ptarr, int numOfPts, int coor, int* max,
		int* min) {
	if (!ptarr || numOfPts < 0 || coor < 0 || !max || !min)
		return;

	*max = spPointGetAxisCoor(ptarr[0], coor);
	*min = *max;
	for (int i = 1; i < numOfPts; i++) {
		double pnt = spPointGetAxisCoor(ptarr[i], coor);
		if (!doubleEquals(pnt, *max))
			*max = (pnt > *max) ? pnt : *max;
		if (!doubleEquals(pnt, *min))
			*min = (pnt < *min) ? pnt : *min;
	}
}

int getMaxSpreadDimension(SPKDArray arr) {
	if (!arr)
		return -1;
	/*Initializaion*/
	SPPoint* ptarr = getKDPointArray(arr);
	if (!ptarr)
		return -1;
	int dims = spPointGetDimension(*ptarr);
	int numOfPts = getKDRows(arr);
	int max, min, maxSpread, dim = 0, tmpMaxSpread;

	/*The first coordinate is the first candidate*/
	getMinMaxOfCoordinate(ptarr, numOfPts, 0, &max, &min);
	maxSpread = max - min;

	for (int coor = 1; coor < dims; coor++) {
		getMinMaxOfCoordinate(ptarr, numOfPts, coor, &max, &min);
		tmpMaxSpread = max - min;
		if (!doubleEquals(tmpMaxSpread, maxSpread)) {
			maxSpread = (tmpMaxSpread > maxSpread) ? tmpMaxSpread : maxSpread;
			dim = coor;
		}
		// If they are equal then lower coordinate is still the candidate
	}

	return dim;
}

KDTreeNode nodeAllocation(int dim, int val, KDTreeNode left, KDTreeNode right,
		SPPoint data) {
	KDTreeNode node = (KDTreeNode) malloc(sizeof(*node));
	if (!node)
		//TODO logger
		return NULL;

	node->dim = dim;
	node->val = val;
	node->left = left;
	node->right = right;
	node->data = data;
	return node;
}

KDTreeNode spKDTreeCreateRecursion(SPKDArray kdarray, SPConfig config,
		SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg,
		int splitIncrementalDim) {

	if (!kdarray || !config || !conf_msg || !log_msg) {
		InvalidError();
		return NULL;
	}

	KDTreeNode nodeLeft = NULL, nodeRight = NULL;
	SPKDArray KDpntr1, KDpntr2;
	if (getKDRows(kdarray) == 1) {
		return nodeAllocation(-1, -1, NULL, NULL, *getKDPointArray(kdarray));
	}

	splitMethod method = spConfigGetSplitMethod(config, conf_msg);
	returnIfConfigMsg(NULL)

	int dim, totalDims = spConfigGetPCADim(config, conf_msg);
	returnIfConfigMsg(NULL)

	/* Chooseing split dimension based on config */
	switch (method) {
	case MAX_SPREAD:
		dim = getMaxSpreadDimension(kdarray);
		if (dim == -1) {
			InvalidError();
			return NULL;
		}
		break;
	case RANDOM:
		dim = randomInRange(0, totalDims);
		break;
	case INCREMENTAL:
		dim = splitIncrementalDim;
		dim = (dim + 1) % totalDims;
		break;
	}
	int split;
	/* Split */
	if ((split = spKDArraySplit(kdarray, dim, &KDpntr1, &KDpntr2, log_msg,
			conf_msg)) == -1)
		return NULL;

	/* Left Recursion */
	nodeLeft = spKDTreeCreateRecursion(KDpntr1, config, conf_msg, log_msg,
			splitIncrementalDim + 1);
	returnIfConfigMsg(NULL);

	/* Right Recursion */
	nodeRight = spKDTreeCreateRecursion(KDpntr2, config, conf_msg, log_msg,
			splitIncrementalDim + 1);
	returnIfConfigMsg(NULL);

	return nodeAllocation(dim, split, nodeLeft, nodeRight, NULL);
}

KDTreeNode spKDTreeCreate(SPKDArray kdarray, SPConfig config,
		SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg) {

	return spKDTreeCreateRecursion(kdarray, config, conf_msg, log_msg, 0);
}

