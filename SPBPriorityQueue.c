#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "SPList.h"
#include "SPBPriorityQueue.h"
/*Macro for checking NULL and returning NULL if is*/
#define NULLCheck(something) if (something == NULL) { 	return NULL; }

SP_BPQUEUE_MSG convertMessage(SP_LIST_MSG);

struct sp_bp_queue_t {
	SPList list;
	int maxSize;
};

SPBPQueue spBPQueueCreate(int maxSize) {

	if (maxSize <= 0) {
		return NULL;
	}

	/* Memory allocation for PBQueue */
	struct sp_bp_queue_t * q = (struct sp_bp_queue_t*) malloc(
			sizeof(struct sp_bp_queue_t));
	NULLCheck(q)

	/* Creating new elements in struct */
	q->list = spListCreate();
	if (!q->list) {
		free(q);
		return NULL;
	}

	q->maxSize = maxSize;
	return q;
}

SPBPQueue spBPQueueCopy(SPBPQueue source) {

	NULLCheck(source)

	/* Memory allocation for PBQueue */
	struct sp_bp_queue_t * newQ = (struct sp_bp_queue_t*) malloc(
			sizeof(struct sp_bp_queue_t));
	NULLCheck(newQ)

	newQ->list = spListCopy(source->list);
	if (newQ->list == NULL) {
		free(newQ);
		return NULL;
	}

	newQ->maxSize = source->maxSize;
	return newQ;
}

void spBPQueueDestroy(SPBPQueue source) {

	if (source != NULL) {
		spListDestroy(source->list);
		free(source);
	}
}

void spBPQueueClear(SPBPQueue source) {
	if (source != NULL) {
		if (source->list) {
			spListClear(source->list);
		}
	}
}

int spBPQueueSize(SPBPQueue source) {
	if (source) {
		if (source->list) {
			return spListGetSize(source->list);
		}
	}
	return -1;
}

int spBPQueueGetMaxSize(SPBPQueue source) {
	if (source) {
		return source->maxSize;
	}
	return -1;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element) {
	double value;
	
	if (!source || !element || !source->list) {
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	SP_BPQUEUE_MSG msg = SP_BPQUEUE_SUCCESS;
	value = spListElementGetValue(element);

	/* If list is empty , insert at start*/
	if (spBPQueueIsEmpty(source)) {
		msg = convertMessage(spListInsertFirst(source->list, element));
	}
	/* If  the value is bigger than the biggest insert him last*/
	else if (value > spBPQueueMaxValue(source)) {

		if (spBPQueueSize(source) != spBPQueueGetMaxSize(source)) {
			msg = convertMessage(spListInsertLast(source->list, element));
		}

		else { /* If also, the list is full, do nothing */
			msg = SP_BPQUEUE_FULL;
		}

	} else {

		SPListElement curr = spListGetFirst(source->list);

		/* Iterating through list until finding place for element*/
		while (curr
				&& spListElementGetValue(curr) < spListElementGetValue(element)) {
			curr = spListGetNext(source->list);
		}

		/* Checks if value is equal to the next one's value
		 * if so, inserting smaller index first
		 * */
		while (curr
				&& spListElementGetValue(element) == spListElementGetValue(curr)
				&& (spListElementGetIndex(element) > spListElementGetIndex(curr))) {
			curr = spListGetNext(source->list);
		}

		/* inserting element to right place in list */
		if (curr) {
			msg = convertMessage(
					spListInsertBeforeCurrent(source->list, element));
		}

		/* if reaches end of queue */
		else {
			msg = convertMessage(spListInsertLast(source->list, element));
		}

	}

	/* updating last element if size limit breached */
	if (spListGetSize(source->list) > spBPQueueGetMaxSize(source)) {

		spListGetLast(source->list);
		spListRemoveCurrent(source->list);
	}

	return msg;
}

double spBPQueueMinValue(SPBPQueue source) {
	if (source != NULL) {
		if (source->list != NULL) {
			return spListElementGetValue(spListGetFirst(source->list));
		}
	}
	return -1;
}

double spBPQueueMaxValue(SPBPQueue source) {
	if (source) {
		if (source->list) {
			return spListElementGetValue(spListGetLast(source->list));
		}
	}
	return -1;

}

bool spBPQueueIsEmpty(SPBPQueue source) {
	if (source != NULL) {
		if (source->list != NULL) {
			return (spBPQueueSize(source) == 0);
		}
	}
	return false;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {

	if (source != NULL) {
		if (source->list != NULL) {
			if (spBPQueueIsEmpty(source)) {
				return SP_BPQUEUE_EMPTY;
			} else {
				/* If queue is not empty destroy the first element,
				 * automatically assigns new "first"
				 */
				spListGetFirst(source->list);
				spListRemoveCurrent(source->list);
				return SP_BPQUEUE_SUCCESS;
			}
		}
	}
	return SP_BPQUEUE_INVALID_ARGUMENT;
}

SPListElement spBPQueuePeek(SPBPQueue source) {
	if (source != NULL) {
		if (source->list != NULL) {
			if (spBPQueueIsEmpty(source)) {
				return NULL;
			} else {

				return spListElementCopy(spListGetFirst(source->list));
			}
		}
	}
	return NULL;
}

SPListElement spBPQueuePeekLast(SPBPQueue source) {
	if (source != NULL) {
		if (source->list != NULL) {
			if (spBPQueueIsEmpty(source)) {
				return NULL;
			} else {

				return spListElementCopy(spListGetLast(source->list));
			}
		}
	}
	return NULL;
}

bool spBPQueueIsFull(SPBPQueue source) {
	if (source != NULL) {
		if (source->list != NULL) {
			return (spBPQueueSize(source) == spBPQueueGetMaxSize(source));
		}
	}
	return false;
}

SP_BPQUEUE_MSG convertMessage(SP_LIST_MSG m) {
	switch (m) {
	case SP_LIST_SUCCESS:
		return SP_BPQUEUE_SUCCESS;
	case SP_LIST_OUT_OF_MEMORY:
		return SP_BPQUEUE_OUT_OF_MEMORY;
	default:
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
}
