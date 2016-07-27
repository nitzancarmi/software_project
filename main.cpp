#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>
extern "C" {
#include "KDArray.h"
}

int main(int argc, char* argv[]) {

	SPConfig config;
	SP_CONFIG_MSG conf_msg;
	SP_LOGGER_MSG log_msg;

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
	if ((log_msg = spLoggerCreate("stdio",
			SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL)) != SP_LOGGER_SUCCESS)
		return ERROR;
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

	SPKDArray kd = spKDArrayCreate(config, arr, 5, &log_msg, &conf_msg), kd1, kd2;

	return OK;
}

