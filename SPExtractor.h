/*
 * SPExtractor.h
 *
 *  Created on: Aug 8, 2016
 *      Author: almog6564
 */

#ifndef SPEXTRACTOR_H_
#define SPEXTRACTOR_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "SPPoint.h"
#include "SPConfig.h"
#include "KDArray.h"



#define SYNTAX "Syntax Error while reading feature file. SKIPPING FILE."

#define printError(msg)		if((*log_msg = spLoggerPrintError(msg,__FILE__,__func__,__LINE__))!= SP_LOGGER_SUCCESS){	\
								fprintf(stderr,ERR_ARGS,LOGGER_ERR,__FILE__,__LINE__,__func__);}

#define printWarning(msg)		if((*log_msg = spLoggerPrintWarning(msg,__FILE__,__func__,__LINE__))!= SP_LOGGER_SUCCESS){	\
								fprintf(stderr,ERR_ARGS,LOGGER_ERR,__FILE__,__LINE__,__func__);}

/*
 * exports image properties to a unique file
 * @param pa = SPPoint array contains the image's features
 * @param size = size of pa
 * @param image_index = the index of the image in its directory
 * @param config = configuration attributes
 *
 * @return = 0 for success, 1 for failure
 */
int exportImageToFile(SPPoint* pa, int size, int image_index, SPConfig config);

//TODO DOCO
SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config,
		SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg);

#endif /* SPEXTRACTOR_H_ */
