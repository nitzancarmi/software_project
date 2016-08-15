#include "macros.h"
#include "KDTree.h"
#include "SPBPriorityQueue.h"
#include "math.h"

void knnRecursive(SPKDTreeNode kd, SPPoint feat, SPBPQueue bpq) {
	if (!kd)
		return;

	SP_BPQUEUE_MSG bpq_msg = SP_BPQUEUE_SUCCESS;

	if (isLeaf(kd)) {
		SPListElement elem = spListElementCreate(getIndex(kd),
				getDistFromPoint(kd, feat));
		bpq_msg = spBPQueueEnqueue(bpq, elem);
		spListElementDestroy(elem);
		return;
	}

	bool left = false;
	double val = getVal(kd), p_dim = spPointGetAxisCoor(feat, getDim(kd));

	bool equal = doubleEquals(val, p_dim), lessOrEqual =
			(equal) ? equal : val < p_dim;

	if (lessOrEqual) {
		knnRecursive(getLeftSubtree(kd), feat, bpq);
		left = true;
	} else
		knnRecursive(getRightSubtree(kd), feat, bpq);

	double max = fabs(val - p_dim);

	if (!spBPQueueIsFull(bpq) || max < spBPQueueMaxValue(bpq)) {
		if (left)
			knnRecursive(getRightSubtree(kd), feat, bpq);
		else
			knnRecursive(getLeftSubtree(kd), feat, bpq);
	}
}

int* findKNearestNeighbors(SPKDTreeNode kdtree, SPPoint feat,
		SPConfig config) {
	SP_CONFIG_MSG _conf_msg = SP_CONFIG_SUCCESS, *conf_msg = &_conf_msg;
	declareLogMsg();
	if (!kdtree || !feat || !config) {
		InvalidError();
		return NULL;
	}
	int k = spConfigGetKNN(config, conf_msg);
	returnIfConfigMsg(NULL)

	SPBPQueue bpq = spBPQueueCreate(k);
	if (!bpq) {
		MallocError()
		return NULL;
	}

	knnRecursive(kdtree, feat, bpq);

	int* ret = (int*) malloc(k * sizeof(int));
	if (!ret) {
		MallocError()
		spBPQueueDestroy(bpq);
		return NULL;
	}

	for(int i=0;i<k;i++){
		SPListElement tmp = spBPQueuePeek(bpq);
		ret[i] = spListElementGetIndex(tmp);
		spListElementDestroy(tmp);
		spBPQueueDequeue(bpq);
	}
	spBPQueueDestroy(bpq);

	return ret;
}






