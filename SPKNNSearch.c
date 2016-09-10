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
 *
 * @return None
 */
void knnRecursive(SPKDTreeNode kd, SPPoint feat, SPBPQueue bpq) {
	if (!kd)
		return;
	//From here, kd is not NULL so no need to check output of inner functions

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

	val = getVal(kd);							// median of the current node, also kd is not NULL here
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
		tmp = NULL; 
		spBPQueueDequeue(bpq);
	}
	spBPQueueDestroy(bpq);

	return ret;
}

int* getClosestImages(SPKDTreeNode kdtree, SPConfig config, SPPoint* q_features,
		int q_numOfFeats, SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg) {
	if(!kdtree || !config || !q_features || q_numOfFeats<0 || !log_msg || !conf_msg){
		InvalidError()
				return NULL;
	}
	int* knn = NULL;
	int i, j;
	int knn_size = spConfigGetKNN(config, conf_msg);
	int numOfImages = spConfigGetNumOfImages(config, conf_msg);
	int numOfSimilarImages = spConfigGetNumOfSimilarImages(config, conf_msg);
	int *img_near_cnt = (int*) calloc(numOfImages, sizeof(int));
	int *similar_images = (int*) calloc(numOfSimilarImages, sizeof(int));
	if (!img_near_cnt || !similar_images) {
		MallocError()
		return NULL;
	}

	//for each point in the query image, find k-nearest neighbors
	for (i = 0; i < q_numOfFeats; i++) {
		knn = findKNearestNeighbors(kdtree, q_features[i], config);
		if (!knn) {
			return NULL;
		}

		//count image indices related to neighbors just found
		for (j = 0; j < knn_size; j++)
			img_near_cnt[knn[j]]++;
		free(knn);
		knn = NULL;
	}

	//return the k nearest images based on img_near_cnt array
	// assumes numOfSimilarImages << n
	for (i = 0; i < numOfSimilarImages; i++) {
		similar_images[i] = 0;
		for (j = 0; j < numOfImages; j++) {
			if (img_near_cnt[j] > img_near_cnt[similar_images[i]])
				similar_images[i] = j;
		}
		img_near_cnt[similar_images[i]] = 0;
	}

	free(img_near_cnt);
	return similar_images;
}

