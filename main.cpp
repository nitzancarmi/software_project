#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>
extern "C" {
#include "KDArray.h"
}
void teardown(SPConfig config) {
	spLoggerPrintInfo("cleaning allocated logger and configibutes");
	spLoggerDestroy();
	spConfigDestroy(config); 

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
        else {
           log_msg = spLoggerPrintDebug("finished azquiring attributes from config file, uploaded logger successfully",
                                    __FILE__,
                                    __func__,
                                    __LINE__);
           if(log_msg != SP_LOGGER_SUCCESS) {
               printf("Got Error msg from logger, No. %d\nMoving to Teatdown\n", log_msg);
               teardown(config);
           }
        }

	log_msg = spLoggerPrintInfo("program has finished successfully. moving to teardown");
        if(log_msg != SP_LOGGER_SUCCESS) {
            spLoggerPrintError("getting error msg from logger",
                                __FILE__,
                                __func__,
                                __LINE__);
        }
        printf("program has finished successfully\n");
        teardown(config);
	return OK;
}

