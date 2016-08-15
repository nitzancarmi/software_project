
#ifndef MACROS_H_
#define MACROS_H_

/**********************************************************/
/************************ SPConfig ************************/
/**********************************************************/

#define MAX_LENGTH 1025

#define OK 0
#define ERROR -1
#define COMMENT_BLANK 2

#define DEFAULT_CANT_OPEN 			"The default configuration file spcbir.config couldn’t be open\n"
#define CANT_OPEN					"The configuration file %s couldn’t be ope\n"
#define MEMORY_FAIL1 				"Memory Allocation Failure - createAttributes\n"
#define UNDECLARED 					"Configuartion file must contain all of the following declerations:\n"\
									" - spImagesDirectory\n"\
									" - spImagesPrefix\n"\
									" - spImagesSuffix\n"\
									" - spNumOfImages\n"
#define INVALID_CONFLINE 			"File: %s\nLine: %d\nMessage: Invalid configuration line\n"

#define CONSTRAINT 					"File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n"
#define INVALID_COMLINE 			"Invalid command line : use -c <config_filename>\n"
#define PARAMETER_NOTSET 			"File: %s\nLine: %d\nMessage: Parameter %s is not set\n"


/**********************************************************/
/************************ Debugging ***********************/
/**********************************************************/


#define ERR_ARGS		"%s - at file: %s, line: %d, func: %s"
#define INVALID 		"Invalid Argument Error"
#define INVALID_WRN		"Invalid Argument Warning"
#define MALLOC_ERR 		"Memory Allocation Failure"
#define CONFIG_ERR 		"SPConfig Error"
#define LOGGER_ERR 		"SPLogger Error"
#define SYNTAX 			"Syntax Error while reading feature file. SKIPPING FILE."
#define CONFIG_MSG_ERR 	"Got SPConfig Error, MSG no. <%d>"
#define FEATS_FILE		"Cannot create feats file"
#define FEATS_PRNT		"Print to feats-file error"

#define errorWithLogger() 		fprintf(stderr,ERR_ARGS,LOGGER_ERR,__FILE__,__LINE__,__func__)

#define printError(msg)			if((*log_msg = spLoggerPrintError(msg,__FILE__,__func__,__LINE__))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();}


#define printWarning(msg)		if((*log_msg = spLoggerPrintWarning(msg,__FILE__,__func__,__LINE__))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();}


#define InvalidError() printError(INVALID)

#define MallocError() printError(MALLOC_ERR)

#define warningWithArgs(MSG,...) 	char err[1024] = {'\0'}; \
									sprintf(err,MSG,__VA_ARGS__); \
									printWarning(err)

#define loggerWithArgs(MSG,...) 	char err[1024] = {'\0'}; \
									sprintf(err,MSG,__VA_ARGS__); \
									printError(err)

#define returnIfConfigMsg(ret) if (*conf_msg != SP_CONFIG_SUCCESS) {	\
									loggerWithArgs(CONFIG_MSG_ERR,*conf_msg) \
									return ret;	\
								}

#define declareLogMsg() SP_LOGGER_MSG _log_msg = SP_LOGGER_SUCCESS, *log_msg = &_log_msg

/**************************************************************/
/************************ 	SPConfig    ***********************/
/**************************************************************/

#define ignoreSpaces(x)						while(isspace(*x)){x++;}

#define errorInvalidLine() 					printf(INVALID_CONFLINE, filename, lineNum)

#define assignString(configAttribute,xVal)	configAttribute = (char*) malloc(strlen(xVal)+1); \
											if(!configAttribute){							\
												return SP_CONFIG_ALLOC_FAIL;				\
											}												\
											strcpy(configAttribute,xVal)

#define checkAndAssign(property,x)			if(!attr->property)								\
												attr->property = x;

#define getter(property)					assert(msg);									\
	*msg = config ? SP_CONFIG_SUCCESS : SP_CONFIG_INVALID_ARGUMENT;							\
	return config ? config->property : 0 //maybe error


/********************************************************/
/************************ KDArray ***********************/
/********************************************************/


#define EPS		0.000001
#define arrayMallocFail(array,j)	 if(!array[j]){ \
										for(j--;j>=0;j--){ \
											free(array[j]); \
										}\
										free(array); \
										return NULL; \
										}
#define frees()							free(map1); free(map2); free(X);

/************************************************************/
/************************ SPExtractor ***********************/
/************************************************************/

#define MAX_FEATS_LINE	50000
#define READ_ERR 		"Error while reading the file. SKIPPING FILE"
#define CHAR_ERR		"Invalid characters found on file. SKIPPING FILE"
#define FEAT_ERR		"%s\n feature = %d, line = %d, char = %c <%d>\nline is:\n%s"
#define FEAT_ERR2		"Invalid characters found on file. SKIPPING FILE\n\tfeature = %d, line = %d, char = %c <%d>\n\tline is:\n%s"
#define FEAT_SIZE_ERR	"Current feature size is %d features while spCADimension = %d. SKIPPING FILE"
#define FEAT_SMALL 		"Feature size is smaller than spCADimension. SKIPPING FILE"
#define errorReturn()	*numOfFeatures = -1; fclose(feats);	return NULL;
#define FEATS_EXT 		".feats"
#define FILE_ERR		"File \"%s\" cannot be opened. SKIPPING FILE"
#define INDEX_ERR		"Image index inside file (%d) doesn't match filename index - \"%s\". SKIPPING FILE"
#define FEATS_QNTTY		"Features file %d features, while %d was the quantity defined. SKIPPING FILE"
#define FEATS_SUFFIX	"Error with the configuration:\nPath of .feats file cannot be retrieved"
#define NO_FEATS_IMPRT	"No image features were imported"
#define SOME_FEATS		"Some of the images features were not imported"

/*****************************************************/
/************************ Main ***********************/
/*****************************************************/

#define clearAll()			finishProgram(config, bpq, pc, kdarray, kdtree, all_points, img_near_cnt, \
								similar_images, all_points_size);




#endif /* MACROS_H_ */







