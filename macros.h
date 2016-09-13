
#ifndef MACROS_H_
#define MACROS_H_

/**********************************************************/
/************************ SPConfig ************************/
/**********************************************************/

#define MAX_LENGTH 					1024

#define OK 							0
#define ERROR 						-1
#define ERR							1
#define COMMENT_BLANK 				2

#define DEFAULT_CANT_OPEN 			"The default configuration file spcbir.config couldn't be open\n"
#define CANT_OPEN					"The configuration file %s couldn't be open\n"
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

#define DEFAULT_CONFIG				"spcbir.config"
#define PCAYML						"pca.yml"
#define STD							"stdout"
#define	C_ARG						"-c"
#define CLS_QRY						"<>"
#define STR_LINE					"%s\n"

#define	IMGDIR						"spImagesDirectory"
#define IMGPRE						"spImagesPrefix"
#define IMGSUF						"spImagesSuffix"
#define IMGNUM						"spNumOfImages"
#define IMGCAD						"spPCADimension"
#define IMGPCA						"spPCAFilename"
#define IMGNOF						"spNumOfFeatures"
#define IMGEXM						"spExtractionMode"
#define TRUE_STR					"true"
#define FALSE_STR					"false"
#define IMGNOS						"spNumOfSimilarImages"
#define POS_FIRST_VAL				1
#define IMGSPM						"spKDTreeSplitMethod"
#define SPLIT_METHODS				"RANDOM", "MAX_SPREAD", "INCREMENTAL"
#define IMGKNN						"spKNN"
#define IMGMNG						"spMinimalGUI"
#define IMGLGL						"spLoggerLevel"
#define IMGLGF						"spLoggerFilename"

#define	POSSIBLE_SUFFIEXS			".jpg", ".png", ".bmp", ".gif"

#define CAD_DEFAULT					20
#define FEATS_DEFAULT				100
#define SIMILAR_DEFAULT				1
#define KNN_DEFAULT					1
#define	PCA_SIZE					8
#define	LOGFILE_SIZE				7

#define PATH_FRMT_STR				"%s%s%d%s"
#define TWO_STR						"%s%s"



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
#define PCA_ERR			"Error in getting PCA Dimensions"
#define KDT_START		"Entering Recursive creation of the KD Tree"
#define NO_CNFG			"No Configuration File"
#define NO_IMG_NUM		"Failed getting NumOfImages from configuration struct"
#define IMG_EXTR		"Extracting image number %d"
#define DBG_CREATE		"Creating .feats file for image"
#define KNN_FAIL		"Failed in finding nearest neighbors for features"
#define KNN_DO			"Finding nearest neighbors to each feature"
#define CLOS_IMGS		"Calculating the global closest images (most frequent appearances in closest per feature)"

#define errorWithLogger() 		fprintf(stderr,ERR_ARGS,LOGGER_ERR,__FILE__,__LINE__,__func__)

#define printError(msg)			if((*log_msg = spLoggerPrintError(msg,__FILE__,__func__,__LINE__))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();}

#define printWarning(msg)		if((*log_msg = spLoggerPrintWarning(msg,__FILE__,__func__,__LINE__))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();}

#define printInfo(msg)		        if((*log_msg = spLoggerPrintInfo(msg))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();}

#define printDebug(msg)	         	if((*log_msg = spLoggerPrintDebug(msg,__FILE__,__func__,__LINE__))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();}
#define printFinishProgram(rc)          sprintf(msg, "Program finished. Status: %s", rc ? "FAIL" : "SUCCESS"); \
                                             printInfo(msg)


#define InvalidError() printError(INVALID)

#define MallocError() printError(MALLOC_ERR)

#define infoWithArgs(MSG,...) 		char err[MAX_LENGTH] = {'\0'}; \
									sprintf(err,MSG,__VA_ARGS__); \
									printInfo(err)

#define warningWithArgs(MSG,...) 	char err[MAX_LENGTH] = {'\0'}; \
									sprintf(err,MSG,__VA_ARGS__); \
									printWarning(err)

#define loggerWithArgs(MSG,...) 	char err[MAX_LENGTH] = {'\0'}; \
									sprintf(err,MSG,__VA_ARGS__); \
									printError(err)

#define returnIfConfigMsg(ret) if (*conf_msg != SP_CONFIG_SUCCESS) {	\
									loggerWithArgs(CONFIG_MSG_ERR,*conf_msg) \
									return ret;	\
								}

#define declareLogMsg() SP_LOGGER_MSG _log_msg = SP_LOGGER_SUCCESS, *log_msg = &_log_msg
#define declareConfMsg() SP_CONFIG_MSG _conf_msg = SP_CONFIG_SUCCESS, *conf_msg = &_conf_msg

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
	return config ? config->property : -1 //maybe error

#define getterBoolean(property)				assert(msg);									\
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

#define MAX_FEATS_LINE		50000
#define READ_ERR 			"Error while reading the feats file. SKIPPING FILE"
#define CHAR_ERR			"Invalid characters found on file. SKIPPING FILE"
#define SPACE_ERR			"Empty lines are not permitted in the .feats file. SKIPPING FILE"
#define INDX_ERR			"Image index and/or number of features written in the .feats file cannot be negative. SKIPPING FILE"
#define FEAT_ERR			"%s\n feature = %d, line = %d, char = %c <%d>\nline is:\n%s"
#define FEAT_ERR2			"Invalid characters found on file. SKIPPING FILE\n\tfeature = %d, line = %d, char = %c <%d>\n\tline is:\n%s"
#define FEAT_SIZE_ERR		"Current feature size is %d features while spCADimension = %d. SKIPPING FILE"
#define FEAT_SMALL 			"Feature size is smaller than spCADimension. SKIPPING FILE"
#define errorReturn()		spPointArrayDestroy(pntsArray, countOfFeatures); \
							*numOfFeatures = -1; \
							fclose(feats);	\
							return NULL;
#define FEATS_EXT 			".feats"
#define FILE_ERR			"File \"%s\" cannot be opened. SKIPPING FILE"
#define INDEX_ERR			"Image index inside file (%d) doesn't match filename index - \"%s\". SKIPPING FILE"
#define FEATS_QNTTY_LESS	"Features file has %d features, less than %d, the quantity defined at the 2nd line. SKIPPING FILE"
#define FEATS_QNTTY_MORE	"Features file has more than %d features, the quantity defined at the 2nd line. SKIPPING FILE"
#define FEATS_SUFFIX		"Error with the configuration:\nPath of .feats file cannot be retrieved"
#define NO_FEATS_IMPRT		"No image features were imported"
#define SOME_FEATS			"Some of the images features were not imported"
#define INTS_FEATS_FILE		"%d\n%d\n"
#define FLOAT				"%f"
#define FLOAT_CHAR			"%f%c"

#define MAIN1				"Start Building a KD Tree"
#define MAIN2				"Importing Image features from files"
#define MAINFAIL1			"Failed Extracting Features from image files"
#define MAIN3				"Constructing KD Array out of the features"
#define MAINFAIL2			"Failed Constructing KD Array"
#define MAIN4				"Constructing KD Tree using the KD Array"

#define MAIN5				"Destroying Configuration parameters struct"
#define MAIN6				"Destroying KD-Tree"
#define MAIN7				"Destroying Logger"

#define MAIN8				"Cleaning query-oriented resources"
#define LOG_OPEN			"Logger opened successfully in file: %s"


/*****************************************************/
/************************ Main ***********************/
/*****************************************************/

#define clearAll()		cleanGlobalResources(config, kdtree);
#define LOGGERnMSG 		"SPLogger Error. Message Type: %d"
#define FINISH_PRG 		"Program Finished Successfully"

#define NO_FILE			"Image file \"%s\" doesn't exist"
#define PLS_ENTER		"Please enter an image path:\n"
#define INV_PATH		"Invalid path to image. Please try again;\n"
#define BST_CND			"Best candidates for - %s - are:\n"
#define EXIT			"Exiting...\n"
#define WRITE_MODE		"w"
#define READ_MODE		"r"

#define	EXTRCT_MODE		"Starting Extraction Mode"
#define EXTRCT_IMG_NUM	"Extracting image in index %d"
#define EXTRCT_FROM_IMG	"Extracting features from image"
#define EXPORT_TO_FILE	"Exporting image features into a file"
#define EXTRCT_SKIP		"Skipping Extraction Mode"

#define GLOBAL_RES		"Creating Global Resources"
#define	USR_QRY			"Start getting queries from user"
#define EXIT_SNGL		"Got Exiting String: \"<>\""
#define QRY_PATH_GOT	"Got query image path: %s"

#define QRY_GET			"Getting features for query image"
#define QRY_FEAT_ERR	"Failed in getting features for query image"
#define	IMG_EXTRCT		"Extracted %d features from image %s"

#define CLS_SRCH		"Searching for the closest images in database for the query image"
#define SRCH_FAIL		"Failed in searching for closest images"
#define RSLTS			"Now showing results"

#define checkifImgExists()	if(access(path, F_OK) == -1){ \
								char err[MAX_LENGTH] = {'\0'}; \
								sprintf(err,NO_FILE,path); \
								if((*log_msg = spLoggerPrintError(err,__FILE__,__func__,__LINE__))	\
									!= SP_LOGGER_SUCCESS){	\
										errorWithLogger();	\
								}	\
								clearAll() \
								return ERROR;	\
							}


/*********************************************************/
/************************ SPLogger ***********************/
/*********************************************************/

#define SP_LOGGER_OPEN_MODE "w"

#define OC logger->outputChannel

#define OUTPUT() 		fprintf(OC,"- file: %s\n- function: %s\n- line: %d\n- message: %s\n",file,function,line,msg)

#define CheckWF(cond) 	if(cond<0){	return SP_LOGGER_WRITE_FAIL; }

#define NULLChecks() 	if (logger == NULL) {	\
							return SP_LOGGER_UNDIFINED;	\
						}	\
						if (msg == NULL) {	\
							return SP_LOGGER_INVAlID_ARGUMENT;	}

#define	DST_FAIL		"Failed destroying logger. No active logger found"
#define DST_LOGGER		"Destroying logger"
#define LOG_TITLE		"---%s---\n"
#define ERROR_LOGGER	"ERROR"
#define WARNING			"WARNING"
#define INFO			"INFO"
#define LOGR_MSG		"- message: %s\n"
#define DEBUG			"DEBUG"
#define STR				"%s"


#endif /* MACROS_H_ */

