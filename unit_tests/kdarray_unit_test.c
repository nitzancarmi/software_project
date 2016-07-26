#include "../SPLogger.h"
#include "../SPPoint.h"
#include "../SPConfig.h"
#include "unit_test_util.h"

int init_test(attr) {
    
    /*create resources for init*/
    int size;
    const char* logfile = "log.txt";
    SP_CONFIG_MSG conf_msg;
    sp_logger_msg log_msg;
    SP_LOGGER_LEVEL level = SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
    SP_LOGGER_MSG msg = spLoggerCreate(logfile, 
                                       level);
    if(msg)
        print("ERROR in logger bringup");
        return 1;

    /*first check - given example*/
    int i,j;
    double[2] a = {1.0,2.0};
    double[2] b = {123.0,70.0};
    double[2] c = {2.0,7.0};
    double[2] d = {9.0,11.0};
    double[2] e = {3.0,4.0};
    SPPoint arr[5];
    arr[0] = spPointCreate(a, 2, 1);
    arr[1] = spPointCreate(b, 2, 1);
    arr[2] = spPointCreate(c, 2, 1);
    arr[3] = spPointCreate(d, 2, 1);
    arr[4] = spPointCreate(e, 2, 1);
    int A[2][5] = {{0,2,4,3,1},
                   {0,4,2,3,1}};
    SPKDArray res = init(attr,
                         arr,
                         size,
                         &log_msg,
                         &conf_msg);

    ASSERT_TRUE(*(res->pointArray+0) == a);
    ASSERT_TRUE(*(res->pointArray+1) == b);
    ASSERT_TRUE(*(res->pointArray+2) == c);
    ASSERT_TRUE(*(res->pointArray+3) == d);
    ASSERT_TRUE(*(res->pointArray+4) == e);

    for(i=0;i<2;i++)
	for(j=0;j<5;j++)
	    ASSERT_TRUE(A[i][j] == 
            res->mat+i*sizeof(int)+j);


    return 0;
}
 
int main() {

    /*create resources */
    SPConfig attr; 
    SP_CONFIG_MSG msg;
    attr = spConfigCreate("../spcbir.config", &msg);
    if (!attr)
	return ERROR;
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

