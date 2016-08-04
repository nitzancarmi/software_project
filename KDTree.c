#include "KDTree.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct kd_tree_node_t {

	int dim;
	int val;
	SPKDTreeNode left;
	SPKDTreeNode right;
	SPPoint data;
};

bool doubleEquals(double a, double b) {  //TODO
	if (fabs(a - b) < EPS)
		return true;
	return false;
}

int randomInRange(int min, int max) {  //TODO
	srand(time(NULL));	// Intializes random number generator
	int range = max - min + 1;
	return (rand() % range) + min;
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

SPKDTreeNode nodeAllocation(int dim, int val, SPKDTreeNode* left,
		SPKDTreeNode* right, SPPoint* data) {
	SPKDTreeNode node = (SPKDTreeNode) malloc(sizeof(*node));
	if (!node)
		//TODO logger
		return NULL;

	node->dim = dim;
	node->val = val;
	node->left = (left) ? *left : NULL;
	node->right = (right) ? *right : NULL;
	node->data = (data) ? *data : NULL;
	return node;
}

SPKDTreeNode spKDTreeCreateRecursion(SPKDArray kdarray, SPConfig config,
		SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg,
		int splitIncrementalDim) {
	printKDArrayMatrix(kdarray);
	printf("cols = %d\n", getKDCols(kdarray));
	if (!config || !conf_msg || !log_msg
			|| splitIncrementalDim < 0) {
		InvalidError()
		printf("NULL\n");
		return NULL;
	}
	if(!kdarray)
		return NULL;

	SPKDTreeNode nodeLeft = NULL, nodeRight = NULL;
	SPKDArray KDpntr1 = NULL, KDpntr2 = NULL;
	if (getKDCols(kdarray) == 1) { /*getKDCols(kdarray) == 1*/
	//	printf("inside\n");
		//return NULL;
		return nodeAllocation(-1, -1, NULL, NULL, getKDPointArray(kdarray));
	}


	returnIfConfigMsg(NULL)
//	printf("**confmsg1 = %d\n",*conf_msg);
	splitMethod method = spConfigGetSplitMethod(config, conf_msg);
	returnIfConfigMsg(NULL)
//	printf("**confmsg2 = %d\n",*conf_msg);
	int dim, totalDims = spConfigGetPCADim(config, conf_msg);
	returnIfConfigMsg(NULL)
	/* Chooseing split dimension based on config */
	switch (method) {
	case MAX_SPREAD:
//		printf("maxspread\n");
		dim = getMaxSpreadDimension(kdarray);
		if (dim == -1) {
			InvalidError()
			return NULL;
		}
		break;
	case RANDOM:
		dim = randomInRange(0, totalDims);
		break;
	case INCREMENTAL:
//		printf("incremental\n");

		dim = splitIncrementalDim % totalDims;
//		if(getKDCols(kdarray) <= 3){
//			printf("dim = %d\n",dim);
//			printf("IM HERE\n");
//			return NULL;
//		}
		break;
	}
	int split;
	/* Split */
	if ((split = spKDArraySplit(kdarray, dim, &KDpntr1, &KDpntr2,config, log_msg,
			conf_msg)) == -1) {
		printf("ERROR WITH SPLIT");		//TODO Remove
		return NULL;

	}

//	printf("**confmsg3 = %d\n",*conf_msg);
	/* Left Recursion */
	nodeLeft = spKDTreeCreateRecursion(KDpntr1, config, conf_msg, log_msg,
			splitIncrementalDim + 1);
	returnIfConfigMsg(NULL)
	/* Right Recursion */
	nodeRight = spKDTreeCreateRecursion(KDpntr2, config, conf_msg, log_msg,
			splitIncrementalDim + 1);
	returnIfConfigMsg(NULL)
	return nodeAllocation(dim, split, &nodeLeft, &nodeRight, NULL);
	return NULL;
}

SPKDTreeNode spKDTreeCreate(SPKDArray kdarray, SPConfig config,
		SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg) {

	return spKDTreeCreateRecursion(kdarray, config, conf_msg, log_msg, 0);
}

void spKDTreeDestroy(SPKDTreeNode kdtree) {
	if (!kdtree)
		return;

	spPointDestroy(kdtree->data);
	spKDTreeDestroy(kdtree->left);
	spKDTreeDestroy(kdtree->right);
	free(kdtree);

}

