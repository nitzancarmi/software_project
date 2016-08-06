#include "../SPLogger.h"
#include "../SPPoint.h"
#include "../SPConfig.h"
#include "../KDArray.h"
#include "unit_test_util.h"

int init_test(SPConfig attr) {

    printf("Strating the init_test\n----------------------\n");    
    /*create resources for init*/
    int rc = 0;
    SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
    SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;

    /*first check - given example*/
    printf("\n===1===\n");
    printf("Strating a basic check\n");
    printf("creating a new KDArray\n");
    int i,j;
    double a[2] = {1.0,2.0};
    double b[2] = {123.0,70.0};
    double c[2] = {2.0,7.0};
    double d[2] = {9.0,11.0};
    double e[2] = {3.0,4.0};
    printf("given points are {(1,2), (123,70), (2,7), (9,11), (3,4)}\n"); 
    SPPoint arr[5];
    int size = 5;
    arr[0] = spPointCreate(a, 2, 1);
    arr[1] = spPointCreate(b, 2, 1);
    arr[2] = spPointCreate(c, 2, 1);
    arr[3] = spPointCreate(d, 2, 1);
    arr[4] = spPointCreate(e, 2, 1);
    int A[2][5] = {{0,2,4,3,1},
                   {0,4,2,3,1}};
   
    SPKDArray res = spKDArrayCreate(attr,
                                    arr,
                                    size,
                                    &log_msg,
                                    &conf_msg);
    printf("finished creating KDArray (note: log and config msgs are left unchecked)\n");
    printf("Start checkups:\n");
    int BASIC_CHECK = 0;
    SPPoint *pa = getKDPointArray(res);
    int **M = getKDMat(res);
    for (i=0;i<5;i++) {
        if(spPointCompare(pa[i],arr[i])){
            printf("ERROR - in the %dth point\n", i);
            BASIC_CHECK = 1;
        }
    }

    for(i=0;i<2;i++) {
	for(j=0;j<5;j++) {
            if(A[i][j] != M[i][j]) {
                printf("ERROR - in M[%d][%d]\n", i,j);
                BASIC_CHECK = 1;
            }
        }
    }

    printf("finished Basic check.\n===%s===\n\n", BASIC_CHECK ? "FAIL" : "SUCCESS");
    rc |= BASIC_CHECK;
    rc <<=1;
    SPKDArrayDestroy(res);
    for(i = 4; i >= 0; i--)
        spPointDestroy(pa[i]);
    free(M[1]);
    free(M[0]);
    free(M);
    return rc;
}

 
int split_test(SPConfig attr) { 
    
    printf("\n\nStarting the split_test\n-----------------------\n");    
    /*create resources for init*/
    int rc = 0;
    SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
    SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;

    /*first check - given example*/
    int BASIC_CHECK = 0;
    printf("\n===1===\n");
    printf("Starting a basic check\n");
    printf("creating a new KDArray\n");
    double a[2] = {1.0,10.0};
    double b[2] = {123.0,70.0};
    double c[2] = {2.0,7.0};
    double d[2] = {9.0,11.0};
    double e[2] = {3.0,4.0};
    printf("given points are {(1,2), (123,70), (2,7), (9,11), (3,4)}\n"); 
    SPPoint arr[5];
    int size = 5;
    arr[0] = spPointCreate(a, 2, 1);
    arr[1] = spPointCreate(b, 2, 1);
    arr[2] = spPointCreate(c, 2, 1);
    arr[3] = spPointCreate(d, 2, 1);
    arr[4] = spPointCreate(e, 2, 1);
    SPKDArray res = spKDArrayCreate(attr,
                                    arr,
                                    size,
                                    &log_msg,
                                    &conf_msg);
    printf("finished creating KDArray (note: log and config msgs are left unchecked)\n");

    SPKDArray L=NULL,R=NULL;
    printf("Splitting the given KDArray\n");
    BASIC_CHECK = spKDArraySplit(res, 0, &L, &R, &log_msg, &conf_msg);
    printf("\nL:\n---\n");
    printKDArrayMatrix(L);
    printf("\nR:\n---\n");
    printKDArrayMatrix(R);
    printf("freeing the initial KDArray, leaving only its splitted arrays\n");
    SPKDArrayDestroy(res);
    rc |= BASIC_CHECK;
    rc <<= 1;

    /*starting a continous check*/
    int CONT_CHECK = 0;
    printf("using already created splitted arrays L,R - now splitting them again\n");
    printf("by X coordinate:\n----------------\n");
    SPKDArray LR, LL, RL, RR;
    CONT_CHECK = spKDArraySplit(L, 0, &LL, &LR, &log_msg, &conf_msg);
    printf("\nLL:\n---\n");
    printKDArrayMatrix(LL);
    printf("\nLR:\n---\n");
    printKDArrayMatrix(LR);
    CONT_CHECK |= spKDArraySplit(R, 0, &RL, &RR, &log_msg, &conf_msg); 
    printf("\nRLL:\n---\n");
    printKDArrayMatrix(RL);
    printf("\nRR:\n---\n");
    printKDArrayMatrix(RR);

/*
    SPKDArrayDestroy(LL);
    SPKDArrayDestroy(LR);
    SPKDArrayDestroy(RL);
    SPKDArrayDestroy(RR);
*/
    printf("\nby Y coordinate:\n----------------\n");
    SPKDArray YLR, YLL, YRL, YRR;
    CONT_CHECK = spKDArraySplit(L, 0, &YLL, &YLR, &log_msg, &conf_msg);
    printf("\nLL:\n---\n");
    printKDArrayMatrix(YLL);
    printf("\nLR:\n---\n");
    printKDArrayMatrix(YLR);
    CONT_CHECK |= spKDArraySplit(R, 0, &YRL, &YRR, &log_msg, &conf_msg); 
    printf("\nRLL:\n---\n");
    printKDArrayMatrix(YRL);
    printf("\nRR:\n---\n");
    printKDArrayMatrix(YRR);
/*    SPKDArrayDestroy(YLL);
    SPKDArrayDestroy(YLR);
    SPKDArrayDestroy(YRL);
    SPKDArrayDestroy(YRR);

    printf("freeing the splitted KD arrays R and L\n");
    SPKDArrayDestroy(L);
    SPKDArrayDestroy(R);
    rc |= CONT_CHECK;
*/
    return rc;
       
}
int main() {

    /*create resources */
    SPConfig attr; 
    SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
    attr = spConfigCreate("../spcbir.config", &msg);
    if (!attr){
        printf("Failed to create attributes. problem is: %d\n", msg);
	return 1;
    }
    else
        printAttributes(attr);

    /*start init_test*/
    int rc = init_test(attr);
    if(rc){
	printf("FAIL - init_test");
	return rc;
    }

    rc = split_test(attr);
    if (rc){
        printf("FAIL - split_test");
        return rc;
    }

    return 0;
}

