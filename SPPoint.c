/*
 * SPPoint.c
 *
 *  Created on: May 19, 2016
 *      Author: almog6564
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SPPoint.h"
#define EPS 0.0000001
struct sp_point_t {
	double* data;
	int index;
	int dim;
};

int spPointCompare(SPPoint a, SPPoint b) {
    /*checks if has the sime dimension*/
    int dim;
    if(a->dim != b->dim)
        return 1;
    else
        dim = a->dim;
    int i;
    int cmp=0;
    for (i=0;i<dim;i++){
        cmp += (a->data[i] - b->data[i]);
    }
    if (abs(cmp) > EPS)
        return 1;
    return a->index - b->index;
}
SPPoint spPointCreate(double* data, int dim, int index) {

	if (!data || dim <= 0 || index <= 0) {
		return NULL;
	}

	struct sp_point_t * p = (struct sp_point_t*) malloc(
			sizeof(struct sp_point_t));
	int i;

	if (p == NULL) {
		return NULL;
	}

	p->data = (double*) malloc(dim * sizeof(double));

	if (p->data == NULL) {
		free(p);
		return NULL;
	}

	p->dim = dim;
	p->index = index;

	for (i = 0; i < dim; i++) {
		p->data[i] = data[i];
	}

	return p;
}

SPPoint spPointCopy(SPPoint source) {

	assert(source != NULL);

	return spPointCreate(source->data, source->dim, source->index);
}

void spPointDestroy(SPPoint point) {
	if (point != NULL) {
		free(point->data);
		free(point);
	}

}

int spPointGetDimension(SPPoint point) {
	assert(point != NULL);

	return point->dim;
}

int spPointGetIndex(SPPoint point) {
	assert(point != NULL);

	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis) {

	assert(axis>=0);										//CHECK

	assert(point!=NULL);
	assert(axis < point->dim);

	return point->data[axis];

}


double spPointL2SquaredDistance(SPPoint p, SPPoint q){

	assert(p!=NULL);
	assert(q!=NULL);
	assert(p->dim == q->dim);

	double res = 0, localdist = 0;
	int j;

	//O(dim)
	for(j = 0; j < p->dim ; j++) {
		localdist = (spPointGetAxisCoor(p,j)-spPointGetAxisCoor(q,j));
		localdist *= localdist;
		res += localdist;
	}

	return res;
}


