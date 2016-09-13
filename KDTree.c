#include "KDTree.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

/* struct definition as explained in the instructions */
struct kd_tree_node_t {

	int dim;
	int val;
	SPKDTreeNode left;
	SPKDTreeNode right;
	SPPoint data;
};

bool doubleEquals(double a, double b) {
	if (fabs(a - b) < EPS)
		return true;
	return false;
}

/**
 * 	Returns an (assumed by C random implementation) uniformly distributed integer
 * 	between min and max. Each time the function is called a new seed is generated.
 * 	Only one call to this function during SPCBIR execution so no checks are needed.
 *
 *  @param min	minimum of range
 *  @param max	maximum of range
 *  @return
 *  The random integer generated.
 */
int randomInRange(int min, int max) {
	srand(time(NULL));	// Intializes random number generator
	int range = max - min + 1;
	return (rand() % range) + min;
}

/**
 * 	Calculate the minimum and maximum values between all the points of a SPPoint Array,
 * 	of the coordinate coor. The values are stored in the pointers max and min.
 *
 *	if ptarr == NULL || min == NULL || max == NULL || coor<0 || numOfPts < 1
 *	than the value INT_MIN is stored in min and max.
 *
 *  @param arr 			SPPoint array relevant
 *  @param numOfPts 	Number of points in arr
 *  @param coor			The coordinate
 *  @param max			Address of the double to be the maximum
 *  @param min			Address of the double to be the min
 *
 */
void getMinMaxOfCoordinate(SPPoint* ptarr, int numOfPts, int coor, double* max,
		double* min) {
	if (!ptarr || numOfPts < 1 || coor < 0 || !max || !min) {
		declareLogMsg();
		InvalidError()
		*min = INT_MIN;
		*max = INT_MIN;
		return;
	}
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

/**
 *  Calculates the Max Spread dimension of a KD-Array.
 *  The max spread dimension is the dimension that has the biggest difference between the maximum
 *  value and the minimum value of all the points in the KDArray in that dimension.
 *
 *  @param arr  The relevant SPKDArray
 *  @return
 *  -1 if arr == NULL or arr has invalid properties
 *  The max spread dimension otherwise
 */
int getMaxSpreadDimension(SPKDArray arr) {
	if (!arr){
		declareLogMsg();
		InvalidError()
		return -1;
	}
	/*Initializaion*/
	SPPoint* ptarr = getKDPointArray(arr);
	if (!ptarr)
		return -1;
	int dims = spPointGetDimension(*ptarr);
	int numOfPts = getKDCols(arr), dim = 0;
	double max, min, maxSpread, tmpMaxSpread;

	/*The first coordinate is the first candidate*/
	getMinMaxOfCoordinate(ptarr, numOfPts, 0, &max, &min);
	if (min == INT_MIN && max == INT_MIN)
		return -1;

	maxSpread = max - min;

	for (int coor = 1; coor < dims; coor++) {
		getMinMaxOfCoordinate(ptarr, numOfPts, coor, &max, &min);
		tmpMaxSpread = max - min;

		if (!doubleEquals(tmpMaxSpread, maxSpread)) {
			if (tmpMaxSpread > maxSpread) {
				maxSpread = tmpMaxSpread;
				dim = coor;
			}
		}
		// If they are equal then lower coordinate is still the candidate
	}
	for (int i = 0; i < numOfPts; i++) {
		spPointDestroy(ptarr[i]);
	}
	free(ptarr);
	return dim;
}

/**
 *  Allocates a new SPKDTreeNode based on the arguments given to this function.
 *
 *  @param dim  	Dimension the node is split with
 *  @param val  	Value of the median in the dimension
 *  @param left		Address of the left SPKDTreeNode child
 *  @param right	Address of the left SPKDTreeNode child
 *  @param data		SPPoint array
 *  @return
 *  NULL if Memory Allocation failure occurred
 *  otherwise the allocated node.
 */
SPKDTreeNode nodeAllocation(int dim, int val, SPKDTreeNode* left,
		SPKDTreeNode* right, SPPoint* data) {
	SPKDTreeNode node = (SPKDTreeNode) malloc(sizeof(*node));
	if (!node) {
		declareLogMsg();
		MallocError()
		return NULL;
	}
	node->dim = dim;
	node->val = val;
	node->left = (left) ? *left : NULL;
	node->right = (right) ? *right : NULL;
	node->data = (data) ? *data : NULL;
	return node;
}

/**
 *  Actual implementation of the recursive creation of a KDTree based on KDArray.
 *  was created as a separate function for comfortability to get splitIncrementalDim.
 *  @see spKDTreeCreate in header.
 *
 *  @param KDArray   				The KD-Array from which the KD-Tree will be constructed.
 *  @param config					Configuration object that contains information about the function execution
 *  								(e.g. splitMethod)
 *  @param splitIncrementalDim		Current incremental dimension
 *  @return
 *  A new SPKDTreeNode in case of a success.
 *  NULL in case one of the following occurred:
 *  	- kdarray == NULL || config == NULL || conf_msg == NULL || log_msg == NULL
 *  	- Memory allocation failure
 *  	- SPConfig access error
 */
SPKDTreeNode spKDTreeCreateRecursion(SPKDArray kdarray, SPConfig config, 
                    SP_CONFIG_MSG *conf_msg, SP_LOGGER_MSG *log_msg, int splitIncrementalDim) {

	if (!kdarray || !config	|| splitIncrementalDim < 0 || 
            *log_msg != SP_LOGGER_SUCCESS || *conf_msg != SP_CONFIG_SUCCESS) {
		InvalidError()
		return NULL;
	}
	SPKDTreeNode nodeLeft = NULL, nodeRight = NULL;
	SPKDArray KDpntr1 = NULL, KDpntr2 = NULL;
	splitMethod method = spConfigGetSplitMethod(config, conf_msg);
	returnIfConfigMsg(NULL)
	int dim, totalDims = spConfigGetPCADim(config, conf_msg);
	returnIfConfigMsg(NULL)

	if (getKDCols(kdarray) == 1) {
		SPPoint pnt = getKDOnlyPoint(kdarray);
		return nodeAllocation(-1, -1, NULL, NULL, &pnt);
	}

	/* Chooseing split dimension based on config */
	switch (method) {

	case MAX_SPREAD:
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
		dim = splitIncrementalDim % totalDims;
		break;

	default:
		InvalidError()
		return NULL;
	}

	/* Split */
	int split;
	if ((split = spKDArraySplit(kdarray, dim, &KDpntr1, &KDpntr2, log_msg,
			conf_msg)) == -1) {
		return NULL;
	}

	/* Left Recursion */
	nodeLeft = spKDTreeCreateRecursion(KDpntr1, config, conf_msg, log_msg,
			splitIncrementalDim + 1);
	SPKDArrayDestroy(KDpntr1);
	returnIfConfigMsg(NULL)

	/* Right Recursion */
	nodeRight = spKDTreeCreateRecursion(KDpntr2, config, conf_msg, log_msg,
			splitIncrementalDim + 1);
	SPKDArrayDestroy(KDpntr2);
	returnIfConfigMsg(NULL)
	return nodeAllocation(dim, split, &nodeLeft, &nodeRight, NULL);
}

SPKDTreeNode spKDTreeCreate(SPKDArray kdarray, SPConfig config) {

        declareLogMsg();
        declareConfMsg();
        printInfo(KDT_START);
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

bool isLeaf(SPKDTreeNode node) {
	if (!node)
		return false;

	if (node->dim == -1 && node->val == -1 && !node->left && !node->right
			&& node->data)
		return true;

	return false;

}

int getIndex(SPKDTreeNode node) {
	if (!node)
		return -1;

	return spPointGetIndex(node->data);
}

double getDistFromPoint(SPKDTreeNode node, SPPoint point) {
	if (!node || !point)
		return -1;

	return spPointL2SquaredDistance(node->data,point);
}

int getDim(SPKDTreeNode node) {
	if (!node)
		return -1;

	return node->dim;
}

int getVal(SPKDTreeNode node) {
	if (!node)
		return -1;

	return node->val;
}

SPKDTreeNode getLeftSubtree(SPKDTreeNode node) {
	if (!node)
		return NULL;

	return node->left;
}

SPKDTreeNode getRightSubtree(SPKDTreeNode node) {
	if (!node)
		return NULL;

	return node->right;
}
