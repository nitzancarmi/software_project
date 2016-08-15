#include "macros.h"
#include "KDTree.h"
#include "SPBPriorityQueue.h"
#include "math.h"

/**
 * 	This function is the recursive function for the actual KNN search.
 * 	It searches for all the neighbors of the SPPoint feat in the KD-Tree kd,
 * 	while maintaining and updating a Bounded Priority Queue of the size KNN.
 *
 *  @param kdtree   	The KD-Tree to be searched on.
 *  @param point		Query for the search
 *  @param bpq			The SPBPQueue to be updated after the search of
 */
void knnRecursive(SPKDTreeNode kd, SPPoint feat, SPBPQueue bpq) {
	if (!kd)
		return;

	SPListElement elem = NULL;
	bool left = false, equal, lessOrEqual;
	double val, p_dim;

	/** If the node is a leaf - try to enqueue the element with the distance
	 * from feat. If it is bigger than all the points already in the bpq
	 * than the element is not added.
	 */
	if (isLeaf(kd)) {
		elem = spListElementCreate(getIndex(kd), getDistFromPoint(kd, feat));
		spBPQueueEnqueue(bpq, elem);
		spListElementDestroy(elem);	//In any case elem needs to be freed because a copy is made in enqueue
		elem = NULL;
		return;
	}

	val = getVal(kd);							// median of the current node
	p_dim = spPointGetAxisCoor(feat, getDim(kd));	// P[curr.dim]

	/** check if val <= p_dim **/
	equal = doubleEquals(val, p_dim);
	lessOrEqual = (equal) ? equal : val < p_dim;
	/***************************/


	/** First, continue the recursion downward the KDTree based on val **/
	if (lessOrEqual) {
		knnRecursive(getLeftSubtree(kd), feat, bpq);
		left = true;	//to know it was left
	} else
		knnRecursive(getRightSubtree(kd), feat, bpq);


	/** Then, check the conditions for recursion continuation
	 *  on the second branch - if the queue isn't full, than obviously
	 *  continue to fill it until its full.
	 *  Or, |curr.val - P[curr.dim]| < bpq.maxPriority.
	 */
	if (!spBPQueueIsFull(bpq) || fabs(val - p_dim) < spBPQueueMaxValue(bpq)) {
		if (left)
			knnRecursive(getRightSubtree(kd), feat, bpq);
		else
			knnRecursive(getLeftSubtree(kd), feat, bpq);
	}
}

int* findKNearestNeighbors(SPKDTreeNode kdtree, SPPoint feat, SPConfig config) {

	/** Initializations **/
	SP_CONFIG_MSG _conf_msg = SP_CONFIG_SUCCESS, *conf_msg = &_conf_msg;
	declareLogMsg();
	if (!kdtree || !feat || !config) {
		InvalidError();
		return NULL;
	}
	int i,
	k = spConfigGetKNN(config, conf_msg), *ret;
	returnIfConfigMsg(NULL)
	SPListElement tmp = NULL;
	/* Creation of the Bounded Priority Queue */
	SPBPQueue bpq = spBPQueueCreate(k);
	if (!bpq) {
		MallocError()
		return NULL;
	}

	/** Main call to the KNN Search **/
	knnRecursive(kdtree, feat, bpq);
	/*********************************/

	/* returned indices array */
	ret = (int*) malloc(k * sizeof(int));
	if (!ret) {
		MallocError()
		spBPQueueDestroy(bpq);
		return NULL;
	}

	/* Copy to the indices array */
	for (i = 0; i < k; i++) {
		tmp = spBPQueuePeek(bpq);
		ret[i] = spListElementGetIndex(tmp);
		spListElementDestroy(tmp); //gets copied for the peek
		tmp = NULL;				   //sanity problem prevention
		spBPQueueDequeue(bpq);	   //remove element from the bpq
	}
	spBPQueueDestroy(bpq);

	return ret;
}

