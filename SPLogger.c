#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {

	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}

	logger = (SPLogger) malloc(sizeof(*logger));

	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}

	logger->level = level; //Set the level of the logger

	if (filename == NULL) { //In case the filename is not set use stdout

		logger->outputChannel = stdout;
		logger->isStdOut = true;

	} else { //Otherwise open the file in write mode

		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);

		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	declareLogMsg();
	if (!logger) {
		printError(DST_FAIL);
		return;
	}
	if (!logger->isStdOut) { //Close file only if not stdout
		printInfo(DST_LOGGER);
		fclose(logger->outputChannel);
	}
	free(logger); //free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintGeneral(const char* msg, const char* file,
		const char* function, const int line, const char* level) {

	if (logger == NULL) {
		return SP_LOGGER_UNDIFINED;
	}
	if (msg == NULL) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	CheckWF(fprintf(OC, LOG_TITLE, level))

	CheckWF(OUTPUT())

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line) {

	return spLoggerPrintGeneral(msg, file, function, line, ERROR_LOGGER);

}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line) {

	if (logger->level > SP_LOGGER_ERROR_LEVEL) {
		return spLoggerPrintGeneral(msg, file, function, line, WARNING);
	}

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg) {

	if (logger->level >= SP_LOGGER_INFO_WARNING_ERROR_LEVEL) {
		NULLChecks()

		CheckWF(fprintf(OC, LOG_TITLE, INFO))
		CheckWF(fprintf(OC,LOGR_MSG,msg))
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line) {

	if (logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) {
		return spLoggerPrintGeneral(msg, file, function, line, DEBUG);
	}

	return SP_LOGGER_SUCCESS;
}
SP_LOGGER_MSG spLoggerPrintMsg(const char* msg) {

	NULLChecks()

	CheckWF(fprintf(OC,STR,msg))

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerGetLevel(SP_LOGGER_LEVEL* levelAddress) {
	if (!logger)
		return SP_LOGGER_UNDIFINED;

	*levelAddress = logger->level;
	return SP_LOGGER_SUCCESS;
}

