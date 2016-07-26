#include "KDArray.h"
#include "SPConfig.h"
#include "SPLogger.h"

/*** Struct Definition ***/
struct kd_array_t {
	
	SPPoint* pointArray;
	int** mat;
	
	int rows;
	int cols;
};
/****************************/

SPKDArray init (SPConfig attr, SPPoint *arr, int size) {
    int i;

    if(!arr || size<1)
	//TODO add logger message
        return NULL;

    /*create new struct SPKDArray*/
    SPKDArray kd = (SPKDArray)malloc(sizeof(*kd));
    if(!kd) {
	//TODO add logger message
        return NULL;
    } 

    /*copy array into struct*/
    kd->pointArray = (SPPoint*)malloc(sizeof(SPPoint) * size);
    if(!kd->pointArray){
	//TODO add logger message
	return NULL;
    }
    for(i=0; i<size; i++) {
	kd->pointArray[i] = spPointCopy(arr[i]);
        if(!kd->pointArray[i]) {
	    //TODO add logger message
            goto Cleanup;
        } 
    }

    /*create initialized matrix of size dims X size */
    int** M;
    int dims = spConfigGetPCADim(attr, msg);
    int* row;
    for(i=0; i<dims; i++) {
        row = (int*)malloc(size * sizeof(int));
	
    } 
    
    /*
Cleanup:
    //TODO to be implemented



}
