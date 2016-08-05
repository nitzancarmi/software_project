#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>
extern "C" {
#include "KDTree.h"
}

void teardown(SPConfig config) {
	spLoggerPrintInfo("cleaning allocated logger and configibutes");
	spLoggerDestroy();
	spConfigDestroy(config);

}

int main(int argc, char* argv[]) {

	SPConfig config;
	SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;

	switch (argc) {

	case 1:
		config = spConfigCreate("spcbir.config", &conf_msg);
		if (!config)
			return ERROR;
		break;

	case 3:

		if (strcmp(argv[1], "-c")) {
			printf(INVALID_COMLINE);
			return ERROR;
		}
		config = spConfigCreate(argv[2], &conf_msg);
		if (!config)
			return ERROR;

		break;

	default:
		config = NULL;
		printf(INVALID_COMLINE);
		break;
	}
	if ((log_msg = spLoggerCreate(NULL, SP_LOGGER_WARNING_ERROR_LEVEL))
			!= SP_LOGGER_SUCCESS)
		return ERROR;
	else {
		log_msg =
				spLoggerPrintDebug(
						"finished azquiring attributes from config file, uploaded logger successfully",
						__FILE__, __func__,
						__LINE__);
		if (log_msg != SP_LOGGER_SUCCESS) {
			printf("Got Error msg from logger, No. %d\nMoving to Teardown\n",
					log_msg);
			teardown(config);
		}
	}

	double data1[2] = { 1, 2 }, data2[2] = { 123, 70 }, data3[2] = { 2, 7 },
			data4[2] = { 9, 11 }, data5[2] = { 3, 4 };
	SPPoint a = spPointCreate(data1, 2, 1);
	SPPoint b = spPointCreate(data2, 2, 2);
	SPPoint c = spPointCreate(data3, 2, 3);
	SPPoint d = spPointCreate(data4, 2, 4);
	SPPoint e = spPointCreate(data5, 2, 5);

	SPPoint* arr = (SPPoint*) malloc(5 * sizeof(SPPoint));
	arr[0] = a;
	arr[1] = b;
	arr[2] = c;
	arr[3] = d;
	arr[4] = e;

	SPKDArray kd = spKDArrayCreate(config, arr, 5, &log_msg, &conf_msg);
//	printKDArrayMatrix(kd);
//	printf("=========\n");
	SPKDTreeNode kdtree = spKDTreeCreate(kd, config, &conf_msg, &log_msg);

//	SPKDArray kd = spKDArrayCreate(config, arr, 2, &log_msg, &conf_msg), kd1 = NULL, kd2 =NULL;
//	printKDArrayMatrix(kd);
//	spKDArraySplit(kd, 1, &kd1, &kd2, &log_msg, &conf_msg);

	spKDTreeDestroy(kdtree);

	SPKDArrayDestroy(kd);
	for (int i = 0; i < 5; i++) {
		spPointDestroy(arr[i]);
	}
	free(arr);

	/***************************/
	log_msg = spLoggerPrintInfo(
			"program has finished successfully. moving to teardown");
	if (log_msg != SP_LOGGER_SUCCESS) {
		spLoggerPrintError("getting error msg from logger",
		__FILE__, __func__,
		__LINE__);
	}
	printf("program has finished successfully\n");
	teardown(config);
	return OK;
}

//Deprecated Checks - to insert in the future to kdarray_unit_test

//double data1[2] = { 1, 2 }, data2[2] = { 123, 70 }, data3[2] = { 2, 7 },
//			data4[2] = { 9, 11 }, data5[2] = { 3, 4 };
//	SPPoint a = spPointCreate(data1, 2, 1);
//	SPPoint b = spPointCreate(data2, 2, 2);
//	SPPoint c = spPointCreate(data3, 2, 3);
//	SPPoint d = spPointCreate(data4, 2, 4);
//	SPPoint e = spPointCreate(data5, 2, 5);
//
//	SPPoint* arr = (SPPoint*) malloc(5 * sizeof(SPPoint));
//	arr[0] = a;
//	arr[1] = b;
//	arr[2] = c;
//	arr[3] = d;
//	arr[4] = e;
//
//	SPKDArray kd = spKDArrayCreate(config, arr, 5, &log_msg, &conf_msg), kd1,
//			kd2;
//	printKDArrayMatrix(kd);
//	spKDArraySplit(kd, 0, &kd1, &kd2, &log_msg, &conf_msg);
//
//	printKDArrayMatrix(kd1);
//	printKDArrayMatrix(kd2);
//
//	SPKDArrayDestroy(kd);
//	SPKDArrayDestroy(kd1);
//	SPKDArrayDestroy(kd2);
//	for(int i=0;i<5;i++){
//		spPointDestroy(arr[i]);
//	}
//	free(arr);
//	spConfigDestroy(config);

