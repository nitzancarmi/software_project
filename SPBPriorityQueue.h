#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * 	Allocates a new empty Bounded Priority Queue.
 *
 *  @param maxSize   maximum capacity (the upper bound of queue size) of the queue.
 *  @return
 *  NULL in case maxSize<=0 || memory allocation fails
 *  A new queue in case of success
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * 	Creates a deep and complete copy from an existing queue.
 *
 *  @param source   existing queue
 *  @return
 *  NULL in case source == NULL || memory allocation fails
 *  A copy of the source queue.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates an existing queue.
 * Clears all elements and field by using the free function.
 *
 * @param source Target queue to be deallocated. If queue is NULL nothing will be done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from source queue.
 * The elements are deallocated using the stored freeing function
 *
 * @param source -  Target queue to remove all element from. If queue is NULL nothing will be done
 *
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a queue.
 *
 * @param source - The target source of which its size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximum capacity of elements in a queue.
 *
 * @param source - The target source of which its maximum size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the maximum number of elements the queue can contain.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Inserting a copy of the element to the source queue.
 * After inserting queue will be sorted by value, than by index.
 *
 * @param source - The target source queue to be enqueued into.
 * @param element - element to be enqueued
 *
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if NULL was sent as at least one of the arguments
 * SP_BPQUEUE_FULL - If source if full and the value of element is bigger than the last element of source
 * 						Or the source is full and the value equals to the last's value, but has bigger index.
 * SP_BPQUEUE_SUCCESS - if insert was successful (Otherwise).
 *
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes from the target queue the element with the lowest value.
 *
 * @param source - The target source of which its maximum size is requested.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if NULL was sent as an argument
 * SP_BPQUEUE_EMPTY - if an source queue is empty
 * SP_BPQUEUE_SUCCESS - if remove was successful.
 *
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);


/**
 * Returns a copy of the lowest value in the queue (the first element).
 *
 * @param source - the priority queue
 * @return
 * If the queue is empty (source.size == 0), or the source is not valid (NULL), or source-> is not valid, return NULL
 * otherwise, return a copy of the first element in the queue, using spListElementCopy(source)
 */
SPListElement spBPQueuePeek(SPBPQueue source);



/**
 * Returns a copy of the highest value in the queue (the last element).
 *
 * @param source - the priority queue
 * @return
 * If the queue is empty (source.size == 0), or the source is not valid (NULL), or source-> is not valid, return NULL
 * otherwise, return a copy of the last element in the queue, using spListElementCopy(source)
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);



/**
 * the function finds the first element in the queue and returns its value.
 *
 * @param source - the priority queue
 * @pre source->list->value > 0
 *
 * @return
 * (-1) - If the queue is empty (source.size == 0), or the source is not valid (NULL), or source-> is not valid
 * otherwise, return the value of the first element, using SPListGetFirst(source)
 */
double spBPQueueMinValue(SPBPQueue source);


/**
 * the function finds the last element in the queue and returns its value.
 * @param source - the priority queue
 * @pre source->list->value > 0
 * @return
 * (-1) - If the queue is empty (source.size == 0), or the source is not valid (NULL), or source-> is not valid, return -1
 * otherwise, return the value of the last element, using SPListGetLast(source)
 */
double spBPQueueMaxValue(SPBPQueue source);




/**
 * the function returns true if the priority queue source is empty, AKA size of queue equals 0
 * @param source - the priority queue
 * @return
 * True if the queue  is empty (source.size == 0)), and false otherwise.
 * the function deals NULL input as false.
 */
bool spBPQueueIsEmpty(SPBPQueue source);



/**
 * the function returns true if the priority queue source is full, and false otherwise
 * @param source - the priority queue
 * @return
 * True if the queue is full (source.size == source.maxsize), and false otherwise.
 * the function deals NULL input as false.
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
