#include "../SPLogger.h"
#include "../SPPoint.h"
#include "../SPConfig.h"
#include "../KDArray.h"
#include "unit_test_util.h"

int spKDArrayCompare(SPKDArray A, SPKDArray B) {
    int i,j;
    int rc = 0;

    int rows = getKDRows(A);
    if(rows != getKDRows(B))
        return 1;
    int cols = getKDCols(A);
    if(cols != getKDCols(B))
        return 1;

    SPPoint *A_pa = getKDPointArray(A);
    SPPoint *B_pa = getKDPointArray(B);
    for (i=0;i<cols;i++) {
        if(spPointCompare(A_pa[i],B_pa[i])){
            rc = 1;
        }
        spPointDestroy(A_pa[i]);
        spPointDestroy(B_pa[i]);
    }
    free(A_pa);
    free(B_pa);
    

    int** AM = getKDMat(A);
    int** BM = getKDMat(B);
    for(i=0;i<rows;i++) {
        for(j=0;j<cols;j++) {
            if(AM[i][j] != BM[i][j])
                rc=1;
        }
    }
    free(AM);
    free(BM);
    return rc;
}

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
    int BASIC_CHECK=0;
    printf("\n===1===\n");
    printf("Starting a basic check\n");
    printf("creating a new KDArray\n");
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
    SPKDArray res = spKDArrayCreate(attr,
                                    arr,
                                    size,
                                    &log_msg,
                                    &conf_msg);
    printf("finished creating the KDArray (note: log and config msgs are left unchecked)\n");

    SPKDArray L=NULL,R=NULL;
    printf("Splitting the given KDArray\n");
    BASIC_CHECK = spKDArraySplit(res, 0, &L, &R, &log_msg, &conf_msg);
//    printf("\nL:\n---\n");
//    printKDArrayMatrix(L);
//    printf("\nR:\n---\n");
//    printKDArrayMatrix(R);
    SPKDArrayDestroy(res);

    printf("Starting checkups.");
    //Left KDArray
    SPPoint leftArr[3] = {arr[0], arr[2],arr[4]}; 
    size=3;
    SPKDArray L_sol = spKDArrayCreate(attr,
                                      leftArr,
                                      size,
                                      &log_msg,
                                      &conf_msg);
    BASIC_CHECK |= spKDArrayCompare(L,L_sol);
    SPKDArrayDestroy(L_sol);
    printf(".....");

    //Right KDArray
    SPPoint rightArr[2] = {arr[1], arr[3]}; 
    size=2;
    SPKDArray R_sol = spKDArrayCreate(attr,
                                      rightArr,
                                      size,
                                      &log_msg,
                                      &conf_msg);
    BASIC_CHECK |= spKDArrayCompare(R,R_sol);
    SPKDArrayDestroy(R_sol);
    printf(".....");
    printf("Complete!\n");
    

    printf("finished Basic check.\n===%s===\n\n", BASIC_CHECK ? "FAIL" : "SUCCESS");
    rc |= BASIC_CHECK;
    rc <<=1;


    /*continous check*/
    int CONT_CHECK = 0;
    printf("using already created splitted arrays L,R - now splitting them again\n");
    printf("by X coordinate:");
    SPKDArray LR, LL, RL, RR;
    CONT_CHECK = spKDArraySplit(L, 0, &LL, &LR, &log_msg, &conf_msg);
    CONT_CHECK |= spKDArraySplit(R, 0, &RL, &RR, &log_msg, &conf_msg);
    printf(".....");

    SPPoint LL_arr[2] = {arr[0], arr[2]}; 
    size=2;
    SPKDArray LL_sol = spKDArrayCreate(attr,
                                       LL_arr,
                                       size,
                                       &log_msg,
                                       &conf_msg);
    CONT_CHECK |= spKDArrayCompare(LL,LL_sol);
    SPKDArrayDestroy(LL_sol);
    printf(".....");

    SPPoint LR_arr[1] = {arr[4]}; 
    size=1;
    SPKDArray LR_sol = spKDArrayCreate(attr,
                                       LR_arr,
                                       size,
                                       &log_msg,
                                       &conf_msg);
    CONT_CHECK |= spKDArrayCompare(LR,LR_sol);
    SPKDArrayDestroy(LR_sol);
    printf(".....");
    SPKDArrayDestroy(LL);
    SPKDArrayDestroy(LR);
    SPKDArrayDestroy(L);
    printf("COMPLETE!\n");
/*
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
    SPKDArrayDestroy(YLL);
    SPKDArrayDestroy(YLR);
    SPKDArrayDestroy(YRL);
    SPKDArrayDestroy(YRR);

    printf("freeing the splitted KD arrays R and L\n");
    SPKDArrayDestroy(L);
*/    SPKDArrayDestroy(R);
    rc |= CONT_CHECK;

    return rc;
       
}
int main2() {

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

