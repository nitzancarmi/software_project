#include "../SPLogger.h"
#include "../SPPoint.h"
#include "../SPConfig.h"
#include "../KDArray.h"
#include "unit_test_util.h"

int init_test(SPConfig attr) {
    
    /*create resources for init*/
    int rc = 0;
    SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
    SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;

    /*first check - given example*/
    printf("\n===1===\n");
    printf("Beginning in a basic check\n");
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

    printf("finished Basic check.\n===%s===\n\n", BASIC_CHECK ? "FAILED" : "SUCCEES");
    rc |= BASIC_CHECK;
    rc <<=1;
<<<<<<< 6ead5b45fb43acaf9d29c313db53ff40457fbda4
<<<<<<< 05cd8e61794cfcac6a24e6bb0c8fe51602f89383
=======
>>>>>>> minor fixes after memory allocation basic checks
    SPKDArrayDestroy(res);
    for(i = 4; i >= 0; i--)
        spPointDestroy(pa[i]);
    free(M[1]);
    free(M[0]);
    free(M);
<<<<<<< 6ead5b45fb43acaf9d29c313db53ff40457fbda4
=======
>>>>>>> kdarray_unit_test: finished basic test
=======
>>>>>>> minor fixes after memory allocation basic checks
    
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
	return 1;
    }

    return 0;
}

